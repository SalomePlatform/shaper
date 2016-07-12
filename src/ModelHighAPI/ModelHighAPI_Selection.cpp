// Name   : ModelHighAPI_Selection.cpp
// Purpose: 
//
// History:
// 06/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "ModelHighAPI_Selection.h"

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
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
    case VT_ResultSubShapePair: theAttribute->setValue(myResultSubShapePair.first, myResultSubShapePair.second); return;
    case VT_TypeSubShapeNamePair: theAttribute->selectSubShape(myTypeSubShapeNamePair.first, myTypeSubShapeNamePair.second); return;
  }
}

//--------------------------------------------------------------------------------------
void ModelHighAPI_Selection::appendToList(
    const std::shared_ptr<ModelAPI_AttributeSelectionList> & theAttribute) const
{
  switch(myVariantType) {
    case VT_ResultSubShapePair: theAttribute->append(myResultSubShapePair.first, myResultSubShapePair.second); return;
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
