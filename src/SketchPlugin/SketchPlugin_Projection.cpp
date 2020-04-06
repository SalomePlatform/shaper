// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include <SketchPlugin_Projection.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_BSpline.h>
#include <SketchPlugin_BSplinePeriodic.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Ellipse.h>
#include <SketchPlugin_EllipticArc.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_ConstraintRigid.h>

#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeDoubleArray.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Events.h>

#include <Events_Loop.h>

#include <GeomAPI_BSpline.h>
#include <GeomAPI_Circ.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Ellipse.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Vertex.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_Projection.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Point2DArray.h>

#include <cmath>

static const double tolerance = 1.e-7;


SketchPlugin_Projection::SketchPlugin_Projection()
    : SketchPlugin_SketchEntity(),
      myIsComputing(false)
{
}

void SketchPlugin_Projection::initDerivedClassAttributes()
{
  data()->addAttribute(EXTERNAL_FEATURE_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(PROJECTED_FEATURE_ID(), ModelAPI_AttributeRefAttr::typeId());
  data()->attribute(PROJECTED_FEATURE_ID())->setIsArgument(false);

  data()->addAttribute(EXTERNAL_ID(), ModelAPI_AttributeSelection::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EXTERNAL_ID());

  data()->addAttribute(INCLUDE_INTO_RESULT(), ModelAPI_AttributeBoolean::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), AUXILIARY_ID());
}

void SketchPlugin_Projection::execute()
{
  AttributeRefAttrPtr aRefAttr = data()->refattr(PROJECTED_FEATURE_ID());
  if (!aRefAttr || !aRefAttr->isInitialized())
    return;
  FeaturePtr aProjection = ModelAPI_Feature::feature(aRefAttr->object());

  if (!lastResult().get()) {
    bool hasProjResult = aProjection->lastResult().get() != NULL;
    ResultConstructionPtr aConstr = document()->createConstruction(data());
    if (hasProjResult)
      aConstr->setShape(aProjection->lastResult()->shape());
    aConstr->setIsInHistory(false);
    aConstr->setDisplayed(false);
    setResult(aConstr);

    if (hasProjResult)
      aProjection->selection(EXTERNAL_ID())->setValue(lastResult(), lastResult()->shape());
  }

  // is sketch plane is changed (issue 1791), attribute of projection is not changed, but
  // projection must be fully recomputed
  computeProjection(EXTERNAL_FEATURE_ID());
}

void SketchPlugin_Projection::attributeChanged(const std::string& theID)
{
  if ((theID == EXTERNAL_FEATURE_ID() || theID == EXTERNAL_ID()) && !myIsComputing) {
    myIsComputing = true;
    computeProjection(theID);
    myIsComputing = false;
  }
}

static const std::set<std::string>& POINT_PROJECTION()
{
  static std::set<std::string> aProj;
  if (aProj.empty())
    aProj.insert(SketchPlugin_Point::ID());
  return aProj;
}

static const std::set<std::string>& LINE_PROJECTION()
{
  static std::set<std::string> aProj;
  if (aProj.empty())
    aProj.insert(SketchPlugin_Line::ID());
  return aProj;
}

static const std::set<std::string>& CIRCLE_ELLIPSE_PROJECTION()
{
  static std::set<std::string> aProj;
  if (aProj.empty()) {
    aProj.insert(SketchPlugin_Circle::ID());
    aProj.insert(SketchPlugin_Ellipse::ID());
  }
  return aProj;
}

static const std::set<std::string>& ARC_PROJECTION()
{
  static std::set<std::string> aProj;
  if (aProj.empty()) {
    aProj.insert(SketchPlugin_Arc::ID());
    aProj.insert(SketchPlugin_EllipticArc::ID());
  }
  return aProj;
}

static const std::set<std::string>& BSPLINE_PROJECTION()
{
  static std::set<std::string> aProj;
  if (aProj.empty()) {
    aProj.insert(SketchPlugin_BSpline::ID());
    aProj.insert(SketchPlugin_BSplinePeriodic::ID());
  }
  return aProj;
}


