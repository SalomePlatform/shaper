// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <SketchSolver_ConstraintTangent.h>
#include <SketchSolver_Error.h>

#include <PlaneGCSSolver_EdgeWrapper.h>
#include <PlaneGCSSolver_PointWrapper.h>
#include <PlaneGCSSolver_Tools.h>
#include <PlaneGCSSolver_UpdateCoincidence.h>

#include <GeomAPI_Pnt2d.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_ConstraintCoincidence.h>

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
static std::list<AttributePtr> coincidentPoints(FeaturePtr theFeature1, FeaturePtr theFeature2);

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
  createArcArcTangency(EntityWrapperPtr theEntity1,
                       EntityWrapperPtr theEntity2,
                       bool             theInternalTangency,
                       EntityWrapperPtr theSharedPoint = EntityWrapperPtr(),
                       double*          theAngle = 0);


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

  mySolverConstraint = ConstraintWrapperPtr();
  mySharedPoint = AttributePtr();

  // Check the quantity of entities of each type and their order (arcs first)
  int aNbLines = 0;
  int aNbCircles = 0;
  std::list<EntityWrapperPtr>::iterator anEntIt = myAttributes.begin();
  for (; anEntIt != myAttributes.end(); ++anEntIt) {
    if (!(*anEntIt).get())
      continue;
    if ((*anEntIt)->type() == ENTITY_LINE)
      ++aNbLines;
    else if ((*anEntIt)->type() == ENTITY_ARC || (*anEntIt)->type() == ENTITY_CIRCLE)
      ++aNbCircles;
  }

  if (aNbCircles < 1) {
    myErrorMsg = SketchSolver_Error::INCORRECT_TANGENCY_ATTRIBUTE();
    return;
  }
  if (aNbLines == 1 && aNbCircles == 1) {
    myType = CONSTRAINT_TANGENT_CIRCLE_LINE;
  }
  else if (aNbCircles == 2) {
    myType = CONSTRAINT_TANGENT_CIRCLE_CIRCLE;
    isArcArcInternal = isArcArcTangencyInternal(myAttributes.front(), myAttributes.back());
  }
  else {
    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
    return;
  }

  FeaturePtr aFeature1, aFeature2;
  getTangentFeatures(myBaseConstraint, aFeature1, aFeature2);

  // check number of coincident points
  std::list<AttributePtr> aCoincidentPoints = coincidentPoints(aFeature1, aFeature2);
  if (myType == CONSTRAINT_TANGENT_CIRCLE_LINE && aCoincidentPoints.size() > 1) {
    myErrorMsg = SketchSolver_Error::TANGENCY_FAILED();
    return;
  }

  EntityWrapperPtr aSharedPointEntity;
  if (!aCoincidentPoints.empty()) {
    mySharedPoint = aCoincidentPoints.front();
    aSharedPointEntity = myStorage->entity(mySharedPoint);
  }

  if (myType == CONSTRAINT_TANGENT_CIRCLE_LINE) {
    mySolverConstraint = createArcLineTangency(myAttributes.front(), myAttributes.back(),
                                           aSharedPointEntity, &myCurveCurveAngle);
  } else {
    mySolverConstraint = createArcArcTangency(myAttributes.front(), myAttributes.back(),
                            isArcArcInternal, aSharedPointEntity, &myCurveCurveAngle);
  }

  myStorage->addConstraint(myBaseConstraint, mySolverConstraint);
}

void SketchSolver_ConstraintTangent::adjustConstraint()
{
  if (myType == CONSTRAINT_TANGENT_CIRCLE_CIRCLE) {
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
  } else if (mySharedPoint) {
    // The features are tangent in the shared point, but the coincidence has been removed.
    // Check if the coincidence is the same.
    std::list<AttributePtr> aCoincidentPoints = coincidentPoints(aTgFeat1, aTgFeat2);
    isRebuild = true;
    std::list<AttributePtr>::iterator anIt = aCoincidentPoints.begin();
    for (; anIt != aCoincidentPoints.end() && isRebuild; ++anIt)
      if (*anIt == mySharedPoint)
        isRebuild = false; // the coincidence is still exists => nothing to change
  }

  if (isRebuild)
    rebuild();
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

std::list<AttributePtr> coincidentPoints(FeaturePtr theFeature1, FeaturePtr theFeature2)
{
  std::set<FeaturePtr> aCoincidences = collectCoincidences(theFeature1, theFeature2);
  // collect points only
  std::list<AttributePtr> aCoincidentPoints;
  std::set<FeaturePtr>::const_iterator aCIt = aCoincidences.begin();
  for (; aCIt != aCoincidences.end(); ++ aCIt) {
    for (int i = 0; i < CONSTRAINT_ATTR_SIZE; ++i) {
      AttributeRefAttrPtr aRefAttr = (*aCIt)->refattr(SketchPlugin_Constraint::ENTITY_A());
      if (aRefAttr && !aRefAttr->isObject()) {
        aCoincidentPoints.push_back(aRefAttr->attr());
        break;
      }
    }
  }
  return aCoincidentPoints;
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
  double anAngle = GCS::System::calculateAngleViaPoint(*theCurve1, *theCurve2, *thePoint);
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
  std::shared_ptr<GCS::Line> aLine =
      std::dynamic_pointer_cast<GCS::Line>(anEntLine->entity());

  GCSConstraintPtr aNewConstr;
  if (theSharedPoint) {
    std::shared_ptr<GCS::Arc> anArc = std::dynamic_pointer_cast<GCS::Arc>(aCirc);
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

ConstraintWrapperPtr createArcArcTangency(EntityWrapperPtr theEntity1,
                                          EntityWrapperPtr theEntity2,
                                          bool             theInternalTangency,
                                          EntityWrapperPtr theSharedPoint,
                                          double*          theAngle)
{
  std::shared_ptr<GCS::Circle> aCirc1 =
    std::dynamic_pointer_cast<GCS::Circle>(GCS_EDGE_WRAPPER(theEntity1)->entity());
  std::shared_ptr<GCS::Circle> aCirc2 =
    std::dynamic_pointer_cast<GCS::Circle>(GCS_EDGE_WRAPPER(theEntity2)->entity());

  GCSConstraintPtr aNewConstr;
  if (theSharedPoint) {
    std::shared_ptr<GCS::Arc> anArc1 = std::dynamic_pointer_cast<GCS::Arc>(aCirc1);
    std::shared_ptr<GCS::Arc> anArc2 = std::dynamic_pointer_cast<GCS::Arc>(aCirc2);
    GCSPointPtr aPoint =
        std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(theSharedPoint)->point();

    adjustAngleBetweenCurves(anArc1, anArc2, aPoint, theAngle);
    aNewConstr =
        GCSConstraintPtr(new GCS::ConstraintAngleViaPoint(*anArc1, *anArc2, *aPoint, theAngle));
  } else {
    aNewConstr = GCSConstraintPtr(new GCS::ConstraintTangentCircumf(aCirc1->center, aCirc2->center,
                                  aCirc1->rad, aCirc2->rad, theInternalTangency));
  }

  return ConstraintWrapperPtr(
      new PlaneGCSSolver_ConstraintWrapper(aNewConstr, CONSTRAINT_TANGENT_CIRCLE_CIRCLE));
}
