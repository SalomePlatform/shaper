// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <SketchSolver_ConstraintCollinear.h>
#include <SketchSolver_Error.h>

#include <PlaneGCSSolver_ConstraintWrapper.h>
#include <PlaneGCSSolver_EdgeWrapper.h>
#include <PlaneGCSSolver_PointWrapper.h>
#include <PlaneGCSSolver_UpdateCoincidence.h>

#include <SketchPlugin_Line.h>

static ConstraintWrapperPtr createPointsOnLine(
    std::shared_ptr<PlaneGCSSolver_PointWrapper>  thePoint1,
    std::shared_ptr<PlaneGCSSolver_PointWrapper>  thePoint2,
    std::shared_ptr<PlaneGCSSolver_EdgeWrapper> theLine)
{
  std::shared_ptr<GCS::Line> aGCSLine = std::dynamic_pointer_cast<GCS::Line>(theLine->entity());

  std::list<GCSConstraintPtr> aConstrList;
  if (thePoint1)
    aConstrList.push_back( GCSConstraintPtr(
        new GCS::ConstraintPointOnLine(*thePoint1->point(), *aGCSLine)) );
  if (thePoint2)
    aConstrList.push_back( GCSConstraintPtr(
        new GCS::ConstraintPointOnLine(*thePoint2->point(), *aGCSLine)) );

  return aConstrList.empty() ?  ConstraintWrapperPtr() : ConstraintWrapperPtr(
      new PlaneGCSSolver_ConstraintWrapper(aConstrList, CONSTRAINT_COLLINEAR));
}


void SketchSolver_ConstraintCollinear::process()
{
  cleanErrorMsg();
  if (!myBaseConstraint || !myStorage) {
    // Not enough parameters are assigned
    return;
  }

  EntityWrapperPtr aValue;
  std::vector<EntityWrapperPtr> anAttributes;
  getAttributes(aValue, anAttributes);
  if (!myErrorMsg.empty())
    return;
  if (anAttributes.empty()) {
    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
    return;
  }

  for (int i = 0; i < 2; ++i) {
    AttributeRefAttrPtr aRefAttr = myBaseConstraint->refattr(SketchPlugin_Constraint::ATTRIBUTE(i));
    FeaturePtr aLine = ModelAPI_Feature::feature(aRefAttr->object());
    myPoints[2*i] = myStorage->entity(aLine->attribute(SketchPlugin_Line::START_ID()));
    myPoints[2*i + 1] = myStorage->entity(aLine->attribute(SketchPlugin_Line::END_ID()));
  }

  myStorage->subscribeUpdates(this, PlaneGCSSolver_UpdateCoincidence::GROUP());
  myStorage->notify(myBaseConstraint);
}

void SketchSolver_ConstraintCollinear::notify(const FeaturePtr&      theFeature,
                                              PlaneGCSSolver_Update* theUpdater)
{
  PlaneGCSSolver_UpdateCoincidence* anUpdater =
      static_cast<PlaneGCSSolver_UpdateCoincidence*>(theUpdater);

  bool isPointOnOppositeLine[4];
  std::list<EntityWrapperPtr>::reverse_iterator anIt = myAttributes.rbegin();
  for (int i = 0; i < 2; ++i, ++anIt) {
    isPointOnOppositeLine[2*i] = anUpdater->isPointOnEntity(myPoints[2*i], *anIt);
    isPointOnOppositeLine[2*i + 1] = anUpdater->isPointOnEntity(myPoints[2*i + 1], *anIt);
  }

  // both points of one line is on another line => lines are already collinear,
  // would like to show "conflicting constraints"
  if (isPointOnOppositeLine[0] && isPointOnOppositeLine[1])
    isPointOnOppositeLine[1] = false;
  if (isPointOnOppositeLine[2] && isPointOnOppositeLine[3])
    isPointOnOppositeLine[3] = false;

  bool aConstraintToApply[4] = {false, false, false, false};
  ConstraintWrapperPtr aNewConstraint;
  std::shared_ptr<PlaneGCSSolver_PointWrapper> aPoints[2];
  std::shared_ptr<PlaneGCSSolver_EdgeWrapper> aLine;

  if (isPointOnOppositeLine[0] || isPointOnOppositeLine[1]) {
    // one of points of first line is already on the second line,
    // make another point of first line to be coincident with second line
    for (int i = 0; i < 2; ++i) {
      if (isPointOnOppositeLine[i])
        continue;

      if (!myIsConstraintApplied[i])
        aPoints[i] = std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(myPoints[i]);
      aConstraintToApply[i] = true;
    }
    aLine = std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(myAttributes.back());
  } else {
    // verify second line and add necessary constraints
    for (int i = 0; i < 2; ++i) {
      if (isPointOnOppositeLine[i + 2])
        continue;

      if (!myIsConstraintApplied[i+2])
        aPoints[i] = std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(myPoints[i + 2]);
      aConstraintToApply[i+2] = true;
    }
    aLine = std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(myAttributes.front());
  }

  bool isNew = false;
  for (int i = 0; i < 4 && !isNew; ++i)
    if (aConstraintToApply[i] != myIsConstraintApplied[i])
      isNew = true;
  if (isNew) {
    mySolverConstraint = createPointsOnLine(aPoints[0], aPoints[1], aLine);
    if (myInSolver) {
      myStorage->removeConstraint(myBaseConstraint);
      myInSolver = false;
    }
    if (mySolverConstraint) {
      myStorage->addConstraint(myBaseConstraint, mySolverConstraint);
      myInSolver = true;
    }

    for (int i = 0; i < 4; ++i)
      myIsConstraintApplied[i] = aConstraintToApply[i];
  }
}
