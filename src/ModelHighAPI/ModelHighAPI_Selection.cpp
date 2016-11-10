// Copyright (C) 2014-20xx CEA/DEN, EDF R&D
// Name   : ModelHighAPI_Selection.cpp
// Purpose:
//
// History:
// 06/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "ModelHighAPI_Selection.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
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
  if (myVariantType == VT_ResultSubShapePair)
    myResultSubShapePair.first->data()->setName(theName);
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
