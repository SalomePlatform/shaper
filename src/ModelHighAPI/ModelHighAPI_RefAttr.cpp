// Name   : ModelHighAPI_RefAttr.cpp
// Purpose: 
//
// History:
// 08/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "ModelHighAPI_RefAttr.h"

#include <ModelAPI_AttributeRefAttr.h>
//--------------------------------------------------------------------------------------
#include <iostream>
//--------------------------------------------------------------------------------------
ModelHighAPI_RefAttr::ModelHighAPI_RefAttr()
{
}

ModelHighAPI_RefAttr::ModelHighAPI_RefAttr(
    const std::shared_ptr<ModelAPI_Attribute> & theValue)
: myValue(theValue)
{
}

ModelHighAPI_RefAttr::ModelHighAPI_RefAttr(
    const std::shared_ptr<ModelAPI_Object> & theValue)
: myValue(theValue)
{
}

ModelHighAPI_RefAttr::~ModelHighAPI_RefAttr()
{
}

//--------------------------------------------------------------------------------------
struct fill_visitor : boost::static_visitor<void>
{
  mutable std::shared_ptr<ModelAPI_AttributeRefAttr> myAttribute;

  fill_visitor(const std::shared_ptr<ModelAPI_AttributeRefAttr> & theAttribute)
  : myAttribute(theAttribute) {}

  void operator()(const std::shared_ptr<ModelAPI_Attribute>& theValue) const { myAttribute->setAttr(theValue); }
  void operator()(const std::shared_ptr<ModelAPI_Object>& theValue) const { myAttribute->setObject(theValue); }
};

void ModelHighAPI_RefAttr::fillAttribute(
    const std::shared_ptr<ModelAPI_AttributeRefAttr> & theAttribute) const
{
  boost::apply_visitor(fill_visitor(theAttribute), myValue);
}
