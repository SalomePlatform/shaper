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
    EntityWrapperPtr& theCenter, ScalarWrapperPtr& theAngle,
    bool& theFullValue, bool& theReversed, std::list<EntityWrapperPtr>& theEntities)
{
  AttributePtr anAngleAttr = myBaseConstraint->attribute(SketchPlugin_MultiRotation::ANGLE_ID());
  PlaneGCSSolver_AttributeBuilder aValueBuilder;
  theAngle = std::dynamic_pointer_cast<PlaneGCSSolver_ScalarWrapper>(
      aValueBuilder.createAttribute(anAngleAttr));
  myStorage->addEntity(anAngleAttr, theAngle);

  AttributeRefAttrPtr aCenterAttr =
      myBaseConstraint->refattr(SketchPlugin_MultiRotation::CENTER_ID());
  if (!aCenterAttr || !aCenterAttr->isInitialized()) {
    myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
    return;
  }

  myType = CONSTRAINT_MULTI_ROTATION;

  myStorage->update(AttributePtr(aCenterAttr));
  theCenter = myStorage->entity(AttributePtr(aCenterAttr));

  AttributeStringPtr aMethodTypeAttr =
      myBaseConstraint->string(SketchPlugin_MultiRotation::ANGLE_TYPE());
  theFullValue = aMethodTypeAttr->value() != "SingleAngle";

  theReversed = myBaseConstraint->boolean(SketchPlugin_MultiRotation::REVERSED_ID())->value();

  getEntities(theEntities);

  // add owner of central point of Multi-Rotation to the list of monitored features
  FeaturePtr anOwner = ModelAPI_Feature::feature(aCenterAttr->attr()->owner());
  if (anOwner)
    myOriginalFeatures.insert(anOwner);
}

void SketchSolver_ConstraintMultiRotation::process()
{
  cleanErrorMsg();
  if (!myBaseConstraint || !myStorage) {
    // Not enough parameters are assigned
    return;
  }

  EntityWrapperPtr aRotationCenter;
  std::list<EntityWrapperPtr> aBaseEntities;
  getAttributes(aRotationCenter, myAngle, myIsFullValue, myIsRevered, aBaseEntities);
  if (!myErrorMsg.empty())
    return;

  myAdjusted = false;
  adjustConstraint();

  myStorage->subscribeUpdates(this, PlaneGCSSolver_UpdateFeature::GROUP());
}

void SketchSolver_ConstraintMultiRotation::updateLocal()
{
  double aValue = myBaseConstraint->real(SketchPlugin_MultiRotation::ANGLE_ID())->value();
  bool isReversed = myBaseConstraint->boolean(SketchPlugin_MultiRotation::REVERSED_ID())->value();
  if (fabs(myAngle->value() - aValue) > tolerance || isReversed != myIsRevered)
    myAdjusted = false;
  // update angle value
  myAngle->setValue(aValue);
  myIsRevered = isReversed;

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

  double anAngleValue = myAngle->value();
  if (fabs(anAngleValue) < tolerance) {
    myStorage->setNeedToResolve(false);
    return;
  }
  if (myIsRevered)
    anAngleValue *= -1.0;

  // Obtain coordinates of rotation center
  AttributeRefAttrPtr aCenterAttr =
      myBaseConstraint->refattr(SketchPlugin_MultiRotation::CENTER_ID());
  std::shared_ptr<PlaneGCSSolver_PointWrapper> aRotCenter =
      std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(
      myStorage->entity(AttributePtr(aCenterAttr)));
  if (aRotCenter)
  {
    GCSPointPtr aCenterPoint = aRotCenter->point();
    myCenterCoord[0] = *(aCenterPoint->x);
    myCenterCoord[1] = *(aCenterPoint->y);
  }
  else
  {
    AttributePoint2DPtr aCenterPnt =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aCenterAttr->attr());
    myCenterCoord[0] = aCenterPnt->x();
    myCenterCoord[1] = aCenterPnt->y();
  }

  if (myIsFullValue && myNumberOfCopies > 0)
  {
    // if the full angle value is equal to 360, then distribute rotated items
    // to avoid superposition of original feature and last copy
    if (fabs(anAngleValue - 360.0) < 1.e-7)
      anAngleValue /= (myNumberOfCopies + 1);
    else
      anAngleValue /= myNumberOfCopies;
  }

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
