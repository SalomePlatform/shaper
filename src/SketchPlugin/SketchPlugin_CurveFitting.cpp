// Copyright (C) 2020  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <SketchPlugin_CurveFitting.h>

#include <SketchPlugin_BSpline.h>
#include <SketchPlugin_BSplinePeriodic.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_MacroBSpline.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Tools.h>
#include <SketchPlugin_Sketch.h>

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeDoubleArray.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeRefAttrList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <Events_InfoMessage.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Point2DArray.h>

#include <GeomAlgoAPI_CurveBuilder.h>
#include <GeomAlgoAPI_EdgeBuilder.h>

#include <GeomAPI_BSpline.h>

static void convertTo3D(SketchPlugin_Sketch* theSketch,
                        const AttributeRefAttrListPtr& theAttribute,
                        bool theClosedButNotPeriodic,
                        std::list<GeomPointPtr>& thePoints);

static GeomEdgePtr buildInterpolationCurve(SketchPlugin_Sketch* theSketch,
                                           AttributeRefAttrListPtr thePoints,
                                           bool thePeriodic,
                                           bool theClosed);
static GeomEdgePtr buildApproximationCurve(SketchPlugin_Sketch* theSketch,
                                           AttributeRefAttrListPtr thePoints,
                                           double thePrecision,
                                           bool thePeriodic,
                                           bool theClosed);


SketchPlugin_CurveFitting::SketchPlugin_CurveFitting()
  : SketchPlugin_SketchEntity()
{
}

void SketchPlugin_CurveFitting::initDerivedClassAttributes()
{
  data()->addAttribute(POINTS_ID(), ModelAPI_AttributeRefAttrList::typeId());

  data()->addAttribute(TYPE_ID(), ModelAPI_AttributeString::typeId());

  data()->addAttribute(PRECISION_ID(), ModelAPI_AttributeDouble::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), PRECISION_ID());

  data()->addAttribute(NEED_CONTROL_POLYGON_ID(), ModelAPI_AttributeBoolean::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(),
                                                               NEED_CONTROL_POLYGON_ID());

  data()->addAttribute(PERIODIC_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(CLOSED_ID(), ModelAPI_AttributeBoolean::typeId());
}

void SketchPlugin_CurveFitting::execute()
{
  FeaturePtr aBSpline = createBSplineFeature();
  // create control polygon
  AttributeBooleanPtr aNeedControlPoly = boolean(NEED_CONTROL_POLYGON_ID());
  if (aNeedControlPoly && aNeedControlPoly->isInitialized() && aNeedControlPoly->value()) {
    bool isPeriodic = boolean(PERIODIC_ID())->value();
    std::list<FeaturePtr> aControlPoles;
    SketchPlugin_MacroBSpline::createControlPolygon(aBSpline, isPeriodic, aControlPoles);
  }
  // constraints for the selected points
  createConstraints(aBSpline);
}

