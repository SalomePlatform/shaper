// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "ModelHighAPI_Selection.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultCompSolid.h>
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
ModelHighAPI_Selection::ModelHighAPI_Selection()
: myVariantType(VT_Empty)
{
}

ModelHighAPI_Selection::ModelHighAPI_Selection(const std::shared_ptr<ModelAPI_Result>& theContext,
                                               const std::shared_ptr<GeomAPI_Shape>& theSubShape)
: myVariantType(VT_ResultSubShapePair)
, myResultSubShapePair(theContext, theSubShape)
{
}

ModelHighAPI_Selection::ModelHighAPI_Selection(const std::string& theType,
                                               const std::string& theSubShapeName)
: myVariantType(VT_TypeSubShapeNamePair)
, myTypeSubShapeNamePair(theType, theSubShapeName)
{
}

ModelHighAPI_Selection::~ModelHighAPI_Selection()
{
}

//--------------------------------------------------------------------------------------
void ModelHighAPI_Selection::fillAttribute(
    const std::shared_ptr<ModelAPI_AttributeSelection> & theAttribute) const
{
  switch(myVariantType) {
    case VT_Empty: return;
    case VT_ResultSubShapePair:
      theAttribute->setValue(myResultSubShapePair.first, myResultSubShapePair.second);
      return;
    case VT_TypeSubShapeNamePair:
      theAttribute->selectSubShape(myTypeSubShapeNamePair.first, myTypeSubShapeNamePair.second);
      if(theAttribute->isInvalid()) {
        FeaturePtr aFeature = ModelAPI_Feature::feature(theAttribute->owner());
        aFeature->setError(
          std::string("Error: attribute \"") + theAttribute->id() + std::string("\" is invalid."));
      }
      return;
  }
}

//--------------------------------------------------------------------------------------
void ModelHighAPI_Selection::appendToList(
    const std::shared_ptr<ModelAPI_AttributeSelectionList> & theAttribute) const
{
  switch(myVariantType) {
    case VT_Empty: return;
    case VT_ResultSubShapePair:
      theAttribute->append(myResultSubShapePair.first, myResultSubShapePair.second);
      return;
    case VT_TypeSubShapeNamePair:
      // Note: the reverse order (first - type, second - sub-shape name)
      theAttribute->append(myTypeSubShapeNamePair.second, myTypeSubShapeNamePair.first);
      return;
  }
}

//==================================================================================================
ModelHighAPI_Selection::VariantType ModelHighAPI_Selection::variantType() const
{
  return myVariantType;
}

//==================================================================================================
ResultSubShapePair ModelHighAPI_Selection::resultSubShapePair() const
{
  return myResultSubShapePair;
}

//==================================================================================================
TypeSubShapeNamePair ModelHighAPI_Selection::typeSubShapeNamePair() const
{
  return myTypeSubShapeNamePair;
}

//==================================================================================================
std::string ModelHighAPI_Selection::shapeType() const
{
  switch(myVariantType) {
  case VT_ResultSubShapePair:
    return myResultSubShapePair.second.get() ? myResultSubShapePair.second->shapeTypeStr() :
                                               myResultSubShapePair.first->shape()->shapeTypeStr();
  case VT_TypeSubShapeNamePair: return myTypeSubShapeNamePair.first;
  }

  return "SHAPE";
}

//==================================================================================================
void ModelHighAPI_Selection::setName(const std::string& theName)
{
  if (myVariantType == VT_ResultSubShapePair) {
    std::shared_ptr<ModelAPI_Result> aResult = myResultSubShapePair.first;
    if(!aResult.get()) {
      return;
    }
    aResult->data()->setName(theName);
  }
}

void ModelHighAPI_Selection::setColor(int theRed, int theGreen, int theBlue)
{
  if (myVariantType != VT_ResultSubShapePair)
    return;

  AttributeIntArrayPtr aColor =
      myResultSubShapePair.first->data()->intArray(ModelAPI_Result::COLOR_ID());
  aColor->setSize(3);
  aColor->setValue(0, theRed);
  aColor->setValue(1, theGreen);
  aColor->setValue(2, theBlue);
}

void ModelHighAPI_Selection::setDeflection(double theValue)
{
  if (myVariantType != VT_ResultSubShapePair)
    return;

  AttributeDoublePtr aDeflectionAttr =
    myResultSubShapePair.first->data()->real(ModelAPI_Result::DEFLECTION_ID());

  aDeflectionAttr->setValue(theValue);
}

int ModelHighAPI_Selection::numberOfSubs() const
{
  if (myVariantType != VT_ResultSubShapePair)
    return 0;

  ResultCompSolidPtr aCompSolid =
      std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(myResultSubShapePair.first);
  if (!aCompSolid)
    return 0;

  return aCompSolid->numberOfSubs();
}

ModelHighAPI_Selection ModelHighAPI_Selection::subResult(int theIndex) const
{
  if (myVariantType != VT_ResultSubShapePair)
    return ModelHighAPI_Selection();

  ResultCompSolidPtr aCompSolid =
      std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(myResultSubShapePair.first);
  if (!aCompSolid)
    return ModelHighAPI_Selection();

  ResultBodyPtr aResult = aCompSolid->subResult(theIndex);
  return ModelHighAPI_Selection(aResult, aResult->shape());
}
