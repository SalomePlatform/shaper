// Name   : ExchangeAPI_Export.cpp
// Purpose: 
//
// History:
// 07/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "ExchangeAPI_Export.h"

#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>
//--------------------------------------------------------------------------------------
ExchangeAPI_Export::ExchangeAPI_Export(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

ExchangeAPI_Export::ExchangeAPI_Export(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const std::string & theFilePath,
    const std::string & theFileFormat,
    const std::list<ModelHighAPI_Selection> & theSelectionList)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    setFilePath(theFilePath);
    setFileFormat(theFileFormat);
    setSelectionList(theSelectionList);
    execute();
  }
}

ExchangeAPI_Export::~ExchangeAPI_Export()
{

}

//--------------------------------------------------------------------------------------
void ExchangeAPI_Export::setFilePath(const std::string & theFilePath)
{
  fillAttribute(theFilePath, myfilePath);
}

void ExchangeAPI_Export::setFileFormat(const std::string & theFileFormat)
{
  fillAttribute(theFileFormat, myfileFormat);
}

void ExchangeAPI_Export::setSelectionList(
    const std::list<ModelHighAPI_Selection> & theSelectionList)
{
  fillAttribute(theSelectionList, myselectionList);
}

//--------------------------------------------------------------------------------------
ExportPtr exportToFile(
    const std::shared_ptr<ModelAPI_Document> & thePart,
    const std::string & theFilePath,
    const std::string & theFileFormat,
    const std::list<ModelHighAPI_Selection> & theSelectionList)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ExchangeAPI_Export::ID());
  return ExportPtr(new ExchangeAPI_Export(aFeature, theFilePath, theFileFormat, theSelectionList));
}
