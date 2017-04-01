// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <SketchSolver_ConstraintMultiRotation.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <PlaneGCSSolver_AttributeBuilder.h>
#include <PlaneGCSSolver_PointWrapper.h>
#include <PlaneGCSSolver_ScalarWrapper.h>
#include <PlaneGCSSolver_UpdateFeature.h>

#include <SketchPlugin_MultiRotation.h>

#include <ModelAPI_AttributeString.h>

#include <cmath>

void SketchSolver_ConstraintMultiRotation::getAttributes(
    EntityWrapperPtr& theCenter, EntityWrapperPtr& theAngle,
    bool& theFullValue, std::list<EntityWrapperPtr>& theEntities)
{
  AttributePtr anAngleAttr = myBaseConstraint->attribute(SketchPlugin_MultiRotation::ANGLE_ID());
  PlaneGCSSolver_AttributeBuilder aValueBuilder;
  theAngle = aValueBuilder.createAttribute(anAngleAttr);
  myStorage->addEntity(anAngleAttr, theAngle);

  AttributeRefAttrPtr aCenterAttr = myBaseConstraint->refattr(SketchPlugin_MultiRotation::CENTER_ID());
  if (!aCenterAttr || !aCenterAttr->isInitialized()) {
    myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
    return;
  }

  myType = CONSTRAINT_MULTI_ROTATION;

  myStorage->update(aCenterAttr);
  theCenter = myStorage->entity(aCenterAttr);

  AttributeStringPtr aMethodTypeAttr =
      myBaseConstraint->string(SketchPlugin_MultiRotation::ANGLE_TYPE());
  theFullValue = aMethodTypeAttr->value() != "SingleAngle";

  getEntities(theEntities);

  // add owner of central point of Multi-Rotation to the list of monitored features
  FeaturePtr anOwner = ModelAPI_Feature::feature(aCenterAttr->attr()->owner());
  if (anOwner)
    myFeatures.insert(anOwner);
}

void SketchSolver_ConstraintMultiRotation::process()
{
  cleanErrorMsg();
  if (!myBaseConstraint || !myStorage) {
    // Not enough parameters are assigned
    return;
  }

  EntityWrapperPtr anAngle;
  EntityWrapperPtr aRotationCenter;
  bool isFullValue;
  std::list<EntityWrapperPtr> aBaseEntities;
  getAttributes(aRotationCenter, anAngle, isFullValue, aBaseEntities);
  if (!myErrorMsg.empty())
    return;

  ScalarWrapperPtr anAngleVal = std::dynamic_pointer_cast<PlaneGCSSolver_ScalarWrapper>(anAngle);
  myAngle = anAngleVal->value();
  myAdjusted = false;
  adjustConstraint();

  myStorage->subscribeUpdates(this, PlaneGCSSolver_UpdateFeature::GROUP());
}

void SketchSolver_ConstraintMultiRotation::updateLocal()
{
  double aValue = myBaseConstraint->real(SketchPlugin_MultiRotation::ANGLE_ID())->value();
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

  if (aCenterPointChanged || isMethodChanged)
    myAdjusted = false;
}

void SketchSolver_ConstraintMultiRotation::adjustConstraint()
{
  if (myAdjusted)
    return;

  if (fabs(myAngle) < tolerance) {
    myStorage->setNeedToResolve(false);
    return;
  }

  // Obtain coordinates of rotation center
  std::shared_ptr<PlaneGCSSolver_PointWrapper> aRotCenter =
      std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(myStorage->entity(
      myBaseConstraint->attribute(SketchPlugin_MultiRotation::CENTER_ID())));
  GCSPointPtr aCenterPoint = aRotCenter->point();
  myCenterCoord[0] = *(aCenterPoint->x);
  myCenterCoord[1] = *(aCenterPoint->y);

  double anAngleValue = myAngle;
  if (myIsFullValue && myNumberOfCopies > 0)
    anAngleValue /= myNumberOfCopies;

  myRotationVal[0] = sin(anAngleValue * PI / 180.0);
  myRotationVal[1] = cos(anAngleValue * PI / 180.0);

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
