// Copyright (C) 2014-2023  CEA/DEN, EDF R&D
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

#include <SketchSolver_ConstraintMultiTranslation.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <PlaneGCSSolver_PointWrapper.h>

#include <SketchPlugin_MultiTranslation.h>

#include <ModelAPI_AttributeString.h>

#include <GeomDataAPI_Point2D.h>

void SketchSolver_ConstraintMultiTranslation::getAttributes(
    EntityWrapperPtr& theStartPoint, EntityWrapperPtr& theEndPoint,
    bool& theFullValue, std::list<EntityWrapperPtr>& theEntities)
{
  DataPtr aData = myBaseConstraint->data();
  AttributeRefAttrPtr aStartPointAttr =
      aData->refattr(SketchPlugin_MultiTranslation::START_POINT_ID());
  AttributeRefAttrPtr aEndPointAttr =
      aData->refattr(SketchPlugin_MultiTranslation::END_POINT_ID());
  if (!aStartPointAttr || !aStartPointAttr->isInitialized() ||
      !aEndPointAttr || !aEndPointAttr->isInitialized()) {
    myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
    return;
  }

  myType = CONSTRAINT_MULTI_TRANSLATION;

  myStorage->update(AttributePtr(aStartPointAttr));
  theStartPoint = myStorage->entity(AttributePtr(aStartPointAttr));
  myStorage->update(AttributePtr(aEndPointAttr));
  theEndPoint = myStorage->entity(AttributePtr(aEndPointAttr));

  AttributeStringPtr aMethodTypeAttr = aData->string(SketchPlugin_MultiTranslation::VALUE_TYPE());
  theFullValue = aMethodTypeAttr->value() != "SingleValue";

  getEntities(theEntities);

  // add owner of start and end points of Multi-Translation to the list of monitored features
  FeaturePtr anOwner = ModelAPI_Feature::feature(aStartPointAttr->attr()->owner());
  if (anOwner)
    myOriginalFeatures.insert(anOwner);
  anOwner = ModelAPI_Feature::feature(aEndPointAttr->attr()->owner());
  if (anOwner)
    myOriginalFeatures.insert(anOwner);
}

void SketchSolver_ConstraintMultiTranslation::process()
{
  cleanErrorMsg();
  if (!myBaseConstraint || !myStorage) {
    // Not enough parameters are assigned
    return;
  }

  EntityWrapperPtr aStartPoint, aEndPoint;
  std::list<EntityWrapperPtr> aBaseEntities;
  getAttributes(aStartPoint, aEndPoint, myIsFullValue, aBaseEntities);
  if (!myErrorMsg.empty())
    return;

  myAdjusted = false;
  adjustConstraint();

  myStorage->subscribeUpdates(this, PlaneGCSSolver_UpdateFeature::GROUP());
}

const std::string& SketchSolver_ConstraintMultiTranslation::nameNbObjects()
{
  return SketchPlugin_MultiTranslation::NUMBER_OF_OBJECTS_ID();
}

void SketchSolver_ConstraintMultiTranslation::updateLocal()
{
  DataPtr aData = myBaseConstraint->data();
  AttributePoint2DPtr aStartPointAttribute = GeomDataAPI_Point2D::getPoint2D(aData,
                                      SketchPlugin_MultiTranslation::START_POINT_ID());
  AttributePoint2DPtr anEndPointAttribute = GeomDataAPI_Point2D::getPoint2D(aData,
                                      SketchPlugin_MultiTranslation::END_POINT_ID());
  AttributeStringPtr aMethodTypeAttr = aData->string(SketchPlugin_MultiTranslation::VALUE_TYPE());
  bool aFullValue = aMethodTypeAttr->value() != "SingleValue";

  bool aStartPointChanged = aStartPointAttribute != myStartPointAttribute;
  bool anEndPointChanged = anEndPointAttribute != myEndPointAttribute;
  bool isMethodChanged = aFullValue != myIsFullValue;

  if (aStartPointChanged)
    myStartPointAttribute = aStartPointAttribute;
  if (aStartPointChanged)
    myEndPointAttribute = anEndPointAttribute;
  if (isMethodChanged)
    myIsFullValue = aFullValue;

  if (aStartPointChanged || anEndPointChanged || isMethodChanged)
    myAdjusted = false;
}

void SketchSolver_ConstraintMultiTranslation::adjustConstraint()
{
  if (myAdjusted)
    return;

  // Obtain delta between start and end points of translation
  AttributeRefAttrPtr aStartEnd[2] = {
      myBaseConstraint->refattr(SketchPlugin_MultiTranslation::START_POINT_ID()),
      myBaseConstraint->refattr(SketchPlugin_MultiTranslation::END_POINT_ID())
  };
  double aCoords[2][2];
  for (int i = 0; i < 2; ++i)
  {
    std::shared_ptr<PlaneGCSSolver_PointWrapper> aPointWrapper =
        std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(
        myStorage->entity(AttributePtr(aStartEnd[i])));
    if (aPointWrapper)
    {
      GCSPointPtr aPnt = aPointWrapper->point();
      aCoords[i][0] = *(aPnt->x);
      aCoords[i][1] = *(aPnt->y);
    }
    else
    {
      AttributePoint2DPtr aPnt =
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aStartEnd[i]->attr());
      aCoords[i][0] = aPnt->x();
      aCoords[i][1] = aPnt->y();
    }
  }

  myDelta[0] = aCoords[1][0] - aCoords[0][0];
  myDelta[1] = aCoords[1][1] - aCoords[0][1];

  if (myIsFullValue && myNumberOfCopies > 0) {
    myDelta[0] /= myNumberOfCopies;
    myDelta[1] /= myNumberOfCopies;
  }

  SketchSolver_ConstraintMulti::adjustConstraint();
}

void SketchSolver_ConstraintMultiTranslation::getRelative(
    double theAbsX, double theAbsY, double& theRelX, double& theRelY)
{
  theRelX = theAbsX;
  theRelY = theAbsY;
}

void SketchSolver_ConstraintMultiTranslation::getAbsolute(
    double theRelX, double theRelY, double& theAbsX, double& theAbsY)
{
  theAbsX = theRelX;
  theAbsY = theRelY;
}

void SketchSolver_ConstraintMultiTranslation::transformRelative(double& theX, double& theY)
{
  // translate coordinates
  theX += myDelta[0];
  theY += myDelta[1];
}

