// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include <SketchSolver_ConstraintPerpendicular.h>
#include <SketchSolver_Error.h>

#include <PlaneGCSSolver_EdgeWrapper.h>
#include <PlaneGCSSolver_PointWrapper.h>
#include <PlaneGCSSolver_Storage.h>
#include <PlaneGCSSolver_Tools.h>
#include <PlaneGCSSolver_UpdateCoincidence.h>

#include <GeomAPI_Circ2d.h>
#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Ellipse2d.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintMiddle.h>

#include <cmath>

#define GCS_EDGE_WRAPPER(x) std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(x)
#define GCS_POINT_WRAPPER(x) std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(x)

/// \brief Obtain constrained features
static void getFeatures(const ConstraintPtr& theConstraint,
                        FeaturePtr& theFeature1,
                        FeaturePtr& theFeature2);

/// \brief Check whether the entities has only one shared point or less.
///        Return list of coincident points.
static std::set<AttributePtr> coincidentBoundaryPoints(FeaturePtr theFeature1,
                                                       FeaturePtr theFeature2);

/// \brief Collect points coincident with each of two features
static std::set<AttributePtr> coincidentPoints(FeaturePtr theFeature1, FeaturePtr theFeature2);

static void calculateIntersectionPoint(EntityWrapperPtr theCurve1, EntityWrapperPtr theCurve2,
                                       GCSPointPtr& theIntersectionPoint);

/// sets angle to 90 or -90 degrees
static void adjustAngleBetweenCurves(const GCSCurvePtr& theCurve1,
                                     const GCSCurvePtr& theCurve2,
                                     const GCSPointPtr& thePoint,
                                     double*            theAngle);


void SketchSolver_ConstraintPerpendicular::process()
{
  cleanErrorMsg();
  if (!myBaseConstraint || !myStorage) {
    // Not enough parameters are assigned
    return;
  }

  EntityWrapperPtr aValue;
  std::vector<EntityWrapperPtr> anAttributes;
  SketchSolver_Constraint::getAttributes(aValue, anAttributes);
  if (!myErrorMsg.empty())
    return;

  rebuild();
  if (!myErrorMsg.empty())
    return;

  myStorage->subscribeUpdates(this, PlaneGCSSolver_UpdateCoincidence::GROUP());
}

