// Name   : PartSetAPI_Part.cpp
// Purpose: 
//
// History:
// 16/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "PartSetAPI_Part.h"
//--------------------------------------------------------------------------------------
#include <ModelAPI_ResultPart.h>
//--------------------------------------------------------------------------------------
#include <PartSetPlugin_Duplicate.h>
#include <PartSetPlugin_Remove.h>
//--------------------------------------------------------------------------------------
PartSetAPI_Part::PartSetAPI_Part(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

PartSetAPI_Part::~PartSetAPI_Part()
{
}

//--------------------------------------------------------------------------------------
std::shared_ptr<ModelAPI_Document> PartSetAPI_Part::document() const
{
  return std::dynamic_pointer_cast<ModelAPI_ResultPart>(defaultResult())->partDoc();
}

//--------------------------------------------------------------------------------------
PartPtr addPart(const std::shared_ptr<ModelAPI_Document> & thePart)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(PartSetAPI_Part::ID());
  aFeature->execute();
  return PartPtr(new PartSetAPI_Part(aFeature));
}

PartPtr duplicatePart(const std::shared_ptr<ModelAPI_Document> & thePart)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(PartSetPlugin_Duplicate::ID());
  aFeature->execute();
  return PartPtr(new PartSetAPI_Part(aFeature));
}

void removePart(const std::shared_ptr<ModelAPI_Document> & thePart)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(PartSetPlugin_Remove::ID());
  aFeature->execute();
}
