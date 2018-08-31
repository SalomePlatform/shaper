// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "ExchangeAPI_Export.h"
//--------------------------------------------------------------------------------------
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelHighAPI_Tools.h>
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Services.h>
//--------------------------------------------------------------------------------------

ExchangeAPI_Export::ExchangeAPI_Export(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

/// Constructor with values for XAO export.
ExchangeAPI_Export::ExchangeAPI_Export(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                              const std::string & theFilePath,
                              const std::string & theAuthor,
                              const std::string & theGeometryName)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
  fillAttribute("XAO", theFeature->string(ExchangePlugin_ExportFeature::EXPORT_TYPE_ID()));
  fillAttribute(theFilePath, theFeature->string(ExchangePlugin_ExportFeature::XAO_FILE_PATH_ID()));
  fillAttribute(theAuthor, theFeature->string(ExchangePlugin_ExportFeature::XAO_AUTHOR_ID()));
  fillAttribute(theGeometryName,
                theFeature->string(ExchangePlugin_ExportFeature::XAO_GEOMETRY_NAME_ID()));
  fillAttribute("XAO", theFeature->string(ExchangePlugin_ExportFeature::FILE_FORMAT_ID()));
  execute();
  apply(); // finish operation to make sure the export is done on the current state of the history
}

/// Constructor with values for export in other formats than XAO.
ExchangeAPI_Export::ExchangeAPI_Export(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                              const std::string & theFilePath,
                              const std::list<ModelHighAPI_Selection> & theSelectionList,
                              const std::string & theFileFormat)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
  fillAttribute("Regular", theFeature->string(ExchangePlugin_ExportFeature::EXPORT_TYPE_ID()));
  fillAttribute(theFilePath, theFeature->string(ExchangePlugin_ExportFeature::FILE_PATH_ID()));
  fillAttribute(theSelectionList,
                theFeature->selectionList(ExchangePlugin_ExportFeature::SELECTION_LIST_ID()));
  fillAttribute(theFileFormat, theFeature->string(ExchangePlugin_ExportFeature::FILE_FORMAT_ID()));
  execute();
  apply(); // finish operation to make sure the export is done on the current state of the history
}

ExchangeAPI_Export::~ExchangeAPI_Export()
{
}

// this method is needed on Windows because back-slashes in python may cause error
static void correctSeparators(std::string& thePath) {
  // replace single "\" or triple "\\\" or more by double "\"
  for (std::size_t aFind = thePath.find('\\'); aFind != std::string::npos;
    aFind = thePath.find('\\', aFind)) {
    // search the next
    std::size_t aFind2 = thePath.find('\\', aFind + 1);
    if (aFind2 == std::string::npos || aFind2 > aFind + 1) { // single, so add one more
      thePath.replace(aFind, 1, 2, '\\');
    } else { // if there is more than double "\", remove them
      for (aFind2 = thePath.find('\\', aFind2 + 1);
           aFind2 != std::string::npos && aFind2 <= aFind + 2;
           aFind2 = thePath.find('\\', aFind2)) {
        thePath.erase(aFind2, 1);
      }
    }
    aFind += 2;
  }
}

void ExchangeAPI_Export::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  theDumper << aBase << " = model.";

  std::string exportType = aBase->string(ExchangePlugin_ExportFeature::EXPORT_TYPE_ID())->value();

  if (exportType == "XAO") {
    std::string aTmpXAOFile =
                aBase->string(ExchangePlugin_ExportFeature::XAO_FILE_PATH_ID())->value();
    correctSeparators(aTmpXAOFile);
    theDumper << "exportToXAO(" << aDocName << ", '" << aTmpXAOFile << "'" ;
    std::string theAuthor = aBase->string(ExchangePlugin_ExportFeature::XAO_AUTHOR_ID())->value();
    if (! theAuthor.empty())
      theDumper << ", '" << theAuthor << "'";
    std::string theGeometryName =
                aBase->string(ExchangePlugin_ExportFeature::XAO_GEOMETRY_NAME_ID())->value();
    if (! theGeometryName.empty())
      theDumper << ", '" << theGeometryName << "'";
    theDumper << ")" << std::endl;
  }
  else {
    std::string aFilePath = aBase->string(ExchangePlugin_ExportFeature::FILE_PATH_ID())->value();
    correctSeparators(aFilePath);
      theDumper << "exportToFile(" << aDocName << ", \"" << aFilePath << "\", " <<
      aBase->selectionList(ExchangePlugin_ExportFeature::SELECTION_LIST_ID());
    std::string theFileFormat =
      aBase->string(ExchangePlugin_ExportFeature::FILE_FORMAT_ID())->value();
    if (!theFileFormat.empty())
      theDumper << ", '" << theFileFormat << "'";
    theDumper << ")" << std::endl;
  }
}

ExportPtr exportToFile(const std::shared_ptr<ModelAPI_Document> & thePart,
                  const std::string & theFilePath,
                  const std::list<ModelHighAPI_Selection> & theSelectionList,
                  const std::string & theFileFormat)
{
  apply(); // finish previous operation to make sure all previous operations are done
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(ExchangePlugin_ExportFeature::ID());
  return ExportPtr(new ExchangeAPI_Export(aFeature, theFilePath, theSelectionList, theFileFormat));
}

ExportPtr exportToXAO(const std::shared_ptr<ModelAPI_Document> & thePart,
                 const std::string & theFilePath,
                 const std::string & theAuthor,
                 const std::string & theGeometryName)
{
  apply(); // finish previous operation to make sure all previous operations are done
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(ExchangePlugin_ExportFeature::ID());
  return ExportPtr(new ExchangeAPI_Export(aFeature, theFilePath, theAuthor, theGeometryName));
}

//--------------------------------------------------------------------------------------