void SketchSolver_ConstraintPerpendicular::rebuild()
{
  if (mySolverConstraint)
    myStorage->removeConstraint(myBaseConstraint);

  std::shared_ptr<PlaneGCSSolver_Storage> aStorage =
      std::dynamic_pointer_cast<PlaneGCSSolver_Storage>(myStorage);

  mySolverConstraint = ConstraintWrapperPtr();
  mySharedPoint = AttributePtr();
  if (myAuxPoint) {
    GCS::SET_pD aParams = PlaneGCSSolver_Tools::parameters(myAuxPoint);
    aStorage->removeParameters(aParams);
    myAuxPoint = EntityWrapperPtr();
  }

  // Check the quantity of entities of each type and their order (arcs first)
  int aNbLines = 0;
  int aNbCircles = 0;
  int aNbOther = 0;
  std::list<EntityWrapperPtr>::iterator anEntIt = myAttributes.begin();
  for (; anEntIt != myAttributes.end(); ++anEntIt) {
    if (!(*anEntIt).get())
      continue;
    if ((*anEntIt)->type() == ENTITY_LINE)
      ++aNbLines;
    else if ((*anEntIt)->type() == ENTITY_ARC || (*anEntIt)->type() == ENTITY_CIRCLE)
      ++aNbCircles;
    else if ((*anEntIt)->type() != ENTITY_POINT)
      ++aNbOther;
  }

  if (aNbLines + aNbCircles + aNbOther == 2) {
    if (aNbOther > 0)
      myType = CONSTRAINT_PERPENDICULAR_CURVES;
  }
  else {
    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
    return;
  }

  FeaturePtr aFeature1, aFeature2;
  getFeatures(myBaseConstraint, aFeature1, aFeature2);

  // check number of coincident points
  std::set<AttributePtr> aCoincidentPoints = coincidentBoundaryPoints(aFeature1, aFeature2);
  // Try to find non-boundary points coincident with both features.
  // It is necesasry to create perpendicularity with ellipse
  if (aCoincidentPoints.empty() && aNbOther > 0)
    aCoincidentPoints = coincidentPoints(aFeature1, aFeature2);

  EntityWrapperPtr aSharedPointEntity;
  std::list<GCSConstraintPtr> anAuxConstraints;
  if (!aCoincidentPoints.empty()) {
    mySharedPoint = *aCoincidentPoints.begin();
    aSharedPointEntity = myStorage->entity(mySharedPoint);
  }
  else if (aNbOther > 0) {
    // create auxiliary point
    GCSPointPtr aPoint(new GCS::Point);
    aPoint->x = aStorage->createParameter();
    aPoint->y = aStorage->createParameter();
    calculateIntersectionPoint(myAttributes.front(), myAttributes.back(), aPoint);

    myAuxPoint.reset(new PlaneGCSSolver_PointWrapper(aPoint));
    aSharedPointEntity = myAuxPoint;

    // create auxiliary coincident constraints for tangency with ellipse
    EntityWrapperPtr aDummy;
    ConstraintWrapperPtr aCoincidence = PlaneGCSSolver_Tools::createConstraint(ConstraintPtr(),
        CONSTRAINT_PT_ON_CURVE, aDummy, aSharedPointEntity, aDummy, myAttributes.front(), aDummy);
    anAuxConstraints = aCoincidence->constraints();
    aCoincidence = PlaneGCSSolver_Tools::createConstraint(ConstraintPtr(),
        CONSTRAINT_PT_ON_CURVE, aDummy, aSharedPointEntity, aDummy, myAttributes.back(), aDummy);
    anAuxConstraints.insert(anAuxConstraints.end(),
        aCoincidence->constraints().begin(), aCoincidence->constraints().end());
  }

  ScalarWrapperPtr anAngleWrapper;
  if (aSharedPointEntity) {
    adjustAngleBetweenCurves(GCS_EDGE_WRAPPER(myAttributes.front())->entity(),
                             GCS_EDGE_WRAPPER(myAttributes.back())->entity(),
                             GCS_POINT_WRAPPER(aSharedPointEntity)->point(), &myCurveCurveAngle);
    anAngleWrapper.reset(new PlaneGCSSolver_ScalarWrapper(&myCurveCurveAngle));
  }

  mySolverConstraint = PlaneGCSSolver_Tools::createConstraint(myBaseConstraint, myType,
      anAngleWrapper, aSharedPointEntity, EntityWrapperPtr(),
      myAttributes.front(), myAttributes.back());

  if (!anAuxConstraints.empty()) {
    anAuxConstraints.insert(anAuxConstraints.end(), mySolverConstraint->constraints().begin(),
        mySolverConstraint->constraints().end());
    mySolverConstraint->setConstraints(anAuxConstraints);
  }

  myStorage->addConstraint(myBaseConstraint, mySolverConstraint);
}

void SketchSolver_ConstraintPerpendicular::notify(const FeaturePtr&      theFeature,
                                                  PlaneGCSSolver_Update* theUpdater)
{
  if (theFeature->getKind() != SketchPlugin_ConstraintCoincidence::ID())
    return;

  // base constraint may become invalid (for example, during undo)
  if (!myBaseConstraint->data() || !myBaseConstraint->data()->isValid())
    return;

  FeaturePtr aTgFeat1, aTgFeat2;
  getFeatures(myBaseConstraint, aTgFeat1, aTgFeat2);

  bool isRebuild = false;
  if (theFeature->data() && theFeature->data()->isValid()) {
    // the constraint has been created
    if (!mySharedPoint) {
      // features has no shared point, check whether coincidence constraint binds these features)
      int aNbCoincidentFeatures = 0;
      for (int i = 0; i < CONSTRAINT_ATTR_SIZE; ++i) {
        AttributeRefAttrPtr aRefAttr = theFeature->refattr(SketchPlugin_Constraint::ATTRIBUTE(i));
        if (!aRefAttr)
          continue;

        ObjectPtr anObj;
        if (aRefAttr->isObject())
          anObj = aRefAttr->object();
        else
          anObj = aRefAttr->attr()->owner();

        FeaturePtr aFeature = ModelAPI_Feature::feature(anObj);
        if (aFeature == aTgFeat1 || aFeature == aTgFeat2)
          ++aNbCoincidentFeatures;
      }

      if (aNbCoincidentFeatures == 2)
        isRebuild = true;
    }
  }

  if (!isRebuild) {
    if (mySharedPoint) {
      // The features are tangent in the shared point, but the coincidence has been removed/updated.
      // Check if the coincidence is the same.
      std::set<AttributePtr> aCoincidentPoints = coincidentBoundaryPoints(aTgFeat1, aTgFeat2);
      isRebuild = true;
      std::set<AttributePtr>::iterator anIt = aCoincidentPoints.begin();
      for (; anIt != aCoincidentPoints.end() && isRebuild; ++anIt)
        if (*anIt == mySharedPoint)
          isRebuild = false; // the coincidence is still exists => nothing to change
    }
    else {
      // check both features have a coincident point
      std::set<AttributePtr> aCoincidentPoints = coincidentPoints(aTgFeat1, aTgFeat2);
      isRebuild = (bool)(myAuxPoint.get()) == (!aCoincidentPoints.empty());
    }
  }

  if (isRebuild)
    rebuild();
}




