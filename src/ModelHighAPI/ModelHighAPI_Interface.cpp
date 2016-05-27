// Name   : ModelHighAPI_Interface.cpp
// Purpose: 
//
// History:
// 17/05/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "ModelHighAPI_Interface.h"
//--------------------------------------------------------------------------------------
#include <Events_Error.h>

#include <ModelAPI_Feature.h>
//--------------------------------------------------------------------------------------
ModelHighAPI_Interface::ModelHighAPI_Interface(const std::shared_ptr<ModelAPI_Feature> & theFeature)
: myFeature(theFeature)
{

}

ModelHighAPI_Interface::~ModelHighAPI_Interface()
{

}

//--------------------------------------------------------------------------------------
std::shared_ptr<ModelAPI_Feature> ModelHighAPI_Interface::feature() const
{
  return myFeature;
}

const std::string& ModelHighAPI_Interface::getKind()
{
  return feature()->getKind();
}

void ModelHighAPI_Interface::execute()
{
  feature()->execute();
}

void ModelHighAPI_Interface::throwException(const std::string & theDescription)
{
  Events_Error::send(theDescription);
}
