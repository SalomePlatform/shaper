// Copyright (C) 2014-2023  CEA, EDF
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "ExchangeAPI_Export.h"
//--------------------------------------------------------------------------------------
#include <ExchangePlugin_ExportPart.h>
//--------------------------------------------------------------------------------------
#include <Locale_Convert.h>
//--------------------------------------------------------------------------------------
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelHighAPI_Tools.h>
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Services.h>
#include <ModelHighAPI_Selection.h>
//--------------------------------------------------------------------------------------
#include <algorithm>
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

// Constructor with values for STL of selected result export.
ExchangeAPI_Export::ExchangeAPI_Export(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                       const std::string & theFilePath,
                                       const ModelHighAPI_Selection& theSelectedShape,
                                       const ModelHighAPI_Double& theDeflectionRelative,
                                       const ModelHighAPI_Double& theDeflectionAbsolute,
                                       const bool theIsRelative,
                                       const bool theIsASCII)
  : ModelHighAPI_Interface(theFeature)
{
  initialize();
  fillAttribute("STL", theFeature->string(ExchangePlugin_ExportFeature::EXPORT_TYPE_ID()));
  fillAttribute(theFilePath, theFeature->string(ExchangePlugin_ExportFeature::STL_FILE_PATH_ID()));

  if (theIsRelative) {
    fillAttribute(ExchangePlugin_ExportFeature::STL_DEFLECTION_TYPE_RELATIVE(),
      theFeature->string(ExchangePlugin_ExportFeature::STL_DEFLECTION_TYPE()) );
    fillAttribute(theDeflectionRelative,
      theFeature->real(ExchangePlugin_ExportFeature::STL_RELATIVE()) );
  }
  else {
    fillAttribute(ExchangePlugin_ExportFeature::STL_DEFLECTION_TYPE_ABSOLUTE(),
      theFeature->string(ExchangePlugin_ExportFeature::STL_DEFLECTION_TYPE()) );
    fillAttribute(theDeflectionAbsolute,
      theFeature->real(ExchangePlugin_ExportFeature::STL_ABSOLUTE()) );
  }

  if(theIsASCII){
    fillAttribute(ExchangePlugin_ExportFeature::STL_FILE_TYPE_ASCII(),
      theFeature->string(ExchangePlugin_ExportFeature::STL_FILE_TYPE()));
  }
  else
  {
    fillAttribute(ExchangePlugin_ExportFeature::STL_FILE_TYPE_BINARY(),
      theFeature->string(ExchangePlugin_ExportFeature::STL_FILE_TYPE()));
  }

  fillAttribute(theSelectedShape,
                theFeature->selection(ExchangePlugin_ExportFeature::STL_OBJECT_SELECTED()));
  fillAttribute("STL", theFeature->string(ExchangePlugin_ExportFeature::FILE_FORMAT_ID()));
  execute();
  apply(); // finish operation to make sure the export is done on the current state of the history
}

/// Constructor with values for XAO of selected result export.
ExchangeAPI_Export::ExchangeAPI_Export(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                       const std::string & theFilePath,
                                       const ModelHighAPI_Selection& theResult,
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
  std::list<ModelHighAPI_Selection> aListOfOneSel;
  aListOfOneSel.push_back(theResult);
  fillAttribute(aListOfOneSel,
    theFeature->selectionList(ExchangePlugin_ExportFeature::XAO_SELECTION_LIST_ID()));
  execute();
  apply(); // finish operation to make sure the export is done on the current state of the history
}

