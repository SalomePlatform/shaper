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
: myValue(ResultSubShapePair(theContext, theSubShape))
{
}

ModelHighAPI_Selection::ModelHighAPI_Selection(const std::string& theType,
                                               const std::string& theSubShapeName)
: myValue(TypeSubShapeNamePair(theType, theSubShapeName))
{
}

ModelHighAPI_Selection::~ModelHighAPI_Selection()
{
}

//--------------------------------------------------------------------------------------
struct fill_visitor : boost::static_visitor<void>
{
  mutable std::shared_ptr<ModelAPI_AttributeSelection> myAttribute;

  fill_visitor(const std::shared_ptr<ModelAPI_AttributeSelection> & theAttribute)
  : myAttribute(theAttribute) {}

  void operator()(const ResultSubShapePair & thePair) const { myAttribute->setValue(thePair.first, thePair.second); }
  void operator()(const TypeSubShapeNamePair & thePair) const { myAttribute->selectSubShape(thePair.first, thePair.second); }
};

void ModelHighAPI_Selection::fillAttribute(
    const std::shared_ptr<ModelAPI_AttributeSelection> & theAttribute) const
{
  boost::apply_visitor(fill_visitor(theAttribute), myValue);
}

//--------------------------------------------------------------------------------------
struct append_visitor : boost::static_visitor<void>
{
  mutable std::shared_ptr<ModelAPI_AttributeSelectionList> myAttribute;

  append_visitor(const std::shared_ptr<ModelAPI_AttributeSelectionList> & theAttribute)
  : myAttribute(theAttribute) {}

  void operator()(const ResultSubShapePair & thePair) const { myAttribute->append(thePair.first, thePair.second); }
  void operator()(const TypeSubShapeNamePair & thePair) const {
    // Note: the reverse order (first - type, second - sub-shape name)
    myAttribute->append(thePair.second, thePair.first);
  }
};

void ModelHighAPI_Selection::appendToList(
    const std::shared_ptr<ModelAPI_AttributeSelectionList> & theAttribute) const
{
  boost::apply_visitor(append_visitor(theAttribute), myValue);
}
