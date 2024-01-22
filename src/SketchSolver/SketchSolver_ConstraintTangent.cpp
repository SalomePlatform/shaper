// Copyright (C) 2014-2024  CEA, EDF
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

#include <SketchSolver_ConstraintTangent.h>
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

#include <GeomDataAPI_Point2DArray.h>

#include <ModelAPI_AttributeInteger.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_BSpline.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintCoincidenceInternal.h>
#include <SketchPlugin_ConstraintMiddle.h>

#include <cmath>

#define GCS_EDGE_WRAPPER(x) std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(x)

/// \brief Obtain tangent features from the constraint
static void getTangentFeatures(const ConstraintPtr& theConstraint,
                               FeaturePtr& theFeature1,
                               FeaturePtr& theFeature2);

/// \brief Obtain all coincident constraints between features
static std::set<FeaturePtr> collectCoincidences(FeaturePtr theFeature1, FeaturePtr theFeature2);

/// \brief Check whether the entities has only one shared point or less.
///        Return list of coincident points.
static std::set<AttributePtr> coincidentBoundaryPoints(FeaturePtr theFeature1,
                                                       FeaturePtr theFeature2);

/// \brief Collect points coincident with each of two features
static std::set<AttributePtr> coincidentPoints(FeaturePtr theFeature1, FeaturePtr theFeature2);

/// \brief Check if two connected arcs have centers
///        in same direction relatively to connection point
static bool isArcArcTangencyInternal(EntityWrapperPtr theArc1,
                                     EntityWrapperPtr theArc2);

static ConstraintWrapperPtr
  createArcLineTangency(EntityWrapperPtr theEntity1,
                        EntityWrapperPtr theEntity2,
                        EntityWrapperPtr theShapedPoint = EntityWrapperPtr(),
                        double*          theAngle = 0);

static ConstraintWrapperPtr
  createCurveCurveTangency(EntityWrapperPtr theEntity1,
                           EntityWrapperPtr theEntity2,
                           bool             theInternalTangency,
                           EntityWrapperPtr theSharedPoint = EntityWrapperPtr(),
                           double*          theAngle = 0);

static void calculateTangencyPoint(EntityWrapperPtr theCurve1, EntityWrapperPtr theCurve2,
                                   GCSPointPtr& theTangencyPoint);


void SketchSolver_ConstraintTangent::process()
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

