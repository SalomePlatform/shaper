// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <SketchSolver_ConstraintMovement.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>

#include <GeomDataAPI_Point2D.h>

#include <GeomAPI_Pnt2d.h>


SketchSolver_ConstraintMovement::SketchSolver_ConstraintMovement(FeaturePtr theFeature)
  : SketchSolver_ConstraintFixed(ConstraintPtr()),
    myMovedFeature(theFeature)
{
}

SketchSolver_ConstraintMovement::SketchSolver_ConstraintMovement(AttributePtr thePoint)
  : SketchSolver_ConstraintFixed(ConstraintPtr()),
    myDraggedPoint(thePoint)
{
  myMovedFeature = ModelAPI_Feature::feature(thePoint->owner());
}

void SketchSolver_ConstraintMovement::blockEvents(bool isBlocked)
{
  if (myMovedFeature)
    myMovedFeature->data()->blockSendAttributeUpdated(isBlocked);
}

void SketchSolver_ConstraintMovement::process()
{
  cleanErrorMsg();
  if (!myMovedFeature || !myStorage) {
    // Not enough parameters are initialized
    return;
  }

  EntityWrapperPtr aMovedEntity = entityToFix();
  if (!myErrorMsg.empty() || !aMovedEntity) {
    // Nothing to move, clear the feature to avoid changing its group
    // after removing the Movement constraint.
    myMovedFeature = FeaturePtr();
    return;
  }

  mySolverConstraint = fixFeature(aMovedEntity);
  myStorage->addMovementConstraint(mySolverConstraint);
}


EntityWrapperPtr SketchSolver_ConstraintMovement::entityToFix()
{
  // if the feature is copy, do not move it
  std::shared_ptr<SketchPlugin_Feature> aSketchFeature =
      std::dynamic_pointer_cast<SketchPlugin_Feature>(myMovedFeature);
  if (!aSketchFeature || aSketchFeature->isCopy()) {
    myStorage->setNeedToResolve(true);
    return EntityWrapperPtr();
  }

  EntityWrapperPtr anEntity =
      myDraggedPoint ? myStorage->entity(myDraggedPoint) : myStorage->entity(myMovedFeature);
  if (!anEntity) {
    myStorage->update(myMovedFeature, true);
    anEntity = myStorage->entity(myMovedFeature);
  }
  return anEntity;
}

void SketchSolver_ConstraintMovement::moveTo(
    const std::shared_ptr<GeomAPI_Pnt2d>& theDestinationPoint)
{
  EntityWrapperPtr aMovedEntity =
      myDraggedPoint ? myStorage->entity(myDraggedPoint) : myStorage->entity(myMovedFeature);
  if (!aMovedEntity)
    return;

  double aDelta[2] = { theDestinationPoint->x() - myStartPoint->x(),
                       theDestinationPoint->y() - myStartPoint->y() };

  GCS::VEC_pD aFixedParams = toParameters(aMovedEntity);
  for (int i = 0; i < aFixedParams.size() && i < myFixedValues.size(); ++i)
    myFixedValues[i] = *(aFixedParams[i]) + aDelta[i % 2];

  // no persistent constraints in the storage, thus store values directly to the feature
  if (myStorage->isEmpty()) {
    for (int i = 0; i < aFixedParams.size() && i < myFixedValues.size(); ++i)
      *(aFixedParams[i]) = myFixedValues[i];
    myStorage->setNeedToResolve(true);
  }
}
