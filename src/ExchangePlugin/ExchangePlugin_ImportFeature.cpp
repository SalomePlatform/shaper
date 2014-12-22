// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ExchangePlugin_ImportFeature.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: sbh
 */

#include <ExchangePlugin_ImportFeature.h>

#include <GeomAPI_Shape.h>
#include <Config_Common.h>
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

#ifdef WIN32
# define _separator_ '\\'
#else
# define _separator_ '/'
#endif

typedef TopoDS_Shape (*importFunctionPointer)(const TCollection_AsciiString&,
                                              const TCollection_AsciiString&,
                                              TCollection_AsciiString&,
                                              const TDF_Label&);

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
  data()->addAttribute(ExchangePlugin_ImportFeature::FILE_PATH_ID(), ModelAPI_AttributeString::type());
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
  // ".brep" -> "BREP". TCollection_AsciiString are numbered from 1
  aFormatName = aFormatName.SubString(2, aFormatName.Length());
  aFormatName.UpperCase();

  // Load plugin library and get the "Import" method
  LibHandle anImportLib = loadImportPlugin(std::string(aFormatName.ToCString()));
  if(!anImportLib)
    return false;
  importFunctionPointer fp = (importFunctionPointer) GetProc(anImportLib, "Import");
   // Perform the import
   TCollection_AsciiString anError;
   TDF_Label anUnknownLabel = TDF_Label();
   TopoDS_Shape aShape = fp(aFileName,
                            aFormatName,
                            anError,
                            anUnknownLabel);
   // Check if shape is valid
   if ( aShape.IsNull() ) {
     const static std::string aShapeError = 
       "An error occurred while importing " + theFileName + ": " + anError.ToCString();
     setError(aShapeError);
     return false;
   }
  //
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
  std::string aPrefix =  data()->name() + "/";
  int aTag(1);
  std::string aNameMS = aPrefix + "Shape";
  theResultBody->loadFirstLevel(theGeomShape, aNameMS, aTag);
  std::string aNameDE = aPrefix + "DiscEdges";
  theResultBody->loadDisconnectedEdges(theGeomShape, aNameDE, aTag);
  std::string aNameDV = aPrefix + "DiscVertexes";
  theResultBody->loadDisconnectedVertexes(theGeomShape, aNameDV, aTag); 
}

LibHandle ExchangePlugin_ImportFeature::loadImportPlugin(const std::string& theFormatName)
{
  std::string aLibName = library(theFormatName + ID());
  LibHandle anImportLib = LoadLib(aLibName.c_str());
  std::string anImportError = "Failed to load " + aLibName + ": ";
  if(!anImportLib) {
#ifdef WIN32
    LPVOID lpMsgBuf;
    ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    0, ::GetLastError(), 0, (LPTSTR) &lpMsgBuf, 0, 0);
    anImportError = anImportError + std::string((char*) lpMsgBuf);
    ::LocalFree(lpMsgBuf);
#else
    anImportError = anImportError + std::string(dlerror());
#endif
    setError(anImportError);
    return false;
  }
  // Test loaded plugin for existence of valid "Import" function:
  importFunctionPointer fp = (importFunctionPointer) GetProc(anImportLib, "Import");
  if (!fp) {
    const static std::string aFunctionError = 
      "No valid \"Import\" function was found in the " + aLibName;
    setError(aFunctionError);
    UnLoadLib(anImportLib)
    return NULL;
  }
  return anImportLib;
}