FeaturePtr SketchPlugin_CurveFitting::createBSplineFeature()
{
  // create transient curve if not created yet
  if (!myTransientResult) {
    getAISObject(AISObjectPtr());
    if (!myTransientResult)
      return FeaturePtr();
  }

  SketchPlugin_Sketch* aSketch = sketch();
  if (!aSketch)
    return FeaturePtr();

  bool isPeriodic = boolean(PERIODIC_ID())->value();

  FeaturePtr aBSplineFeature = aSketch->addFeature(
      isPeriodic ? SketchPlugin_BSplinePeriodic::ID() : SketchPlugin_BSpline::ID());

  // Convert transient edge to B-spline curve.
  GeomCurvePtr aCurve = std::make_shared<GeomAPI_Curve>(myTransientResult);
  std::shared_ptr<GeomAPI_BSpline> aBSpline = std::make_shared<GeomAPI_BSpline>(aCurve);

  // Fill attributes of B-spline feature:
  bool aWasBlocked = aBSplineFeature->data()->blockSendAttributeUpdated(true, false);
  // 1. Degree
  aBSplineFeature->integer(SketchPlugin_BSplineBase::DEGREE_ID())->setValue(aBSpline->degree());
  // 2. Poles
  std::list<GeomPointPtr> aPoles = aBSpline->poles();
  AttributePoint2DArrayPtr aPolesAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2DArray>(
      aBSplineFeature->attribute(SketchPlugin_BSplineBase::POLES_ID()));
  aPolesAttr->setSize((int)aPoles.size());
  int anIndex = 0;
  for (auto it = aPoles.begin(); it != aPoles.end(); ++it, ++anIndex)
    aPolesAttr->setPnt(anIndex, aSketch->to2D(*it));
  // 3. Weights
  std::list<double> aWeights = aBSpline->weights();
  AttributeDoubleArrayPtr aWeightsAttr =
      aBSplineFeature->data()->realArray(SketchPlugin_BSplineBase::WEIGHTS_ID());
  if (aWeights.empty()) {
    aWeightsAttr->setSize((int)aPoles.size());
    for (anIndex = 0; anIndex < (int)aPoles.size(); ++anIndex)
      aWeightsAttr->setValue(anIndex, 1.0);
  }
  else {
    aWeightsAttr->setSize((int)aWeights.size());
    anIndex = 0;
    for (auto it = aWeights.begin(); it != aWeights.end(); ++it, ++anIndex)
      aWeightsAttr->setValue(anIndex, *it);
  }
  // 4. Knots (normalized from 0 to 1)
  std::list<double> aKnots = aBSpline->knots();
  AttributeDoubleArrayPtr aKnotsAttr =
      aBSplineFeature->data()->realArray(SketchPlugin_BSplineBase::KNOTS_ID());
  aKnotsAttr->setSize((int)aKnots.size());
  double aFirstKnot = aKnots.front();
  double aLastKnot = aKnots.back();
  anIndex = 0;
  for (auto it = aKnots.begin(); it != aKnots.end(); ++it, ++anIndex)
    aKnotsAttr->setValue(anIndex, (*it - aFirstKnot) / (aLastKnot - aFirstKnot));
  // 5. Multiplicities
  std::list<int> aMults = aBSpline->mults();
  AttributeIntArrayPtr aMultsAttr =
      aBSplineFeature->data()->intArray(SketchPlugin_BSplineBase::MULTS_ID());
  aMultsAttr->setSize((int)aMults.size());
  anIndex = 0;
  for (auto it = aMults.begin(); it != aMults.end(); ++it, ++anIndex)
    aMultsAttr->setValue(anIndex, *it);

  if (!isPeriodic) {
    AttributePoint2DPtr aStartPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aBSplineFeature->attribute(SketchPlugin_BSpline::START_ID()));
    aStartPoint->setValue(aPolesAttr->pnt(0));

    AttributePoint2DPtr aEndPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aBSplineFeature->attribute(SketchPlugin_BSpline::END_ID()));
    aEndPoint->setValue(aPolesAttr->pnt(aPolesAttr->size() - 1));
  }

  aBSplineFeature->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->setValue(
      boolean(AUXILIARY_ID())->value());

  aBSplineFeature->data()->blockSendAttributeUpdated(aWasBlocked);

  aBSplineFeature->execute();

  return aBSplineFeature;
}

