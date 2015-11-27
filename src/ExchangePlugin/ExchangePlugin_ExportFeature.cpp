// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    ExchangePlugin_ExportFeature.cpp
// Created: May 14, 2015
// Author:  Sergey POKHODENKO

#include <ExchangePlugin_ExportFeature.h>

#include <algorithm>
#include <iterator>
#include <string>
#ifdef _DEBUG
#include <iostream>
#include <ostream>
#endif

#include <Config_Common.h>
#include <Config_PropManager.h>

#include <GeomAlgoAPI_BREPExport.h>
#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_IGESExport.h>
#include <GeomAlgoAPI_STEPExport.h>
#include <GeomAlgoAPI_Tools.h>
#include <GeomAlgoAPI_XAOExport.h>

#include <GeomAPI_Shape.h>

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <XAO_Xao.hxx>

#include <ExchangePlugin_Tools.h>

ExchangePlugin_ExportFeature::ExchangePlugin_ExportFeature()
{
}

ExchangePlugin_ExportFeature::~ExchangePlugin_ExportFeature()
{
  // TODO Auto-generated destructor stub
}

/*
 * Request for initialization of data model of the feature: adding all attributes
 */
void ExchangePlugin_ExportFeature::initAttributes()
{
  data()->addAttribute(ExchangePlugin_ExportFeature::EXPORT_TYPE_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(ExchangePlugin_ExportFeature::FILE_PATH_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(ExchangePlugin_ExportFeature::FILE_FORMAT_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(ExchangePlugin_ExportFeature::SELECTION_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(ExchangePlugin_ExportFeature::XAO_AUTHOR_ID(), ModelAPI_AttributeString::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), ExchangePlugin_ExportFeature::XAO_AUTHOR_ID());
}

/*
 * Computes or recomputes the results
 */
void ExchangePlugin_ExportFeature::execute()
{
  AttributeStringPtr aFormatAttr =
      this->string(ExchangePlugin_ExportFeature::FILE_FORMAT_ID());
  std::string aFormat = aFormatAttr->value();
  // Format may be empty. In this case look at extension.
//  if (aFormat.empty())
//    return;

  AttributeStringPtr aFilePathAttr =
      this->string(ExchangePlugin_ExportFeature::FILE_PATH_ID());
  std::string aFilePath = aFilePathAttr->value();
  if (aFilePath.empty())
    return;

  AttributeSelectionListPtr aSelectionListAttr =
      this->selectionList(ExchangePlugin_ExportFeature::SELECTION_LIST_ID());
  std::list<std::shared_ptr<GeomAPI_Shape> > aShapes;
  for (int i = 0, aSize = aSelectionListAttr->size(); i < aSize; ++i) {
    AttributeSelectionPtr anAttrSelection = aSelectionListAttr->value(i);
    std::shared_ptr<GeomAPI_Shape> aCurShape = anAttrSelection->value();
    if (aCurShape.get() == NULL)
      aCurShape = anAttrSelection->context()->shape();
    if (aCurShape.get() != NULL)
      aShapes.push_back(aCurShape);
  }

  // Store compound if we have more than one shape.
  std::shared_ptr<GeomAPI_Shape> aShape;
  if(aShapes.size() == 1) {
    aShape = aShapes.front();
  } else {
    aShape = GeomAlgoAPI_CompoundBuilder::compound(aShapes);
  }

  exportFile(aFilePath, aFormat, aShape);
}

void ExchangePlugin_ExportFeature::exportFile(const std::string& theFileName,
                                              const std::string& theFormat,
                                              std::shared_ptr<GeomAPI_Shape> theShape)
{
  std::string aFormatName = theFormat;

  if (aFormatName.empty()) { // get default format for the extension
    // ".brep" -> "BREP"
    std::string anExtension = GeomAlgoAPI_Tools::File_Tools::extension(theFileName);
    if (anExtension == "BREP" || anExtension == "BRP") {
      aFormatName = "BREP";
    } else if (anExtension == "STEP" || anExtension == "STP") {
      aFormatName = "STEP";
    } else if (anExtension == "IGES" || anExtension == "IGS") {
      aFormatName = "IGES-5.1";
    } else if (anExtension == "XAO") {
      aFormatName = "XAO";
    } else {
      aFormatName = anExtension;
    }
  }

  if (aFormatName == "XAO") {
    exportXAO(theFileName, theShape);
    return;
  }

  // Perform the export
  std::string anError;
  bool aResult = false;
  if (aFormatName == "BREP") {
    aResult = BREPExport(theFileName, aFormatName, theShape, anError);
  } else if (aFormatName == "STEP") {
    aResult = STEPExport(theFileName, aFormatName, theShape, anError);
  } else if (aFormatName.substr(0, 4) == "IGES") {
    aResult = IGESExport(theFileName, aFormatName, theShape, anError);
  } else {
    anError = "Unsupported format: " + aFormatName;
  }

  if (!anError.empty()) {
    setError("An error occurred while exporting " + theFileName + ": " + anError);
    return;
  }
}

void ExchangePlugin_ExportFeature::exportXAO(const std::string& theFileName,
                                             std::shared_ptr<GeomAPI_Shape> theShape)
{
  std::string anAuthor = string(ExchangePlugin_ExportFeature::XAO_AUTHOR_ID())->value();

  XAO::Xao aXao(anAuthor, "1.0");

  std::string anError;
  XAOExport(theFileName, theShape, &aXao, anError);

  if (!anError.empty()) {
    setError("An error occurred while exporting " + theFileName + ": " + anError);
    return;
  }
}