/// Constructor with values for XAO of selected result export to memory buffer.
ExchangeAPI_Export::ExchangeAPI_Export(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                       const ModelHighAPI_Selection& theResult,
                                       const std::string & theAuthor,
                                       const std::string & theGeometryName)
  : ModelHighAPI_Interface(theFeature)
{
  initialize();
  fillAttribute("XAOMem", theFeature->string(ExchangePlugin_ExportFeature::EXPORT_TYPE_ID()));
  fillAttribute(theAuthor, theFeature->string(ExchangePlugin_ExportFeature::XAO_AUTHOR_ID()));
  fillAttribute(theGeometryName,
                theFeature->string(ExchangePlugin_ExportFeature::XAO_GEOMETRY_NAME_ID()));
  fillAttribute("XAO", theFeature->string(ExchangePlugin_ExportFeature::FILE_FORMAT_ID()));
  std::list<ModelHighAPI_Selection> aListOfOneSel;
  aListOfOneSel.push_back(theResult);
  fillAttribute(aListOfOneSel,
                theFeature->selectionList(ExchangePlugin_ExportFeature::XAO_SELECTION_LIST_ID()));
  execute(true);
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

  std::string exportType = aBase->string(ExchangePlugin_ExportFeature::EXPORT_TYPE_ID())->value();

  if (exportType == "XAOMem") {
    std::string aGeometryName =
      aBase->string(ExchangePlugin_ExportFeature::XAO_GEOMETRY_NAME_ID())->value();

    theDumper << "aXAOBuff";
    std::string aGeometryNamePy;
    if (! aGeometryName.empty()) {
      aGeometryNamePy = aGeometryName;
    }
    else {
      aGeometryNamePy = Locale::Convert::toString(aBase->data()->name());
    }
    if (! aGeometryNamePy.empty()) {
      // add shape name
      std::replace(aGeometryNamePy.begin(), aGeometryNamePy.end(), ' ', '_');
      theDumper << "_" << aGeometryNamePy;
    }
    theDumper << " = model.exportToXAOMem(" << aDocName;
    AttributeSelectionListPtr aShapeSelected =
      aBase->selectionList(ExchangePlugin_ExportFeature::XAO_SELECTION_LIST_ID());
    if (aShapeSelected->isInitialized() && aShapeSelected->size() == 1) {
      theDumper << ", " << aShapeSelected->value(0);
    }

    std::string theAuthor = aBase->string(ExchangePlugin_ExportFeature::XAO_AUTHOR_ID())->value();
    if (! theAuthor.empty())
      theDumper << ", '" << theAuthor << "'";
    if (! aGeometryName.empty())
      theDumper << ", '" << aGeometryName << "'";
    theDumper << ")" << std::endl;
    return;
  }

  theDumper << aBase << " = model.";

  if (exportType == "XAO") {
    std::string aTmpXAOFile =
      aBase->string(ExchangePlugin_ExportFeature::XAO_FILE_PATH_ID())->value();
    correctSeparators(aTmpXAOFile);
    theDumper << "exportToXAO(" << aDocName << ", '" << aTmpXAOFile << "'" ;
    AttributeSelectionListPtr aShapeSelected =
      aBase->selectionList(ExchangePlugin_ExportFeature::XAO_SELECTION_LIST_ID());
    if (aShapeSelected->isInitialized() && aShapeSelected->size() == 1) {
      theDumper<<", "<<aShapeSelected->value(0);
    }

    std::string theAuthor = aBase->string(ExchangePlugin_ExportFeature::XAO_AUTHOR_ID())->value();
    if (! theAuthor.empty())
      theDumper << ", '" << theAuthor << "'";
    std::string theGeometryName =
      aBase->string(ExchangePlugin_ExportFeature::XAO_GEOMETRY_NAME_ID())->value();
    if (! theGeometryName.empty())
      theDumper << ", '" << theGeometryName << "'";
    theDumper << ")" << std::endl;
  }
  else if (exportType == "STL") {
    std::string aTmpSTLFile =
                aBase->string(ExchangePlugin_ExportFeature::STL_FILE_PATH_ID())->value();
    correctSeparators(aTmpSTLFile);
    theDumper << "exportToSTL(" << aDocName << ", '" << aTmpSTLFile << "'" ;
    AttributeSelectionPtr aShapeSelected =
      aBase->selection(ExchangePlugin_ExportFeature::STL_OBJECT_SELECTED());

    theDumper<<","<< aShapeSelected;

    theDumper <<","<<  stlabsolute() <<","<< stlrelative();

    if (stldeflectionType()->value()
         == ExchangePlugin_ExportFeature::STL_DEFLECTION_TYPE_RELATIVE()){
      theDumper <<","<< "True";
    }
    else {
      theDumper <<","<< "False";
    }

    if (stlfileType()->value() == ExchangePlugin_ExportFeature::STL_FILE_TYPE_BINARY()) {
      theDumper << "False";
    }
    else {
      theDumper <<  "True";
    }
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

ExportPtr exportToSTL(const std::shared_ptr<ModelAPI_Document> & thePart,
      const std::string & theFilePath,
      const ModelHighAPI_Selection& theSelectedShape,
      const ModelHighAPI_Double&  theDeflectionRelative,
      const ModelHighAPI_Double&  theDeflectionAbsolute,
      const bool theIsRelative,
      const bool theIsASCII)
{
  apply(); // finish previous operation to make sure all previous operations are done
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(ExchangePlugin_ExportFeature::ID());

  return ExportPtr(new ExchangeAPI_Export(aFeature,
                                          theFilePath,
                                          theSelectedShape,
                                          theDeflectionRelative,
                                          theDeflectionAbsolute,
                                          theIsRelative,
                                          theIsASCII));
}

ExportPtr exportToXAO(const std::shared_ptr<ModelAPI_Document> & thePart,
                      const std::string & theFilePath,
                      const ModelHighAPI_Selection& theSelectedShape,
                      const std::string & /*theAuthor*/,
                      const std::string & /*theGeometryName*/)
{
  apply(); // finish previous operation to make sure all previous operations are done
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(ExchangePlugin_ExportFeature::ID());
  // special internal case when for XAO a selection list is filled
  return ExportPtr(new ExchangeAPI_Export(aFeature, theFilePath, theSelectedShape, "XAO"));
}

PyObject* exportToXAOMem(const std::shared_ptr<ModelAPI_Document> & thePart,
                           const ModelHighAPI_Selection& theSelectedShape,
                           const std::string & theAuthor,
                           const std::string & theGeometryName)
{
  apply(); // finish previous operation to make sure all previous operations are done
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(ExchangePlugin_ExportFeature::ID());
  ExportPtr aXAOExportAPI (new ExchangeAPI_Export
                           (aFeature, theSelectedShape, theAuthor, theGeometryName));
  std::string aBuff = aFeature->string(ExchangePlugin_ExportFeature::MEMORY_BUFFER_ID())->value();
  return PyBytes_FromString(aBuff.c_str());
}

void exportPart(const std::shared_ptr<ModelAPI_Document> & thePart,
                const std::string & theFilePath,
                const std::list<ModelHighAPI_Selection> & theSelected)
{
  FeaturePtr aFeature = thePart->addFeature(ExchangePlugin_ExportPart::ID());
  aFeature->string(ExchangePlugin_ExportPart::FILE_PATH_ID())->setValue(theFilePath);
  if (!theSelected.empty()) {
    fillAttribute(theSelected,
        aFeature->selectionList(ExchangePlugin_ExportPart::SELECTION_LIST_ID()));
  }
  // restart transaction to execute and delete the macro-feature
  apply();
}
//--------------------------------------------------------------------------------------
