#include <SketchSolver_ConstraintMultiRotation.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <SketchPlugin_MultiRotation.h>

#include <math.h>

void SketchSolver_ConstraintMultiRotation::getAttributes(
    EntityWrapperPtr& theCenter, double& theAngle,
    std::list< std::list<EntityWrapperPtr> >& theEntities)
{
  DataPtr aData = myBaseConstraint->data();
  theAngle = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
      aData->attribute(SketchPlugin_MultiRotation::ANGLE_ID()))->value();

  AttributePtr aCenterAttr = aData->attribute(SketchPlugin_MultiRotation::CENTER_ID());
  if (!aCenterAttr || !aCenterAttr->isInitialized()) {
    myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
    return;
  }

  myType = CONSTRAINT_MULTI_ROTATION;

  myStorage->update(aCenterAttr, GID_OUTOFGROUP);
  theCenter = myStorage->entity(aCenterAttr);

  getEntitiesAndCopies(theEntities);
}

void SketchSolver_ConstraintMultiRotation::process()
{
  cleanErrorMsg();
  if (!myBaseConstraint || !myStorage || myGroupID == GID_UNKNOWN) {
    /// TODO: Put error message here
    return;
  }

  EntityWrapperPtr aRotationCenter;
  std::list<std::list<EntityWrapperPtr> > anEntitiesAndCopies;
  getAttributes(aRotationCenter, myAngle, anEntitiesAndCopies);
  if (!myErrorMsg.empty())
    return;

  BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();
  std::list<ConstraintWrapperPtr> aRotConstraints;

  std::list< std::list<EntityWrapperPtr> >::iterator anEntIt = anEntitiesAndCopies.begin();
  for (; anEntIt != anEntitiesAndCopies.end(); ++anEntIt) {
    std::list<ConstraintWrapperPtr> aNewConstraints =
        aBuilder->createConstraint(myBaseConstraint, myGroupID, mySketchID, myType,
        myAngle, aRotationCenter, EntityWrapperPtr(), *anEntIt);
    aRotConstraints.insert(aRotConstraints.end(), aNewConstraints.begin(), aNewConstraints.end());
  }
  myStorage->addConstraint(myBaseConstraint, aRotConstraints);

  myAdjusted = false;
  adjustConstraint();
}

void SketchSolver_ConstraintMultiRotation::updateLocal()
{
  double aValue = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
      myBaseConstraint->attribute(SketchPlugin_MultiRotation::ANGLE_ID()))->value();
  if (fabs(myAngle - aValue) > tolerance)
    myAdjusted = false;
  // update angle value
  myAngle = aValue;

  // update center
  AttributePtr aCenterAttr = myBaseConstraint->attribute(SketchPlugin_MultiRotation::CENTER_ID());
  if (myStorage->update(aCenterAttr, myGroupID)) {
    myStorage->update(aCenterAttr, GID_UNKNOWN);
    myAdjusted = false;
  }
}

void SketchSolver_ConstraintMultiRotation::adjustConstraint()
{
  if (fabs(myAngle) < tolerance) {
    myStorage->setNeedToResolve(false);
    return;
  }

  const std::list<ConstraintWrapperPtr>& aConstraints = myStorage->constraint(myBaseConstraint);
  std::list<ConstraintWrapperPtr>::const_iterator aCIt = aConstraints.begin();
  for (; aCIt != aConstraints.end(); ++aCIt)
    (*aCIt)->setValue(myAngle);

  SketchSolver_ConstraintMulti::adjustConstraint();
}

const std::string& SketchSolver_ConstraintMultiRotation::nameNbObjects()
{
  return SketchPlugin_MultiRotation::NUMBER_OF_OBJECTS_ID();
}
