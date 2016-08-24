// Name   : ExchangeAPI_Export.cpp
// Purpose: 
//
// History:
// 07/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "ExchangeAPI_Export.h"
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Tools.h>
//--------------------------------------------------------------------------------------
void exportToFile(const std::shared_ptr<ModelAPI_Document> & thePart,
                  const std::string & theFilePath,
                  const std::list<ModelHighAPI_Selection> & theSelectionList,
                  const std::string & theFileFormat)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ExchangePlugin_ExportFeature::ID());
  fillAttribute("Regular", aFeature->string(ExchangePlugin_ExportFeature::EXPORT_TYPE_ID()));
  fillAttribute(theFilePath, aFeature->string(ExchangePlugin_ExportFeature::FILE_PATH_ID()));
  fillAttribute(theSelectionList, aFeature->selectionList(ExchangePlugin_ExportFeature::SELECTION_LIST_ID()));
  fillAttribute(theFileFormat, aFeature->string(ExchangePlugin_ExportFeature::FILE_FORMAT_ID()));
  aFeature->execute();
}

void exportToXAO(const std::shared_ptr<ModelAPI_Document> & thePart,
                 const std::string & theFilePath,
                 const std::string & theAuthor,
                 const std::string & theGeometryName)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ExchangePlugin_ExportFeature::ID());
  fillAttribute("XAO", aFeature->string(ExchangePlugin_ExportFeature::EXPORT_TYPE_ID()));
  fillAttribute(theFilePath, aFeature->string(ExchangePlugin_ExportFeature::XAO_FILE_PATH_ID()));
  fillAttribute(theAuthor, aFeature->string(ExchangePlugin_ExportFeature::XAO_AUTHOR_ID()));
  fillAttribute(theGeometryName, aFeature->string(ExchangePlugin_ExportFeature::XAO_GEOMETRY_NAME_ID()));
  fillAttribute("XAO", aFeature->string(ExchangePlugin_ExportFeature::FILE_FORMAT_ID()));
  aFeature->execute();
}

//--------------------------------------------------------------------------------------
