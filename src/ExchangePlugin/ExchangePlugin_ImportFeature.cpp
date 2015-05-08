// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ExchangePlugin_ImportFeature.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: sbh
 */

#include <ExchangePlugin_ImportFeature.h>
#include <GeomAlgoAPI_BREPImport.h>
#include <GeomAlgoAPI_STEPImport.h>
#include <GeomAlgoAPI_IGESImport.h>

#include <GeomAPI_Shape.h>
#include <Config_Common.h>
#include <Config_PropManager.h>

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
  AttributeStringPtr aFilePathAttr = std::dynamic_pointer_cast<ModelAPI_AttributeString>(
      data()->attribute(ExchangePlugin_ImportFeature::FILE_PATH_ID()));
  std::string aFilePath = aFilePathAttr->value();
  if(aFilePath.empty())
    return;
  importFile(aFilePath);
}

bool ExchangePlugin_ImportFeature::importFile(const std::string& theFileName)
{
  // retrieve the file and plugin library names
  TCollection_AsciiString aFileName (theFileName.c_str());
  OSD_Path aPath(aFileName);
  TCollection_AsciiString aFormatName = aPath.Extension();
  // ".brep" -> "BREP", TCollection_AsciiString are numbered from 1
  aFormatName = aFormatName.SubString(2, aFormatName.Length());
  aFormatName.UpperCase();

  // Perform the import
  TCollection_AsciiString anError;
  TDF_Label anUnknownLabel = TDF_Label();

  TopoDS_Shape aShape;
  if (aFormatName == "BREP") {
    aShape = BREPImport::Import(aFileName, aFormatName, anError, anUnknownLabel);
  } else if (aFormatName == "STEP" || aFormatName == "STP") {
    aShape = STEPImport::Import(aFileName, aFormatName, anError, anUnknownLabel);
  } else if (aFormatName == "IGES") {
    aShape = IGESImport::Import(aFileName, aFormatName, anError, anUnknownLabel);
  }
   // Check if shape is valid
  if ( aShape.IsNull() ) {
     const static std::string aShapeError = 
       "An error occurred while importing " + theFileName + ": " + anError.ToCString();
     setError(aShapeError);
     return false;
   }

  // Pass the results into the model
  std::string anObjectName = aPath.Name().ToCString();
  data()->setName(anObjectName);
  std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data());
  std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
  aGeomShape->setImpl(new TopoDS_Shape(aShape));

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
  theResultBody->store(theGeomShape);
 
  int aTag(1);
  std::string aNameMS = "Shape";
  theResultBody->loadFirstLevel(theGeomShape, aNameMS, aTag);
  //std::string aNameDE = "DiscEdges";
  //theResultBody->loadDisconnectedEdges(theGeomShape, aNameDE, aTag);
  //std::string aNameDV = "DiscVertexes";
  //theResultBody->loadDisconnectedVertexes(theGeomShape, aNameDV, aTag); 
}