void SketchSolver_ConstraintTangent::rebuild()
{
  if (mySolverConstraint)
    myStorage->removeConstraint(myBaseConstraint);

  std::shared_ptr<PlaneGCSSolver_Storage> aStorage =
      std::dynamic_pointer_cast<PlaneGCSSolver_Storage>(myStorage);

  mySolverConstraint = ConstraintWrapperPtr();
  mySharedPoint = AttributePtr();
  if (myAuxPoint) {
    GCS::SET_pD aParams = PlaneGCSSolver_Tools::parameters(myAuxPoint);
    if (myAuxParameters[0])
      aParams.insert(myAuxParameters[0]->scalar());
    if (myAuxParameters[1])
      aParams.insert(myAuxParameters[1]->scalar());
    aStorage->removeParameters(aParams);
    myAuxPoint = EntityWrapperPtr();
    myAuxParameters[0] = myAuxParameters[1] = ScalarWrapperPtr();
  }

  // Check the quantity of entities of each type and their order (arcs first)
  int aNbLines = 0;
  int aNbCircles = 0;
  int aNbEllipses = 0;
  int aNbSplines = 0;
  std::list<EntityWrapperPtr>::iterator anEntIt = myAttributes.begin();
  for (; anEntIt != myAttributes.end(); ++anEntIt) {
    if (!(*anEntIt).get())
      continue;
    if ((*anEntIt)->type() == ENTITY_LINE)
      ++aNbLines;
    else if ((*anEntIt)->type() == ENTITY_ARC || (*anEntIt)->type() == ENTITY_CIRCLE)
      ++aNbCircles;
    else if ((*anEntIt)->type() == ENTITY_ELLIPSE || (*anEntIt)->type() == ENTITY_ELLIPTIC_ARC)
      ++aNbEllipses;
    else if ((*anEntIt)->type() == ENTITY_BSPLINE)
      ++aNbSplines;
  }

  if (aNbCircles + aNbEllipses + aNbSplines < 1) {
    myErrorMsg = SketchSolver_Error::INCORRECT_TANGENCY_ATTRIBUTE();
    return;
  }
  if (aNbLines == 1 && aNbCircles == 1) {
    myType = CONSTRAINT_TANGENT_CIRCLE_LINE;
  }
  else if (aNbLines + aNbCircles + aNbEllipses + aNbSplines == 2) {
    myType = CONSTRAINT_TANGENT_CURVE_CURVE;
    isArcArcInternal = isArcArcTangencyInternal(myAttributes.front(), myAttributes.back());
  }
  else {
    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
    return;
  }

  FeaturePtr aFeatures[2];
  getTangentFeatures(myBaseConstraint, aFeatures[0], aFeatures[1]);

  // check number of coincident points
  std::set<AttributePtr> aCoincidentPoints = coincidentBoundaryPoints(aFeatures[0], aFeatures[1]);
  if (myType == CONSTRAINT_TANGENT_CIRCLE_LINE && aCoincidentPoints.size() > 2) {
    myErrorMsg = SketchSolver_Error::TANGENCY_FAILED();
    return;
  }

  EntityWrapperPtr aTgEntities[2] = { myAttributes.front(), myAttributes.back() };

  if (aCoincidentPoints.empty()) {
    // Try to find non-boundary points coincident with both features.
    // It is necesasry to create tangency with ellipse.
    if (aNbEllipses > 0)
      aCoincidentPoints = coincidentPoints(aFeatures[0], aFeatures[1]);
  }
  else if (aNbSplines > 0) {
    // General approach applying tangency to B-spline leads to hang-up in PlaneGCS.
    // So, the tangency will be applied for the construction segment instead of B-spline curve.
    for (int i = 0; i < 2; ++i) {
      if (aTgEntities[i]->type() == ENTITY_BSPLINE) {
        EdgeWrapperPtr anEdge =
            std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(aTgEntities[i]);
        std::shared_ptr<GCS::BSpline> aBSpline =
            std::dynamic_pointer_cast<GCS::BSpline>(anEdge->entity());

        // which boundary is coincident?
        GCS::Point aPoint1, aPoint2;
        for (std::set<AttributePtr>::iterator aPIt = aCoincidentPoints.begin();
             aPIt != aCoincidentPoints.end(); ++aPIt) {
          if ((*aPIt)->owner() == aFeatures[i]) {
            if ((*aPIt)->id() == SketchPlugin_BSpline::START_ID()) {
              aPoint1 = aBSpline->poles[0];
              aPoint2 = aBSpline->poles[1];
            }
            else if ((*aPIt)->id() == SketchPlugin_BSpline::END_ID()) {
              aPoint1 = aBSpline->poles[aBSpline->poles.size() - 2];
              aPoint2 = aBSpline->poles[aBSpline->poles.size() - 1];
            }
            break;
          }
        }

        // substitute B-spline by its boundary segment
        std::shared_ptr<GCS::Line> aSegment(new GCS::Line);
        aSegment->p1 = aPoint1;
        aSegment->p2 = aPoint2;
        aTgEntities[i] = EdgeWrapperPtr(new PlaneGCSSolver_EdgeWrapper(aSegment));
        --aNbSplines;
        ++aNbLines;
      }
    }
  }

  EntityWrapperPtr aSharedPointEntity;
  std::list<GCSConstraintPtr> anAuxConstraints;
  if (!aCoincidentPoints.empty()) {
    mySharedPoint = *aCoincidentPoints.begin();
    aSharedPointEntity = myStorage->entity(mySharedPoint);
  }
  else if (aNbEllipses + aNbSplines > 0) {
    // create auxiliary point
    GCSPointPtr aPoint(new GCS::Point);
    aPoint->x = aStorage->createParameter();
    aPoint->y = aStorage->createParameter();
    calculateTangencyPoint(aTgEntities[0], aTgEntities[1], aPoint);

    myAuxPoint.reset(new PlaneGCSSolver_PointWrapper(aPoint));
    aSharedPointEntity = myAuxPoint;

    EntityWrapperPtr aDummy;
    for (int i = 0; i < 2; ++i) {
      // create auxiliary parameters for coincidence with B-spline
      if (aTgEntities[i]->type() == ENTITY_BSPLINE)
        myAuxParameters[i].reset(new PlaneGCSSolver_ScalarWrapper(aStorage->createParameter()));

      // create auxiliary coincident constraints for tangency with ellipse
      ConstraintWrapperPtr aCoincidence = PlaneGCSSolver_Tools::createConstraint(ConstraintPtr(),
          CONSTRAINT_PT_ON_CURVE, myAuxParameters[i],
          aSharedPointEntity, aDummy, aTgEntities[i], aDummy);
      anAuxConstraints.insert(anAuxConstraints.end(),
          aCoincidence->constraints().begin(), aCoincidence->constraints().end());
    }
  }

  if (myType == CONSTRAINT_TANGENT_CIRCLE_LINE) {
    mySolverConstraint = createArcLineTangency(aTgEntities[0], aTgEntities[1],
                                           aSharedPointEntity, &myCurveCurveAngle);
  } else {
    mySolverConstraint = createCurveCurveTangency(aTgEntities[0], aTgEntities[1],
                            isArcArcInternal, aSharedPointEntity, &myCurveCurveAngle);
  }

  if (!anAuxConstraints.empty()) {
    anAuxConstraints.insert(anAuxConstraints.end(), mySolverConstraint->constraints().begin(),
        mySolverConstraint->constraints().end());
    mySolverConstraint->setConstraints(anAuxConstraints);
  }

  myStorage->addConstraint(myBaseConstraint, mySolverConstraint);
}

