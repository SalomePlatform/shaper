// Name   : ExchangeAPI_Import.cpp
// Purpose: 
//
// History:
// 07/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "ExchangeAPI_Import.h"
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Tools.h>
//--------------------------------------------------------------------------------------
ExchangeAPI_Import::ExchangeAPI_Import(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

ExchangeAPI_Import::ExchangeAPI_Import(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const std::string & theFilePath)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize())
    setFilePath(theFilePath);
}

ExchangeAPI_Import::~ExchangeAPI_Import()
{

}

//--------------------------------------------------------------------------------------
void ExchangeAPI_Import::setFilePath(const std::string & theFilePath)
{
  fillAttribute(theFilePath, myfilePath);

  execute();
}

//--------------------------------------------------------------------------------------
ImportPtr addImport(
    const std::shared_ptr<ModelAPI_Document> & thePart,
    const std::string & theFilePath)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ExchangeAPI_Import::ID());
  return ImportPtr(new ExchangeAPI_Import(aFeature, theFilePath));
}