static const std::set<std::string>& possibleProjectionTypes(GeomEdgePtr theEdge,
                                                            GeomVertexPtr theVertex)
{
  if (theVertex)
    return POINT_PROJECTION();
  if (theEdge) {
    if (theEdge->isLine())
      return LINE_PROJECTION();
    else if (theEdge->isCircle() || theEdge->isArc() || theEdge->isEllipse()) {
      if (theEdge->isClosed())
        return CIRCLE_ELLIPSE_PROJECTION();
      else
        return ARC_PROJECTION();
    }
    else
      return BSPLINE_PROJECTION();
  }
  static const std::set<std::string> DUMMY;
  return DUMMY;
}

void SketchPlugin_Projection::computeProjection(const std::string& theID)
{
  AttributeSelectionPtr aExtFeature =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(attribute(EXTERNAL_FEATURE_ID()));

  GeomShapePtr aShape;
  GeomEdgePtr anEdge;
  GeomVertexPtr aVertex;
  if (aExtFeature)
    aShape = aExtFeature->value();
  if (!aShape && aExtFeature->context())
    aShape = aExtFeature->context()->shape();
  if (aShape) {
    if (aShape->isEdge())
      anEdge = GeomEdgePtr(new GeomAPI_Edge(aShape));
    else if (aShape->isVertex())
      aVertex = GeomVertexPtr(new GeomAPI_Vertex(aShape));
  }
  if (!anEdge && !aVertex)
    return;

  const std::set<std::string>& aProjType = possibleProjectionTypes(anEdge, aVertex);

  AttributeRefAttrPtr aRefAttr = data()->refattr(PROJECTED_FEATURE_ID());
  FeaturePtr aProjection;
  if (aRefAttr && aRefAttr->isInitialized())
    aProjection = ModelAPI_Feature::feature(aRefAttr->object());

  // if the type of feature differs with already selected, remove it and create once again
  bool isRebuild = rebuildProjectedFeature(aProjection, aProjType);

  ResultConstructionPtr aResult =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(lastResult());
  if (!isRebuild && aResult && aResult->shape() && theID == EXTERNAL_FEATURE_ID()) {
    aResult->setShape(std::shared_ptr<GeomAPI_Edge>());
    if (aProjection)
      aProjection->selection(EXTERNAL_ID())->setValue(ResultPtr(), GeomShapePtr());
  }

  keepCurrentFeature();

  bool isProjected = false;
  if (aVertex)
    isProjected = projectPoint(aProjection, aVertex->point());
  else
    isProjected = projectEdge(aProjection, anEdge);

  if (!isProjected)
    return; // projection is not computed, stop processing

  aProjection->boolean(COPY_ID())->setValue(true);
  aProjection->execute();
  aRefAttr->setObject(aProjection);

  restoreCurrentFeature();

  if (theID == EXTERNAL_FEATURE_ID()) {
    selection(EXTERNAL_ID())->selectValue(aExtFeature);

    if (aResult) {
      aResult->setShape(aProjection->lastResult()->shape());
      setResult(aResult);
      GeomShapePtr anEmptyVal;
      aProjection->selection(EXTERNAL_ID())->setValue(lastResult(), anEmptyVal);

      static const Events_ID anEvent = Events_Loop::eventByName(EVENT_VISUAL_ATTRIBUTES);
      ModelAPI_EventCreator::get()->sendUpdated(aProjection, anEvent, false);
    }
  }
}

bool SketchPlugin_Projection::rebuildProjectedFeature(
    FeaturePtr& theProjection,
    const std::set<std::string>& theSupportedTypes,
    const std::string& theRequestedFeature)
{
  bool isRebuild = false;
  if (theProjection &&
      (theSupportedTypes.find(theProjection->getKind()) == theSupportedTypes.end() ||
      (!theRequestedFeature.empty() && theProjection->getKind() != theRequestedFeature))) {
    DocumentPtr aDoc = sketch()->document();

    AttributeRefAttrPtr aRefAttr = data()->refattr(PROJECTED_FEATURE_ID());
    aRefAttr->setObject(data()->owner()); // to not remove of this remove reference to aProjection
    std::set<FeaturePtr> aFeaturesToBeRemoved;
    aFeaturesToBeRemoved.insert(theProjection);
    ModelAPI_Tools::removeFeaturesAndReferences(aFeaturesToBeRemoved);
    theProjection = FeaturePtr();
    aRefAttr->setObject(theProjection);
    isRebuild = true;
  }

  if (!theProjection && !theRequestedFeature.empty())
    theProjection = sketch()->addFeature(theRequestedFeature);
  return isRebuild;
}