// ==================   Auxiliary functions   =================================
void getFeatures(const ConstraintPtr& theConstraint,
                 FeaturePtr&          theFeature1,
                 FeaturePtr&          theFeature2)
{
  AttributeRefAttrPtr aRefAttr = theConstraint->refattr(SketchPlugin_Constraint::ENTITY_A());
  theFeature1 = ModelAPI_Feature::feature(aRefAttr->object());
  aRefAttr = theConstraint->refattr(SketchPlugin_Constraint::ENTITY_B());
  theFeature2 = ModelAPI_Feature::feature(aRefAttr->object());
}

static std::set<FeaturePtr> collectCoincidences(FeaturePtr theFeature1, FeaturePtr theFeature2)
{
  const std::set<AttributePtr>& aRefs1 = theFeature1->data()->refsToMe();
  const std::set<AttributePtr>& aRefs2 = theFeature2->data()->refsToMe();

  std::set<FeaturePtr> aCoincidences;
  std::set<AttributePtr>::const_iterator anIt;

  // collect coincidences referred to the first feature
  for (anIt = aRefs1.begin(); anIt != aRefs1.end(); ++anIt) {
    FeaturePtr aRef = ModelAPI_Feature::feature((*anIt)->owner());
    if (aRef && aRef->getKind() == SketchPlugin_ConstraintCoincidence::ID())
      aCoincidences.insert(aRef);
  }

  // leave only coincidences referred to the second feature
  std::set<FeaturePtr> aCoincidencesBetweenFeatures;
  for (anIt = aRefs2.begin(); anIt != aRefs2.end(); ++anIt) {
    FeaturePtr aRef = ModelAPI_Feature::feature((*anIt)->owner());
    if (aCoincidences.find(aRef) != aCoincidences.end())
      aCoincidencesBetweenFeatures.insert(aRef);
  }

  return aCoincidencesBetweenFeatures;
}

std::set<AttributePtr> coincidentBoundaryPoints(FeaturePtr theFeature1, FeaturePtr theFeature2)
{
  std::set<FeaturePtr> aCoincidences = collectCoincidences(theFeature1, theFeature2);
  // collect points only
  std::set<AttributePtr> aCoincidentPoints;
  std::set<FeaturePtr>::const_iterator aCIt = aCoincidences.begin();
  for (; aCIt != aCoincidences.end(); ++ aCIt) {
    AttributeRefAttrPtr aRefAttrA = (*aCIt)->refattr(SketchPlugin_Constraint::ENTITY_A());
    AttributeRefAttrPtr aRefAttrB = (*aCIt)->refattr(SketchPlugin_Constraint::ENTITY_B());
    if (!aRefAttrA || aRefAttrA->isObject() ||
        !aRefAttrB || aRefAttrB->isObject())
      continue;

    AttributePtr anAttrA = aRefAttrA->attr();
    AttributePtr anAttrB = aRefAttrB->attr();
    if (anAttrA->id() != SketchPlugin_Arc::CENTER_ID() &&
        anAttrA->id() != SketchPlugin_Circle::CENTER_ID() &&
        anAttrB->id() != SketchPlugin_Arc::CENTER_ID() &&
        anAttrB->id() != SketchPlugin_Circle::CENTER_ID()) {
      aCoincidentPoints.insert(anAttrA);
      aCoincidentPoints.insert(anAttrB);
    }
  }
  return aCoincidentPoints;
}

static std::set<AttributePtr> refsToFeatureAndResults(FeaturePtr theFeature)
{
  std::set<AttributePtr> aRefs = theFeature->data()->refsToMe();
  const std::list<ResultPtr>& aResults = theFeature->results();
  for (std::list<ResultPtr>::const_iterator anIt = aResults.begin();
      anIt != aResults.end(); ++anIt) {
    const std::set<AttributePtr>& aResRefs = (*anIt)->data()->refsToMe();
    aRefs.insert(aResRefs.begin(), aResRefs.end());
  }
  return aRefs;
}

