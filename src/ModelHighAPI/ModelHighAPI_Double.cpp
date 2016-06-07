// Name   : ModelHighAPI_Double.cpp
// Purpose: 
//
// History:
// 29/03/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "ModelHighAPI_Double.h"

#include <ModelAPI_AttributeDouble.h>
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
ModelHighAPI_Double::ModelHighAPI_Double(double theValue)
: myValue(theValue)
{
}

ModelHighAPI_Double::ModelHighAPI_Double(const std::string & theValue)
: myValue(theValue)
{
}

ModelHighAPI_Double::ModelHighAPI_Double(const char * theValue)
: myValue(theValue)
{
}

ModelHighAPI_Double::~ModelHighAPI_Double()
{
}

//--------------------------------------------------------------------------------------
struct fill_visitor : boost::static_visitor<void>
{
  mutable std::shared_ptr<ModelAPI_AttributeDouble> myAttribute;

  fill_visitor(const std::shared_ptr<ModelAPI_AttributeDouble> & theAttribute)
  : myAttribute(theAttribute) {}

  void operator()(double theValue) const { myAttribute->setValue(theValue); }
  void operator()(const std::string & theValue) const { myAttribute->setText(theValue); }
};

void ModelHighAPI_Double::fillAttribute(
    const std::shared_ptr<ModelAPI_AttributeDouble> & theAttribute) const
{
  boost::apply_visitor(fill_visitor(theAttribute), myValue);
}