bool SketchPlugin_Projection::projectPoint(FeaturePtr& theProjection, const GeomPointPtr& thePoint)
{
  std::shared_ptr<GeomAPI_Pln> aSketchPlane = sketch()->plane();

  std::shared_ptr<GeomAPI_Pnt> aPrjPnt = aSketchPlane->project(thePoint);
  std::shared_ptr<GeomAPI_Pnt2d> aPntInSketch = sketch()->to2D(aPrjPnt);

  rebuildProjectedFeature(theProjection, POINT_PROJECTION(), SketchPlugin_Point::ID());

  // update coordinates of projection
  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theProjection->attribute(SketchPlugin_Point::COORD_ID()))->setValue(aPntInSketch);
  return true;
}

bool SketchPlugin_Projection::projectSegment(FeaturePtr& theProjection, const GeomEdgePtr& theEdge)
{
  std::shared_ptr<GeomAPI_Pln> aSketchPlane = sketch()->plane();

  std::shared_ptr<GeomAPI_Pnt> aFirst = aSketchPlane->project(theEdge->firstPoint());
  std::shared_ptr<GeomAPI_Pnt> aLast = aSketchPlane->project(theEdge->lastPoint());

  std::shared_ptr<GeomAPI_Pnt2d> aFirstInSketch = sketch()->to2D(aFirst);
  std::shared_ptr<GeomAPI_Pnt2d> aLastInSketch = sketch()->to2D(aLast);
  if (aFirstInSketch->distance(aLastInSketch) < tolerance)
    return false; // line is semi-orthogonal to the sketch plane

  rebuildProjectedFeature(theProjection, LINE_PROJECTION(), SketchPlugin_Line::ID());

  // update attributes of projection
  std::shared_ptr<GeomDataAPI_Point2D> aStartPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theProjection->attribute(SketchPlugin_Line::START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aEndPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theProjection->attribute(SketchPlugin_Line::END_ID()));
  aStartPnt->setValue(aFirstInSketch);
  aEndPnt->setValue(aLastInSketch);

  return true;
}

bool SketchPlugin_Projection::projectEdge(FeaturePtr& theProjection, const GeomEdgePtr& theEdge)
{
  if (theEdge->isLine())
    return projectSegment(theProjection, theEdge);

  std::shared_ptr<GeomAPI_Pln> aSketchPlane = sketch()->plane();

  GeomAlgoAPI_Projection aProjAlgo(aSketchPlane);
  GeomCurvePtr aProjectedCurve = aProjAlgo.project(theEdge);

  bool isOk = false;
  if (aProjectedCurve->isCircle()) {
    if (aProjectedCurve->isTrimmed()) {
      // ARC is a projection
      isOk = fillArc(theProjection, aProjectedCurve, aSketchPlane);
    }
    else {
      // CIRCLE is a projection
      isOk = fillCircle(theProjection, aProjectedCurve, aSketchPlane);
    }
  }
  else if (aProjectedCurve->isEllipse()) {
    if (aProjectedCurve->isTrimmed()) {
      // ELLIPTIC ARC is a projection
      isOk = fillEllipticArc(theProjection, aProjectedCurve, aSketchPlane);
    }
    else {
      // ELLIPSE is a projection
      isOk = fillEllipse(theProjection, aProjectedCurve, aSketchPlane);
    }
  }
  else
    isOk = fillBSpline(theProjection, aProjectedCurve, aSketchPlane);

  return isOk;
}

