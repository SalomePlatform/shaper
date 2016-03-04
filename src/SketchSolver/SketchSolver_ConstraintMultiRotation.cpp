#include <SketchSolver_ConstraintMultiRotation.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <SketchPlugin_MultiRotation.h>

#include <ModelAPI_AttributeString.h>

#include <math.h>

void SketchSolver_ConstraintMultiRotation::getAttributes(
    EntityWrapperPtr& theCenter, double& theAngle,
    bool& theFullValue, std::list<EntityWrapperPtr>& theEntities)
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

  getEntities(theEntities);
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
  std::list<EntityWrapperPtr> aBaseEntities;
  getAttributes(aRotationCenter, myAngle, isFullValue, aBaseEntities);
  if (!myErrorMsg.empty())
    return;

  BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();
  std::list<ConstraintWrapperPtr> aRotConstraints;

  std::list<EntityWrapperPtr>::iterator anEntIt = aBaseEntities.begin();
  for (; anEntIt != aBaseEntities.end(); ++anEntIt) {
    std::list<ConstraintWrapperPtr> aNewConstraints =
        aBuilder->createConstraint(myBaseConstraint, myGroupID, mySketchID, myType,
        myAngle, isFullValue, aRotationCenter, EntityWrapperPtr(), std::list<EntityWrapperPtr>(1, *anEntIt));
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
  if (myAdjusted)
    return;

  if (fabs(myAngle) < tolerance) {
    myStorage->setNeedToResolve(false);
    return;
  }

  const std::list<ConstraintWrapperPtr>& aConstraints = myStorage->constraint(myBaseConstraint);
  std::list<ConstraintWrapperPtr>::const_iterator aCIt = aConstraints.begin();
  for (; aCIt != aConstraints.end(); ++aCIt)
    (*aCIt)->setValue(myAngle);

  // Obtain coordinates of rotation center
  EntityWrapperPtr aRotCenter = myStorage->entity(
      myBaseConstraint->attribute(SketchPlugin_MultiRotation::CENTER_ID()));
  std::list<ParameterWrapperPtr> aParams = aRotCenter->parameters();
  myCenterCoord[0] = aParams.front()->value();
  myCenterCoord[1] = aParams.back()->value();

  myRotationVal[0] = sin(myAngle * PI / 180.0);
  myRotationVal[1] = cos(myAngle * PI / 180.0);

  SketchSolver_ConstraintMulti::adjustConstraint();
}

void SketchSolver_ConstraintMultiRotation::getRelative(
    double theAbsX, double theAbsY, double& theRelX, double& theRelY)
{
  theRelX = theAbsX - myCenterCoord[0];
  theRelY = theAbsY - myCenterCoord[1];
}

void SketchSolver_ConstraintMultiRotation::getAbsolute(
    double theRelX, double theRelY, double& theAbsX, double& theAbsY)
{
  theAbsX = theRelX + myCenterCoord[0];
  theAbsY = theRelY + myCenterCoord[1];
}

void SketchSolver_ConstraintMultiRotation::transformRelative(double& theX, double& theY)
{
  // rotate direction
  // myRotationVal[0] = sinA, myRotationVal[1] = cosA
  double aTemp = theX * myRotationVal[1] - theY * myRotationVal[0];
  theY = theX * myRotationVal[0] + theY * myRotationVal[1];
  theX = aTemp;
}

const std::string& SketchSolver_ConstraintMultiRotation::nameNbObjects()
{
  return SketchPlugin_MultiRotation::NUMBER_OF_OBJECTS_ID();
}
