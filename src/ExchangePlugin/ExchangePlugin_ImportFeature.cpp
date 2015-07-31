// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    ExchangePlugin_ImportFeature.cpp
// Created: Aug 28, 2014
// Author:  Sergey BELASH

#include <ExchangePlugin_ImportFeature.h>

#include <GeomAlgoAPI_BREPImport.h>
#include <GeomAlgoAPI_IGESImport.h>
#include <GeomAlgoAPI_STEPImport.h>
#include <GeomAlgoAPI_Tools.h>

#include <GeomAPI_Shape.h>

#include <Config_Common.h>
#include <Config_PropManager.h>

#include <ModelAPI_AttributeString.h>
#include <ModelAPI_BodyBuilder.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_ResultBody.h>

#include <algorithm>
#include <string>
#ifdef _DEBUG
#include <iostream>
#include <ostream>
#endif

ExchangePlugin_ImportFeature::ExchangePlugin_ImportFeature()
{
}

ExchangePlugin_ImportFeature::~ExchangePlugin_ImportFeature()
{
  // TODO Auto-generated destructor stub
}

/*
 * Returns the unique kind of a feature
 */
const std::string& ExchangePlugin_ImportFeature::getKind()
{
  return ExchangePlugin_ImportFeature::ID();
}

/*
 * Request for initialization of data model of the feature: adding all attributes
 */
void ExchangePlugin_ImportFeature::initAttributes()
{
  data()->addAttribute(ExchangePlugin_ImportFeature::FILE_PATH_ID(), ModelAPI_AttributeString::typeId());
}

/*
 * Computes or recomputes the results
 */
void ExchangePlugin_ImportFeature::execute()
{
  AttributeStringPtr aFilePathAttr =
      this->string(ExchangePlugin_ImportFeature::FILE_PATH_ID());
  std::string aFilePath = aFilePathAttr->value();
  if (aFilePath.empty())
    return;

  importFile(aFilePath);
}

bool ExchangePlugin_ImportFeature::importFile(const std::string& theFileName)
{
  // retrieve the file and plugin library names
  // ".brep" -> "BREP"
  std::string anExtension = GeomAlgoAPI_Tools::File_Tools::extension(theFileName);

  // Perform the import
  std::string anError;

  std::shared_ptr<GeomAPI_Shape> aGeomShape;
  if (anExtension == "BREP" || anExtension == "BRP") {
    aGeomShape = BREPImport(theFileName, anExtension, anError);
  } else if (anExtension == "STEP" || anExtension == "STP") {
    aGeomShape = STEPImport(theFileName, anExtension, anError);
  } else if (anExtension == "IGES" || anExtension == "IGS") {
    aGeomShape = IGESImport(theFileName, anExtension, anError);
  }
   // Check if shape is valid
  if ( aGeomShape->isNull() ) {
    const static std::string aShapeError =
      "An error occurred while importing " + theFileName + ": " + anError;
    setError(aShapeError);
    return false;
  }

  // Pass the results into the model
  std::string anObjectName = GeomAlgoAPI_Tools::File_Tools::name(theFileName);
  data()->setName(anObjectName);
  std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data());

  //LoadNamingDS of the imported shape
  loadNamingDS(aGeomShape, aResultBody);

  setResult(aResultBody);

  return true;
}

//============================================================================
void ExchangePlugin_ImportFeature::loadNamingDS(
    std::shared_ptr<GeomAPI_Shape> theGeomShape,
    std::shared_ptr<ModelAPI_ResultBody> theResultBody)
{
  //load result
  theResultBody->getBodyBuilder()->store(theGeomShape);

  int aTag(1);
  std::string aNameMS = "Shape";
  theResultBody->getBodyBuilder()->loadFirstLevel(theGeomShape, aNameMS, aTag);
}