bool SketchPlugin_Projection::fillArc(FeaturePtr& theProjection,
                                      const GeomCurvePtr& theArc,
                                      const GeomPlanePtr& thePlane)
{
  rebuildProjectedFeature(theProjection, ARC_PROJECTION(), SketchPlugin_Arc::ID());

  GeomAPI_Circ aCircle(theArc);

  double aNormalsDot = aCircle.normal()->dot(thePlane->direction());
  if (fabs(fabs(aNormalsDot) - 1.0) > tolerance)
    return false; // arc is not in the plane, parallel to the sketch plane

  bool isInversed = aNormalsDot < 0.;

  GeomPointPtr aCenter = thePlane->project(aCircle.center());
  GeomPnt2dPtr aCenterInSketch = sketch()->to2D(aCenter);

  GeomPointPtr aFirst = theArc->getPoint(theArc->startParam());
  GeomPnt2dPtr aFirstInSketch = sketch()->to2D(thePlane->project(aFirst));

  GeomPointPtr aLast = theArc->getPoint(theArc->endParam());
  GeomPnt2dPtr aLastInSketch = sketch()->to2D(thePlane->project(aLast));

  bool aWasBlocked = theProjection->data()->blockSendAttributeUpdated(true);

  // update attributes of projection
  std::shared_ptr<GeomDataAPI_Point2D> aCenterPnt =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theProjection->attribute(SketchPlugin_Arc::CENTER_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aStartPnt =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theProjection->attribute(SketchPlugin_Arc::START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aEndPnt =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theProjection->attribute(SketchPlugin_Arc::END_ID()));
  aStartPnt->setValue(aFirstInSketch);
  aEndPnt->setValue(aLastInSketch);
  aCenterPnt->setValue(aCenterInSketch);
  theProjection->boolean(SketchPlugin_Arc::REVERSED_ID())->setValue(isInversed);

  theProjection->data()->blockSendAttributeUpdated(aWasBlocked);
  return true;
}

bool SketchPlugin_Projection::fillCircle(FeaturePtr& theProjection,
                                         const GeomCurvePtr& theCircle,
                                         const GeomPlanePtr& thePlane)
{
  rebuildProjectedFeature(theProjection, CIRCLE_ELLIPSE_PROJECTION(), SketchPlugin_Circle::ID());

  GeomAPI_Circ aCircle(theCircle);
  GeomPointPtr aCenter = thePlane->project(aCircle.center());
  GeomPnt2dPtr aCenterInSketch = sketch()->to2D(aCenter);

  // update attributes of projection
  std::shared_ptr<GeomDataAPI_Point2D> aCenterPnt =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theProjection->attribute(SketchPlugin_Circle::CENTER_ID()));
  aCenterPnt->setValue(aCenterInSketch);
  theProjection->real(SketchPlugin_Circle::RADIUS_ID())->setValue(aCircle.radius());
  return true;
}

bool SketchPlugin_Projection::fillEllipse(FeaturePtr& theProjection,
                                          const GeomCurvePtr& theEllipse,
                                          const GeomPlanePtr& thePlane)
{
  rebuildProjectedFeature(theProjection, CIRCLE_ELLIPSE_PROJECTION(), SketchPlugin_Ellipse::ID());

  GeomAPI_Ellipse anEllipse(theEllipse);
  GeomPointPtr aCenter = thePlane->project(anEllipse.center());
  GeomPnt2dPtr aCenterInSketch = sketch()->to2D(aCenter);
  GeomPointPtr aFocus = thePlane->project(anEllipse.firstFocus());
  GeomPnt2dPtr aFocusInSketch = sketch()->to2D(aFocus);

  // update attributes of projection
  std::shared_ptr<GeomDataAPI_Point2D> aCenterPnt =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theProjection->attribute(SketchPlugin_Ellipse::CENTER_ID()));
  aCenterPnt->setValue(aCenterInSketch);
  std::shared_ptr<GeomDataAPI_Point2D> aFocusPnt =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theProjection->attribute(SketchPlugin_Ellipse::FIRST_FOCUS_ID()));
  aFocusPnt->setValue(aFocusInSketch);
  theProjection->real(SketchPlugin_Ellipse::MINOR_RADIUS_ID())->setValue(anEllipse.minorRadius());
  return true;
}

