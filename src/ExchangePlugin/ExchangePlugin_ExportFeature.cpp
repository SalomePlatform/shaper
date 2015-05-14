// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ExchangePlugin_ExportFeature.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: sbh
 */

#include <ExchangePlugin_ExportFeature.h>
//#include <GeomAlgoAPI_BREPExport.h>
//#include <GeomAlgoAPI_STEPExport.h>
#include <GeomAlgoAPI_IGESExport.h>

#include <Config_Common.h>
#include <Config_PropManager.h>

#include <GeomAlgoAPI_CompoundBuilder.h>

#include <GeomAPI_Shape.h>

#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_ResultBody.h>
#include <TCollection_AsciiString.hxx>
#include <TDF_Label.hxx>
#include <TopoDS_Shape.hxx>
#include <OSD_Path.hxx>

#include <algorithm>
#include <string>
#ifdef _DEBUG
#include <iostream>
#include <ostream>
#endif

ExchangePlugin_ExportFeature::ExchangePlugin_ExportFeature()
{
}

ExchangePlugin_ExportFeature::~ExchangePlugin_ExportFeature()
{
  // TODO Auto-generated destructor stub
}

/*
 * Returns the unique kind of a feature
 */
const std::string& ExchangePlugin_ExportFeature::getKind()
{
  return ExchangePlugin_ExportFeature::ID();
}

/*
 * Request for initialization of data model of the feature: adding all attributes
 */
void ExchangePlugin_ExportFeature::initAttributes()
{
  data()->addAttribute(ExchangePlugin_ExportFeature::FILE_PATH_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(ExchangePlugin_ExportFeature::SELECTION_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
}

/*
 * Computes or recomputes the results
 */
void ExchangePlugin_ExportFeature::execute()
{
  AttributeStringPtr aFilePathAttr = std::dynamic_pointer_cast<ModelAPI_AttributeString>(
      data()->attribute(ExchangePlugin_ExportFeature::FILE_PATH_ID()));
  std::string aFilePath = aFilePathAttr->value();
  if (aFilePath.empty())
    return;

  AttributeSelectionListPtr aSelectionListAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(
          data()->attribute(ExchangePlugin_ExportFeature::SELECTION_LIST_ID()));

  std::list<std::shared_ptr<GeomAPI_Shape> > aShapes;
  for ( int i = 0, aSize = aSelectionListAttr->size(); i < aSize; ++i ) {
    std::shared_ptr<ModelAPI_AttributeSelection> anSelectionAttr = aSelectionListAttr->value(i);
    aShapes.push_back(anSelectionAttr->value());
  }
  std::shared_ptr<GeomAPI_Shape> aShape =
      GeomAlgoAPI_CompoundBuilder::compound(aShapes);

  exportFile(aFilePath, aShape);
}

bool ExchangePlugin_ExportFeature::exportFile(const std::string& theFileName,
                                              std::shared_ptr<GeomAPI_Shape> theShape)
{
  // retrieve the file and plugin library names
  TCollection_AsciiString aFileName(theFileName.c_str());
  OSD_Path aPath(aFileName);
  TCollection_AsciiString aFormatName = aPath.Extension();
  // ".brep" -> "BREP", TCollection_AsciiString are numbered from 1
  aFormatName = aFormatName.SubString(2, aFormatName.Length());
  aFormatName.UpperCase();

  // Perform the export
  TCollection_AsciiString anError;
  TDF_Label anUnknownLabel = TDF_Label();

  TopoDS_Shape aShape(theShape->impl<TopoDS_Shape>());
  bool aResult = true;
  if (aFormatName == "BREP") {
//    aShape = BREPExport::Export(aFileName, aFormatName, anError, anUnknownLabel);
  } else if (aFormatName == "STEP" || aFormatName == "STP") {
//    aShape = STEPExport::Export(aFileName, aFormatName, anError, anUnknownLabel);
  } else if (aFormatName == "IGES") {
    aResult = IGESExport::Export(aFileName, aFormatName, aShape, anError, anUnknownLabel);
  }

  if ( !aResult ) {
    const static std::string aShapeError =
        "An error occurred while exporting " + theFileName + ": " + anError.ToCString();
    setError(aShapeError);
    return false;
  }

  return true;
}