// collect all points coincident with the feature
static std::set<AttributePtr> pointsOnFeature(FeaturePtr theFeature)
{
  std::set<AttributePtr> aPoints;

  std::set<AttributePtr> aRefs = refsToFeatureAndResults(theFeature);
  for (std::set<AttributePtr>::const_iterator anIt = aRefs.begin(); anIt != aRefs.end(); ++anIt) {
    FeaturePtr aRef = ModelAPI_Feature::feature((*anIt)->owner());
    if (aRef && (aRef->getKind() == SketchPlugin_ConstraintCoincidence::ID() ||
                 aRef->getKind() == SketchPlugin_ConstraintMiddle::ID())) {
      for (int i = 0; i < CONSTRAINT_ATTR_SIZE; ++i) {
        AttributeRefAttrPtr aRefAttr = aRef->refattr(SketchPlugin_Constraint::ATTRIBUTE(i));
        if (aRefAttr) {
          AttributePtr anAttr = aRefAttr->attr();
          if (anAttr && anAttr->id() != SketchPlugin_Arc::CENTER_ID() &&
                        anAttr->id() != SketchPlugin_Circle::CENTER_ID())
            aPoints.insert(anAttr);
        }
      }
    }
  }
  return aPoints;
}

std::set<AttributePtr> coincidentPoints(FeaturePtr theFeature1, FeaturePtr theFeature2)
{
  std::set<AttributePtr> aPointsOnF1 = pointsOnFeature(theFeature1);
  std::set<AttributePtr> aPointsOnF2 = pointsOnFeature(theFeature2);

  std::set<AttributePtr> aCommonPoints;
  for (std::set<AttributePtr>::iterator anIt = aPointsOnF1.begin();
       anIt != aPointsOnF1.end(); ++anIt)
    if (aPointsOnF2.find(*anIt) != aPointsOnF2.end())
      aCommonPoints.insert(*anIt);
  return aCommonPoints;
}

void adjustAngleBetweenCurves(const GCSCurvePtr& theCurve1,
                              const GCSCurvePtr& theCurve2,
                              const GCSPointPtr& thePoint,
                              double*            theAngle)
{
  double anAngle = GCS::System().calculateAngleViaPoint(*theCurve1, *theCurve2, *thePoint);
  // bring angle to [-pi..pi]
  if (anAngle >  PI) anAngle -= 2.0 * PI;
  if (anAngle < -PI) anAngle += 2.0 * PI;
  // set angle value according to the current angle between curves
  if (anAngle >= 0)
    *theAngle = PI / 2.;
  else
    *theAngle = -PI / 2.;
}

void calculateIntersectionPoint(EntityWrapperPtr theCurve1, EntityWrapperPtr theCurve2,
                                GCSPointPtr& theIntersectionPoint)
{
  std::shared_ptr<GeomAPI_Ellipse2d> anEllipse = PlaneGCSSolver_Tools::ellipse(theCurve1);
  EntityWrapperPtr aCurve2 = theCurve2;
  if (!anEllipse) {
    // try converting to ellipse the second curve
    anEllipse = PlaneGCSSolver_Tools::ellipse(theCurve2);
    if (!anEllipse)
      return; // no one curve is ellipse
    aCurve2 = theCurve1;
  }

  GeomPnt2dPtr aP1, aP2;
  if (aCurve2->type() == ENTITY_LINE) {
    std::shared_ptr<GeomAPI_Lin2d> aLine = PlaneGCSSolver_Tools::line(aCurve2);
    anEllipse->distance(aLine, aP1, aP2);
  }
  else if (aCurve2->type() == ENTITY_ARC || aCurve2->type() == ENTITY_CIRCLE) {
    std::shared_ptr<GeomAPI_Circ2d> aCircle = PlaneGCSSolver_Tools::circle(aCurve2);
    anEllipse->distance(aCircle, aP1, aP2);
  }
  else if (aCurve2->type() == ENTITY_ELLIPSE || aCurve2->type() == ENTITY_ELLIPTIC_ARC) {
    std::shared_ptr<GeomAPI_Ellipse2d> anEl2 = PlaneGCSSolver_Tools::ellipse(aCurve2);
    anEllipse->distance(anEl2, aP1, aP2);
  }

  if (aP1 && aP2) {
    *theIntersectionPoint->x = 0.5 * (aP1->x() + aP2->x());
    *theIntersectionPoint->y = 0.5 * (aP1->y() + aP2->y());
  }
}
