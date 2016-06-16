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

#include "ModelHighAPI_Selection.h"
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

const std::string& ModelHighAPI_Interface::getKind() const
{
  return feature()->getKind();
}

void ModelHighAPI_Interface::execute()
{
  feature()->execute();
}

std::list<ModelHighAPI_Selection> ModelHighAPI_Interface::result() const
{
  // TODO(spo): should I use more common function for the whole model to prepare results?
  const_cast<ModelHighAPI_Interface*>(this)->execute();

  std::list<ModelHighAPI_Selection> aSelectionList;

  std::list<std::shared_ptr<ModelAPI_Result> > aResults = feature()->results();
  for (auto it = aResults.begin(), end = aResults.end(); it != end; ++it) {
    aSelectionList.push_back(ModelHighAPI_Selection(*it));
  }

  return aSelectionList;
}

std::shared_ptr<ModelAPI_Result> ModelHighAPI_Interface::defaultResult() const
{
  return feature()->lastResult();
}

void ModelHighAPI_Interface::throwException(const std::string & theDescription)
{
  Events_Error::send(theDescription);
}
