// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ExchangePlugin_ExportFeature.cpp
 *
 *  Created on: May 14, 2015
 *      Author: spo
 */

#include <ExchangePlugin_ExportFeature.h>

#include <ExchangePlugin_Tools.h>

#include <GeomAlgoAPI_BREPExport.h>
#include <GeomAlgoAPI_STEPExport.h>
#include <GeomAlgoAPI_IGESExport.h>

#include <Config_Common.h>
#include <Config_PropManager.h>

#include <GeomAlgoAPI_CompoundBuilder.h>

#include <GeomAPI_Shape.h>

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_ResultBody.h>

#include <TCollection_AsciiString.hxx>
#include <TDF_Label.hxx>
#include <TopoDS_Shape.hxx>
#include <OSD_Path.hxx>

#include <algorithm>
#include <iterator>
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
  data()->addAttribute(ExchangePlugin_ExportFeature::FILE_FORMAT_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(ExchangePlugin_ExportFeature::FILE_PATH_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(ExchangePlugin_ExportFeature::SELECTION_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
}

/*
 * Computes or recomputes the results
 */
void ExchangePlugin_ExportFeature::execute()
{
  AttributeStringPtr aFormatAttr =
      this->string(ExchangePlugin_ExportFeature::FILE_FORMAT_ID());
  std::string aFormat = aFormatAttr->value();
  if (aFormat.empty())
    return;

  AttributeStringPtr aFilePathAttr =
      this->string(ExchangePlugin_ExportFeature::FILE_PATH_ID());
  std::string aFilePath = aFilePathAttr->value();
  if (aFilePath.empty())
    return;

  AttributeSelectionListPtr aSelectionListAttr =
      this->selectionList(ExchangePlugin_ExportFeature::SELECTION_LIST_ID());
  std::list<std::shared_ptr<GeomAPI_Shape> > aShapes;
  for (int i = 0, aSize = aSelectionListAttr->size(); i < aSize; ++i) {
    aShapes.push_back(aSelectionListAttr->value(i)->value());
  }
  std::shared_ptr<GeomAPI_Shape> aShape =
      GeomAlgoAPI_CompoundBuilder::compound(aShapes);

  exportFile(aFilePath, aFormat, aShape);
}

bool ExchangePlugin_ExportFeature::exportFile(const std::string& theFileName,
                                              const std::string& theFormat,
                                              std::shared_ptr<GeomAPI_Shape> theShape)
{
  // retrieve the file and plugin library names
  TCollection_AsciiString aFileName(theFileName.c_str());
  TCollection_AsciiString aFormatName(theFormat.c_str());

  // Perform the export
  TCollection_AsciiString anError;
  TopoDS_Shape aShape(theShape->impl<TopoDS_Shape>());
  bool aResult = false;
  if (aFormatName == "BREP") {
    aResult = BREPExport::Export(aFileName, aFormatName, aShape, anError);
  } else if (aFormatName == "STEP") {
    aResult = STEPExport::Export(aFileName, aFormatName, aShape, anError);
  } else if (aFormatName.SubString(1, 4) == "IGES") {
    aResult = IGESExport::Export(aFileName, aFormatName, aShape, anError);
  } else {
    anError = TCollection_AsciiString("Unsupported format ") + aFormatName;
  }

  if (!aResult) {
    std::string aShapeError =
        "An error occurred while exporting " + theFileName + ": " + anError.ToCString();
    setError(aShapeError);
    return false;
  }

  return true;
}