void SketchPlugin_CurveFitting::createConstraints(FeaturePtr theProducedFeature)
{
  if (!theProducedFeature)
    return;

  SketchPlugin_Sketch* aSketch = sketch();
  ResultPtr aResult = theProducedFeature->lastResult();
  bool isPeriodic = boolean(PERIODIC_ID())->value();
  bool isClosed = boolean(CLOSED_ID())->value();
  bool isApproximation = string(TYPE_ID())->value() == TYPE_APPROXIMATION_ID();

  AttributeRefAttrListPtr aPointsAttr = refattrlist(POINTS_ID());
  std::list<std::pair<ObjectPtr, AttributePtr> > aPointsList = aPointsAttr->list();
  std::list<std::pair<ObjectPtr, AttributePtr> >::iterator aLastIt = --aPointsList.end();
  for (auto it = aPointsList.begin(); it != aPointsList.end(); ++it) {
    AttributePtr anAttr = it->second;
    if (!anAttr) {
      // maybe the SketchPoint is selected
      FeaturePtr aFeature = ModelAPI_Feature::feature(it->first);
      if (aFeature && aFeature->getKind() == SketchPlugin_Point::ID())
        anAttr = aFeature->attribute(SketchPlugin_Point::COORD_ID());
      else
        continue;
    }

    if (!isPeriodic && it == aPointsList.begin()) {
      SketchPlugin_Tools::createConstraintAttrAttr(aSketch,
          SketchPlugin_ConstraintCoincidence::ID(),
          anAttr, theProducedFeature->attribute(SketchPlugin_BSpline::START_ID()));
      if (isClosed) {
        // end of B-spline curve should be coincident with the first selected point
        SketchPlugin_Tools::createConstraintAttrAttr(aSketch,
            SketchPlugin_ConstraintCoincidence::ID(),
            anAttr, theProducedFeature->attribute(SketchPlugin_BSpline::END_ID()));
      }
    }
    else if (!isPeriodic && !isClosed && it == aLastIt) {
      SketchPlugin_Tools::createConstraintAttrAttr(aSketch,
          SketchPlugin_ConstraintCoincidence::ID(),
          anAttr, theProducedFeature->attribute(SketchPlugin_BSpline::END_ID()));
    }
    else if (!isApproximation) {
      SketchPlugin_Tools::createConstraintAttrObject(aSketch,
          SketchPlugin_ConstraintCoincidence::ID(),
          anAttr, aResult);
    }
  }
}

bool SketchPlugin_CurveFitting::customAction(const std::string& theActionId)
{
  bool isOk = true;
  if (theActionId == REORDER_POINTS_ACTION_ID()) {
    reorderPoints();
  } else {
    static const std::string MESSAGE("Error: Feature \"%1\" does not support action \"%2\".");
    Events_InfoMessage("SketchPlugin_CurveFitting", MESSAGE)
        .arg(getKind()).arg(theActionId).send();
    isOk = false;
  }
  return isOk;
}


void SketchPlugin_CurveFitting::reorderPoints()
{
  AttributeRefAttrListPtr aPointsAttr = refattrlist(POINTS_ID());
  bool isPeriodic = boolean(PERIODIC_ID())->value();
  bool isClosed = boolean(CLOSED_ID())->value();

  std::list<GeomPointPtr> aCoordinates;
  convertTo3D(sketch(), aPointsAttr, !isPeriodic && isClosed, aCoordinates);

  // to keep mapping between points and attributes
  std::map<GeomPointPtr, std::pair<ObjectPtr, AttributePtr> > aMap;
  std::list<std::pair<ObjectPtr, AttributePtr> > aPointsList = aPointsAttr->list();

  std::list<GeomPointPtr>::iterator aCoordIt = aCoordinates.begin();
  std::list<std::pair<ObjectPtr, AttributePtr> >::iterator anAttrIt = aPointsList.begin();
  for (; aCoordIt != aCoordinates.end() && anAttrIt != aPointsList.end(); ++aCoordIt, ++anAttrIt)
    aMap[*aCoordIt] = *anAttrIt;

  // reorder points
  GeomAlgoAPI_CurveBuilder::reorderPoints(aCoordinates);

  // re-compose the attribute
  bool aWasBlocked = data()->blockSendAttributeUpdated(true);
  aPointsAttr->clear();
  for (aCoordIt = aCoordinates.begin(); aCoordIt != aCoordinates.end(); ++aCoordIt) {
    const std::pair<ObjectPtr, AttributePtr>& aValue = aMap.at(*aCoordIt);
    if (aValue.second)
      aPointsAttr->append(aValue.second);
    else
      aPointsAttr->append(aValue.first);
  }
  data()->blockSendAttributeUpdated(aWasBlocked);
}

