// Name   : ModelHighAPI_Integer.cpp
// Purpose: 
//
// History:
// 29/03/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "ModelHighAPI_Integer.h"

#include <ModelAPI_AttributeInteger.h>
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
ModelHighAPI_Integer::ModelHighAPI_Integer(int theValue)
: myValue(theValue)
{
}

ModelHighAPI_Integer::ModelHighAPI_Integer(const std::string & theValue)
: myValue(theValue)
{
}

ModelHighAPI_Integer::ModelHighAPI_Integer(const char * theValue)
: myValue(theValue)
{
}

ModelHighAPI_Integer::~ModelHighAPI_Integer()
{
}

//--------------------------------------------------------------------------------------
struct fill_visitor : boost::static_visitor<void>
{
  mutable std::shared_ptr<ModelAPI_AttributeInteger> myAttribute;

  fill_visitor(const std::shared_ptr<ModelAPI_AttributeInteger> & theAttribute)
  : myAttribute(theAttribute) {}

  void operator()(int theValue) const { myAttribute->setValue(theValue); }
  void operator()(const std::string & theValue) const { myAttribute->setText(theValue); }
};

void ModelHighAPI_Integer::fillAttribute(
    const std::shared_ptr<ModelAPI_AttributeInteger> & theAttribute) const
{
  boost::apply_visitor(fill_visitor(theAttribute), myValue);
}
