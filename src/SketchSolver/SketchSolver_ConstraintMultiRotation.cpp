#include <SketchSolver_ConstraintMultiRotation.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <SketchPlugin_MultiRotation.h>

#include <ModelAPI_AttributeString.h>

#include <math.h>

void SketchSolver_ConstraintMultiRotation::getAttributes(
    EntityWrapperPtr& theCenter, double& theAngle,
    bool& theFullValue,
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

  myStorage->update(aCenterAttr);
  theCenter = myStorage->entity(aCenterAttr);

  AttributeStringPtr aMethodTypeAttr = aData->string(SketchPlugin_MultiRotation::ANGLE_TYPE());
  theFullValue = aMethodTypeAttr->value() != "SingleAngle";

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
  bool isFullValue;
  std::list<std::list<EntityWrapperPtr> > anEntitiesAndCopies;
  getAttributes(aRotationCenter, myAngle, isFullValue, anEntitiesAndCopies);
  if (!myErrorMsg.empty())
    return;

  BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();
  std::list<ConstraintWrapperPtr> aRotConstraints;

  std::list< std::list<EntityWrapperPtr> >::iterator anEntIt = anEntitiesAndCopies.begin();
  for (; anEntIt != anEntitiesAndCopies.end(); ++anEntIt) {
    std::list<ConstraintWrapperPtr> aNewConstraints =
        aBuilder->createConstraint(myBaseConstraint, myGroupID, mySketchID, myType,
        myAngle, isFullValue, aRotationCenter, EntityWrapperPtr(), *anEntIt);
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
  DataPtr aData = myBaseConstraint->data();
  AttributePoint2DPtr aCenterPointAttribute = GeomDataAPI_Point2D::getPoint2D(aData,
                                           SketchPlugin_MultiRotation::CENTER_ID());
  bool aCenterPointChanged = aCenterPointAttribute != myCenterPointAttribute;
  if (aCenterPointChanged)
    myCenterPointAttribute = aCenterPointAttribute;

  AttributeStringPtr aMethodTypeAttr = aData->string(SketchPlugin_MultiRotation::ANGLE_TYPE());
  bool aFullValue = aMethodTypeAttr->value() != "SingleAngle";
  bool isMethodChanged = aFullValue != myIsFullValue;
  if (isMethodChanged)
    myIsFullValue = aFullValue;

  if (aCenterPointChanged || isMethodChanged) {
    DataPtr aData = myBaseConstraint->data();
    std::list<ConstraintWrapperPtr> aConstraints = myStorage->constraint(myBaseConstraint);
    std::list<ConstraintWrapperPtr>::const_iterator anIt = aConstraints.begin(),
      aLast = aConstraints.end();
    std::list<EntityWrapperPtr> anEntities;
    for (; anIt != aLast; anIt++) {
      ConstraintWrapperPtr aConstraint = *anIt;
      aConstraint->setIsFullValue(myIsFullValue);
      if (aCenterPointChanged) {
        anEntities.clear();
        const std::list<EntityWrapperPtr>& aConstraintEntities = aConstraint->entities();
        std::list<EntityWrapperPtr>::const_iterator aSIt = aConstraintEntities.begin(),
          aSLast = aConstraintEntities.end();
        EntityWrapperPtr aCenterPointEntity = *aSIt++;
        if (aCenterPointChanged) {
          AttributePtr aCenterPointAttr = aData->attribute(SketchPlugin_MultiRotation::CENTER_ID());
          myStorage->update(aCenterPointAttr);
          aCenterPointEntity = myStorage->entity(aCenterPointAttr);
        }
        anEntities.push_back(aCenterPointEntity);

        for (; aSIt != aSLast; ++aSIt)
          anEntities.push_back(*aSIt);

        aConstraint->setEntities(anEntities);
      }
    }
    myStorage->addConstraint(myBaseConstraint, aConstraints);

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