void SketchSolver_ConstraintTangent::adjustConstraint()
{
  if (myType == CONSTRAINT_TANGENT_CURVE_CURVE) {
    EntityWrapperPtr anEntity1 =
        myStorage->entity(myBaseConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
    EntityWrapperPtr anEntity2 =
        myStorage->entity(myBaseConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));

    if (isArcArcInternal != isArcArcTangencyInternal(anEntity1, anEntity2))
      rebuild();
  }
}

void SketchSolver_ConstraintTangent::notify(const FeaturePtr&      theFeature,
                                            PlaneGCSSolver_Update* theUpdater)
{
  if (theFeature->getKind() != SketchPlugin_ConstraintCoincidence::ID())
    return;

  // base constraint may become invalid (for example, during undo)
  if (!myBaseConstraint->data() || !myBaseConstraint->data()->isValid())
    return;

  FeaturePtr aTgFeat1, aTgFeat2;
  getTangentFeatures(myBaseConstraint, aTgFeat1, aTgFeat2);

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

bool SketchSolver_ConstraintTangent::remove()
{
  if (myAuxPoint) {
    std::shared_ptr<PlaneGCSSolver_Storage> aStorage =
        std::dynamic_pointer_cast<PlaneGCSSolver_Storage>(myStorage);

    GCS::SET_pD aParams = PlaneGCSSolver_Tools::parameters(myAuxPoint);
    if (myAuxParameters[0])
      aParams.insert(myAuxParameters[0]->scalar());
    if (myAuxParameters[1])
      aParams.insert(myAuxParameters[1]->scalar());
    aStorage->removeParameters(aParams);
    myAuxPoint = EntityWrapperPtr();
    myAuxParameters[0] = myAuxParameters[1] = ScalarWrapperPtr();
  }
  return SketchSolver_Constraint::remove();
}




// ==================   Auxiliary functions   =================================
void getTangentFeatures(const ConstraintPtr& theConstraint,
                              FeaturePtr&    theFeature1,
                              FeaturePtr&    theFeature2)
{
  AttributeRefAttrPtr aRefAttr = theConstraint->refattr(SketchPlugin_Constraint::ENTITY_A());
  theFeature1 = ModelAPI_Feature::feature(aRefAttr->object());
  aRefAttr = theConstraint->refattr(SketchPlugin_Constraint::ENTITY_B());
  theFeature2 = ModelAPI_Feature::feature(aRefAttr->object());
}

std::set<FeaturePtr> collectCoincidences(FeaturePtr theFeature1, FeaturePtr theFeature2)
{
  const std::set<AttributePtr>& aRefs1 = theFeature1->data()->refsToMe();
  const std::set<AttributePtr>& aRefs2 = theFeature2->data()->refsToMe();

  std::set<FeaturePtr> aCoincidences;
  std::map<AttributePtr, FeaturePtr> aCoincidentPoints;
  std::set<AttributePtr>::const_iterator anIt;

  // collect coincidences referred to the first feature
  for (anIt = aRefs1.begin(); anIt != aRefs1.end(); ++anIt) {
    FeaturePtr aRef = ModelAPI_Feature::feature((*anIt)->owner());
    if (aRef && (aRef->getKind() == SketchPlugin_ConstraintCoincidence::ID() ||
                 aRef->getKind() == SketchPlugin_ConstraintCoincidenceInternal::ID())) {
      aCoincidences.insert(aRef);
      AttributeRefAttrPtr aRefAttrA = aRef->refattr(SketchPlugin_Constraint::ENTITY_A());
      if (!aRefAttrA->isObject())
        aCoincidentPoints[aRefAttrA->attr()] = aRef;
      AttributeRefAttrPtr aRefAttrB = aRef->refattr(SketchPlugin_Constraint::ENTITY_B());
      if (!aRefAttrB->isObject())
        aCoincidentPoints[aRefAttrB->attr()] = aRef;
    }
  }

  // leave only coincidences referred to the second feature
  std::set<FeaturePtr> aCoincidencesBetweenFeatures;
  for (anIt = aRefs2.begin(); anIt != aRefs2.end(); ++anIt) {
    FeaturePtr aRef = ModelAPI_Feature::feature((*anIt)->owner());
    if (aCoincidences.find(aRef) != aCoincidences.end())
      aCoincidencesBetweenFeatures.insert(aRef);
    else if (aRef && (aRef->getKind() == SketchPlugin_ConstraintCoincidence::ID() ||
                      aRef->getKind() == SketchPlugin_ConstraintCoincidenceInternal::ID())) {
      for (int i = 0; i < CONSTRAINT_ATTR_SIZE; ++i) {
        AttributeRefAttrPtr aRefAttr = aRef->refattr(SketchPlugin_Constraint::ATTRIBUTE(i));
        if (aRefAttr && !aRefAttr->isObject()) {
          std::map<AttributePtr, FeaturePtr>::iterator aFound =
              aCoincidentPoints.find(aRefAttr->attr());
          if (aFound != aCoincidentPoints.end()) {
            aCoincidencesBetweenFeatures.insert(aRef);
            aCoincidencesBetweenFeatures.insert(aFound->second);
          }
        }
      }
    }
  }

  return aCoincidencesBetweenFeatures;
}

std::set<AttributePtr> coincidentBoundaryPoints(FeaturePtr theFeature1, FeaturePtr theFeature2)
{
  std::set<FeaturePtr> aCoincidences = collectCoincidences(theFeature1, theFeature2);
  // collect points only
  std::map<FeaturePtr, std::set<AttributePtr> > aCoincidentPoints;
  std::set<FeaturePtr>::const_iterator aCIt = aCoincidences.begin();
  for (; aCIt != aCoincidences.end(); ++ aCIt) {
    for (int i = 0; i < CONSTRAINT_ATTR_SIZE; ++i) {
      AttributeRefAttrPtr aRefAttr = (*aCIt)->refattr(SketchPlugin_Constraint::ATTRIBUTE(i));
      if (!aRefAttr || aRefAttr->isObject())
        continue;

      AttributePtr anAttr = aRefAttr->attr();
      FeaturePtr anOwner = ModelAPI_Feature::feature(anAttr->owner());
      if (anOwner == theFeature1 || anOwner == theFeature2) {
        if (anAttr->id() == SketchPlugin_BSplineBase::POLES_ID()) {
          AttributePoint2DArrayPtr aPoles =
              std::dynamic_pointer_cast<GeomDataAPI_Point2DArray>(anAttr);

          AttributeIntegerPtr anIndex;
          if (anOwner->getKind() == SketchPlugin_BSpline::ID()) {
            anIndex = (*aCIt)->integer(i == 0 ?
                SketchPlugin_ConstraintCoincidenceInternal::INDEX_ENTITY_A() :
                SketchPlugin_ConstraintCoincidenceInternal::INDEX_ENTITY_B());
          }
          if (anIndex) {
            if (anIndex->value() == 0)
              anAttr = anOwner->attribute(SketchPlugin_BSpline::START_ID());
            else if (anIndex->value() + 1 == aPoles->size())
              anAttr = anOwner->attribute(SketchPlugin_BSpline::END_ID());
            if (anAttr)
              aCoincidentPoints[anOwner].insert(anAttr);
          }
        }
        else if (anAttr->id() != SketchPlugin_Arc::CENTER_ID() &&
                 anAttr->id() != SketchPlugin_Circle::CENTER_ID())
          aCoincidentPoints[anOwner].insert(anAttr);
      }
    }
  }

  std::set<AttributePtr> aBoundaryPoints;
  if (aCoincidentPoints.size() == 2) {
    for (std::map<FeaturePtr, std::set<AttributePtr> >::iterator anIt = aCoincidentPoints.begin();
         anIt != aCoincidentPoints.end(); ++anIt)
      aBoundaryPoints.insert(anIt->second.begin(), anIt->second.end());
  }
  return aBoundaryPoints;
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
                 aRef->getKind() == SketchPlugin_ConstraintCoincidenceInternal::ID() ||
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

bool isArcArcTangencyInternal(EntityWrapperPtr theArc1, EntityWrapperPtr theArc2)
{
  std::shared_ptr<GCS::Circle> aCirc1 = std::dynamic_pointer_cast<GCS::Circle>(
      GCS_EDGE_WRAPPER(theArc1)->entity());
  std::shared_ptr<GCS::Circle> aCirc2 = std::dynamic_pointer_cast<GCS::Circle>(
      GCS_EDGE_WRAPPER(theArc2)->entity());

  if (!aCirc1 || !aCirc2)
    return false;

  double aDX = *(aCirc1->center.x) - *(aCirc2->center.x);
  double aDY = *(aCirc1->center.y) - *(aCirc2->center.y);
  double aDist = sqrt(aDX * aDX + aDY * aDY);

  return (aDist < *(aCirc1->rad) || aDist < *(aCirc2->rad));
}

// sets angle to 0 or 180 degrees
static void adjustAngleBetweenCurves(const GCSCurvePtr& theCurve1,
                                     const GCSCurvePtr& theCurve2,
                                     const GCSPointPtr& thePoint,
                                     double*            theAngle)
{
  double anAngle = GCS::System().calculateAngleViaPoint(*theCurve1, *theCurve2, *thePoint);
  // bring angle to [-pi..pi]
  if (anAngle >  PI) anAngle -= 2.0 * PI;
  if (anAngle < -PI) anAngle += 2.0 * PI;
  // set angle value according to the current angle between curves
  if (fabs(anAngle) <= PI / 2.)
    *theAngle = 0.0;
  else
    *theAngle = PI;
}


ConstraintWrapperPtr createArcLineTangency(EntityWrapperPtr theEntity1,
                                           EntityWrapperPtr theEntity2,
                                           EntityWrapperPtr theSharedPoint,
                                           double*          theAngle)
{
  EdgeWrapperPtr anEntLine, anEntCirc;
  if (theEntity1->type() == ENTITY_LINE) {
    anEntLine = GCS_EDGE_WRAPPER(theEntity1);
    anEntCirc = GCS_EDGE_WRAPPER(theEntity2);
  } else {
    anEntLine = GCS_EDGE_WRAPPER(theEntity2);
    anEntCirc = GCS_EDGE_WRAPPER(theEntity1);
  }

  std::shared_ptr<GCS::Circle> aCirc =
      std::dynamic_pointer_cast<GCS::Circle>(anEntCirc->entity());
  std::shared_ptr<GCS::Arc> anArc = std::dynamic_pointer_cast<GCS::Arc>(aCirc);

  std::shared_ptr<GCS::Line> aLine =
      std::dynamic_pointer_cast<GCS::Line>(anEntLine->entity());

  GCSConstraintPtr aNewConstr;
  if (theSharedPoint && anArc) { // do not process shared point between circle and line
    GCSPointPtr aPoint =
        std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(theSharedPoint)->point();

    adjustAngleBetweenCurves(anArc, aLine, aPoint, theAngle);
    aNewConstr =
        GCSConstraintPtr(new GCS::ConstraintAngleViaPoint(*anArc, *aLine, *aPoint, theAngle));
  } else {
    aNewConstr =
      GCSConstraintPtr(new GCS::ConstraintP2LDistance(aCirc->center, *aLine, aCirc->rad));
  }

  return  ConstraintWrapperPtr(
      new PlaneGCSSolver_ConstraintWrapper(aNewConstr, CONSTRAINT_TANGENT_CIRCLE_LINE));
}

ConstraintWrapperPtr createCurveCurveTangency(EntityWrapperPtr theEntity1,
                                              EntityWrapperPtr theEntity2,
                                              bool             theInternalTangency,
                                              EntityWrapperPtr theSharedPoint,
                                              double*          theAngle)
{
  GCSCurvePtr aCurve1 =
    std::dynamic_pointer_cast<GCS::Curve>(GCS_EDGE_WRAPPER(theEntity1)->entity());
  GCSCurvePtr aCurve2 =
    std::dynamic_pointer_cast<GCS::Curve>(GCS_EDGE_WRAPPER(theEntity2)->entity());

  GCSConstraintPtr aNewConstr;
  if (theSharedPoint) {
    GCSPointPtr aPoint =
        std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(theSharedPoint)->point();

    adjustAngleBetweenCurves(aCurve1, aCurve2, aPoint, theAngle);
    aNewConstr =
        GCSConstraintPtr(new GCS::ConstraintAngleViaPoint(*aCurve1, *aCurve2, *aPoint, theAngle));
  } else {
    std::shared_ptr<GCS::Circle> aCirc1 = std::dynamic_pointer_cast<GCS::Circle>(aCurve1);
    std::shared_ptr<GCS::Circle> aCirc2 = std::dynamic_pointer_cast<GCS::Circle>(aCurve2);
    aNewConstr = GCSConstraintPtr(new GCS::ConstraintTangentCircumf(aCirc1->center, aCirc2->center,
                                  aCirc1->rad, aCirc2->rad, theInternalTangency));
  }

  return ConstraintWrapperPtr(
      new PlaneGCSSolver_ConstraintWrapper(aNewConstr, CONSTRAINT_TANGENT_CURVE_CURVE));
}

void calculateTangencyPoint(EntityWrapperPtr theCurve1, EntityWrapperPtr theCurve2,
                            GCSPointPtr& theTangencyPoint)
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
    *theTangencyPoint->x = 0.5 * (aP1->x() + aP2->x());
    *theTangencyPoint->y = 0.5 * (aP1->y() + aP2->y());
  }
}