bool SketchPlugin_Projection::fillEllipticArc(FeaturePtr& theProjection,
                                              const GeomCurvePtr& theEllipticArc,
                                              const GeomPlanePtr& thePlane)
{
  rebuildProjectedFeature(theProjection, ARC_PROJECTION(), SketchPlugin_EllipticArc::ID());

  GeomAPI_Ellipse anEllipse(theEllipticArc);

  double aNormalsDot = anEllipse.normal()->dot(thePlane->direction());
  if (fabs(fabs(aNormalsDot) - 1.0) > tolerance)
    return false; // arc is not in the plane, parallel to the sketch plane

  bool isInversed = aNormalsDot < 0.;

  GeomPointPtr aCenter = thePlane->project(anEllipse.center());
  GeomPnt2dPtr aCenterInSketch = sketch()->to2D(aCenter);
  GeomPointPtr aFocus = thePlane->project(anEllipse.firstFocus());
  GeomPnt2dPtr aFocusInSketch = sketch()->to2D(aFocus);

  GeomPointPtr aFirst = theEllipticArc->getPoint(theEllipticArc->startParam());
  GeomPnt2dPtr aFirstInSketch = sketch()->to2D(thePlane->project(aFirst));
  GeomPointPtr aLast = theEllipticArc->getPoint(theEllipticArc->endParam());
  GeomPnt2dPtr aLastInSketch = sketch()->to2D(thePlane->project(aLast));

  bool aWasBlocked = theProjection->data()->blockSendAttributeUpdated(true);

  // update attributes of projection
  std::shared_ptr<GeomDataAPI_Point2D> aCenterPnt =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theProjection->attribute(SketchPlugin_EllipticArc::CENTER_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aFocusPnt =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theProjection->attribute(SketchPlugin_EllipticArc::FIRST_FOCUS_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aStartPnt =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theProjection->attribute(SketchPlugin_EllipticArc::START_POINT_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aEndPnt =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theProjection->attribute(SketchPlugin_EllipticArc::END_POINT_ID()));
  aStartPnt->setValue(aFirstInSketch);
  aEndPnt->setValue(aLastInSketch);
  aCenterPnt->setValue(aCenterInSketch);
  aFocusPnt->setValue(aFocusInSketch);
  theProjection->boolean(SketchPlugin_EllipticArc::REVERSED_ID())->setValue(isInversed);

  theProjection->data()->blockSendAttributeUpdated(aWasBlocked);
  return true;
}

bool SketchPlugin_Projection::fillBSpline(FeaturePtr& theProjection,
                                          const GeomCurvePtr& theCurve,
                                          const GeomPlanePtr& thePlane)
{
  GeomAPI_BSpline aBSpline(theCurve);

  rebuildProjectedFeature(theProjection, BSPLINE_PROJECTION(),
      aBSpline.isPeriodic() ? SketchPlugin_BSplinePeriodic::ID() : SketchPlugin_BSpline::ID());

  theProjection->integer(SketchPlugin_BSpline::DEGREE_ID())->setValue(aBSpline.degree());

  AttributePoint2DArrayPtr aPolesAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2DArray>(
      theProjection->attribute(SketchPlugin_BSpline::POLES_ID()));
  std::list<GeomPointPtr> aPoles = aBSpline.poles();
  aPolesAttr->setSize((int)aPoles.size());
  std::list<GeomPointPtr>::iterator anIt = aPoles.begin();
  for (int anIndex = 0; anIt != aPoles.end(); ++anIt, ++anIndex) {
    GeomPnt2dPtr aPoleInSketch = sketch()->to2D(*anIt);
    aPolesAttr->setPnt(anIndex, aPoleInSketch);
  }

  AttributeDoubleArrayPtr aWeightsAttr =
      theProjection->data()->realArray(SketchPlugin_BSpline::WEIGHTS_ID());
  std::list<double> aWeights = aBSpline.weights();
  if (aWeights.empty()) { // rational B-spline
    int aSize = (int)aPoles.size();
    aWeightsAttr->setSize(aSize);
    for (int anIndex = 0; anIndex < aSize; ++anIndex)
      aWeightsAttr->setValue(anIndex, 1.0);
  }
  else { // non-rational B-spline
    aWeightsAttr->setSize((int)aWeights.size());
    std::list<double>::iterator anIt = aWeights.begin();
    for (int anIndex = 0; anIt != aWeights.end(); ++anIt, ++anIndex)
      aWeightsAttr->setValue(anIndex, *anIt);
  }

  AttributeDoubleArrayPtr aKnotsAttr =
      theProjection->data()->realArray(SketchPlugin_BSpline::KNOTS_ID());
  std::list<double> aKnots = aBSpline.knots();
  int aSize = (int)aKnots.size();
  aKnotsAttr->setSize(aSize);
  std::list<double>::iterator aKIt = aKnots.begin();
  for (int index = 0; index < aSize; ++index, ++aKIt)
    aKnotsAttr->setValue(index, *aKIt);

  AttributeIntArrayPtr aMultsAttr =
      theProjection->data()->intArray(SketchPlugin_BSpline::MULTS_ID());
  std::list<int> aMultiplicities = aBSpline.mults();
  aSize = (int)aMultiplicities.size();
  aMultsAttr->setSize(aSize);
  std::list<int>::iterator aMIt = aMultiplicities.begin();
  for (int index = 0; index < aSize; ++index, ++aMIt)
    aMultsAttr->setValue(index, *aMIt);

  return true;
}