AISObjectPtr SketchPlugin_CurveFitting::getAISObject(AISObjectPtr thePrevious)
{
  SketchPlugin_Sketch* aSketch = sketch();
  if (!aSketch)
    return AISObjectPtr();

  std::string aType = string(TYPE_ID())->value();
  if (aType == TYPE_INTERPOLATION_ID()) {
    myTransientResult = buildInterpolationCurve(aSketch,
                                                refattrlist(POINTS_ID()),
                                                boolean(PERIODIC_ID())->value(),
                                                boolean(CLOSED_ID())->value());
  }
  else if (aType == TYPE_APPROXIMATION_ID()) {
    myTransientResult = buildApproximationCurve(aSketch,
                                                refattrlist(POINTS_ID()),
                                                real(PRECISION_ID())->value(),
                                                boolean(PERIODIC_ID())->value(),
                                                boolean(CLOSED_ID())->value());
  }
  if (!myTransientResult)
    return AISObjectPtr();

  AISObjectPtr anAIS = thePrevious;
  if (!anAIS)
    anAIS.reset(new GeomAPI_AISObject());
  anAIS->createShape(myTransientResult);

  // Modify attributes
  SketchPlugin_Tools::customizeFeaturePrs(anAIS, boolean(AUXILIARY_ID())->value());

  return anAIS;
}


// ===============================     Auxiliary functions     ====================================

void convertTo3D(SketchPlugin_Sketch* theSketch,
                 const AttributeRefAttrListPtr& theAttribute,
                 bool theClosedButNotPeriodic,
                 std::list<GeomPointPtr>& thePoints)
{
  std::list<std::pair<ObjectPtr, AttributePtr> > aPointsList = theAttribute->list();
  for (auto it = aPointsList.begin(); it != aPointsList.end(); ++it) {
    AttributePtr anAttr = it->second;
    if (!anAttr) {
      // maybe the SketchPoint is selected
      FeaturePtr aFeature = ModelAPI_Feature::feature(it->first);
      if (aFeature && aFeature->getKind() == SketchPlugin_Point::ID())
        anAttr = aFeature->attribute(SketchPlugin_Point::COORD_ID());
      else {
        thePoints.clear();
        break;
      }
    }

    AttributePoint2DPtr aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr);
    if (aPoint) {
      GeomPointPtr aPnt3D = theSketch->to3D(aPoint->x(), aPoint->y());
      thePoints.push_back(aPnt3D);
    }
  }

  if (theClosedButNotPeriodic && !thePoints.empty() &&
      thePoints.front()->distance(thePoints.back()) > 1.e-7)
    thePoints.push_back(thePoints.front()); // close the curve
}

GeomEdgePtr buildInterpolationCurve(SketchPlugin_Sketch* theSketch,
                                    AttributeRefAttrListPtr thePoints,
                                    bool thePeriodic,
                                    bool theClosed)
{
  std::list<GeomPointPtr> aCoordinates;
  convertTo3D(theSketch, thePoints, !thePeriodic && theClosed, aCoordinates);

  GeomEdgePtr aResult;
  if (aCoordinates.size() > 1) {
    static const bool isReorder = false;
    static GeomDirPtr aStartEndDir;
    aResult = GeomAlgoAPI_CurveBuilder::edge(aCoordinates, thePeriodic,
                                             isReorder, aStartEndDir, aStartEndDir);
  }
  return aResult;
}

GeomEdgePtr buildApproximationCurve(SketchPlugin_Sketch* theSketch,
                                    AttributeRefAttrListPtr thePoints,
                                    double thePrecision,
                                    bool thePeriodic,
                                    bool theClosed)
{
  std::list<GeomPointPtr> aCoordinates;
  convertTo3D(theSketch, thePoints, !thePeriodic && theClosed, aCoordinates);

  GeomEdgePtr aResult;
  if (aCoordinates.size() > 1)
    aResult = GeomAlgoAPI_CurveBuilder::approximate(aCoordinates, thePeriodic, thePrecision);
  return aResult;
}
