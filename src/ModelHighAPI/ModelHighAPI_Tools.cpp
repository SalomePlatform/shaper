// Copyright (C) 2014-20xx CEA/DEN, EDF R&D
// Name   : ModelHighAPI_Tools.cpp
// Purpose:
//
// History:
// 07/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "ModelHighAPI_Tools.h"
#include <ModelHighAPI_FeatureStore.h>
//--------------------------------------------------------------------------------------
#include <GeomAPI_Dir.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Pnt2d.h>
//--------------------------------------------------------------------------------------
#include <GeomDataAPI_Dir.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>
//--------------------------------------------------------------------------------------
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefAttrList.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeStringArray.h>
#include <ModelAPI_AttributeDoubleArray.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_ResultPart.h>
//--------------------------------------------------------------------------------------
#include <Config_ModuleReader.h>
//--------------------------------------------------------------------------------------
#include "ModelHighAPI_Double.h"
#include "ModelHighAPI_Integer.h"
#include "ModelHighAPI_RefAttr.h"
#include "ModelHighAPI_Reference.h"
#include "ModelHighAPI_Selection.h"

#include <Events_InfoMessage.h>

// Have to be included before std headers
#include <Python.h>

#include <algorithm>
#include <iostream>

//--------------------------------------------------------------------------------------
void fillAttribute(const std::shared_ptr<GeomAPI_Pnt2d> & theValue,
                   const std::shared_ptr<GeomDataAPI_Point2D> & theAttribute)
{
  theAttribute->setValue(theValue);
}

void fillAttribute(const std::shared_ptr<GeomDataAPI_Point2D> & theAttribute,
                   double theX, double theY)
{
  theAttribute->setValue(theX, theY);
}

//--------------------------------------------------------------------------------------
void fillAttribute(const std::shared_ptr<GeomAPI_Dir> & theValue,
                   const std::shared_ptr<GeomDataAPI_Dir> & theAttribute)
{
  theAttribute->setValue(theValue);
}

//--------------------------------------------------------------------------------------
void fillAttribute(const std::shared_ptr<GeomAPI_Pnt> & theValue,
                   const std::shared_ptr<GeomDataAPI_Point> & theAttribute)
{
  theAttribute->setValue(theValue);
}

//--------------------------------------------------------------------------------------
void fillAttribute(bool theValue,
                   const std::shared_ptr<ModelAPI_AttributeBoolean> & theAttribute)
{
  theAttribute->setValue(theValue);
}

//--------------------------------------------------------------------------------------
void fillAttribute(const ModelHighAPI_Double & theValue,
                   const std::shared_ptr<ModelAPI_AttributeDouble> & theAttribute)
{
  theValue.fillAttribute(theAttribute);
}
void fillAttribute(double theValue,
                   const std::shared_ptr<ModelAPI_AttributeDouble> & theAttribute)
{
  theAttribute->setValue(theValue);
}

//--------------------------------------------------------------------------------------
void fillAttribute(const ModelHighAPI_Integer & theValue,
                   const std::shared_ptr<ModelAPI_AttributeInteger> & theAttribute)
{
  theValue.fillAttribute(theAttribute);
}
void fillAttribute(int theValue,
                   const std::shared_ptr<ModelAPI_AttributeInteger> & theAttribute)
{
  theAttribute->setValue(theValue);
}

//--------------------------------------------------------------------------------------
void fillAttribute(const ModelHighAPI_RefAttr & theValue,
                   const std::shared_ptr<ModelAPI_AttributeRefAttr> & theAttribute)
{
  theValue.fillAttribute(theAttribute);
}

//--------------------------------------------------------------------------------------
void fillAttribute(const std::list<ModelHighAPI_RefAttr> & theValue,
                   const std::shared_ptr<ModelAPI_AttributeRefAttrList> & theAttribute)
{
  theAttribute->clear();
  for (auto it = theValue.begin(); it != theValue.end(); ++it)
    it->appendToList(theAttribute);
}

//--------------------------------------------------------------------------------------
void fillAttribute(const ModelHighAPI_Reference & theValue,
                   const std::shared_ptr<ModelAPI_AttributeReference> & theAttribute)
{
  theValue.fillAttribute(theAttribute);
}

//--------------------------------------------------------------------------------------
void fillAttribute(const std::list<ModelHighAPI_Reference> & theValue,
                   const std::shared_ptr<ModelAPI_AttributeRefList> & theAttribute)
{
  theAttribute->clear();
  for (auto it = theValue.begin(); it != theValue.end(); ++it)
    it->appendToList(theAttribute);
}

//--------------------------------------------------------------------------------------
void fillAttribute(const std::shared_ptr<ModelAPI_Object> & theValue,
                   const std::shared_ptr<ModelAPI_AttributeReference> & theAttribute)
{
  theAttribute->setValue(theValue);
}

//--------------------------------------------------------------------------------------
void fillAttribute(const std::list<std::shared_ptr<ModelAPI_Object> > & theValue,
                   const std::shared_ptr<ModelAPI_AttributeRefList> & theAttribute)
{
  theAttribute->clear();
  for (auto it = theValue.begin(); it != theValue.end(); ++it)
    theAttribute->append(*it);
}

MODELHIGHAPI_EXPORT
void fillAttribute(const std::list<ModelHighAPI_Selection> & theValue,
                   const std::shared_ptr<ModelAPI_AttributeRefList> & theAttribute)
{
  theAttribute->clear();
  for (auto it = theValue.begin(); it != theValue.end(); ++it)
    theAttribute->append(it->resultSubShapePair().first); // use only context
}

//--------------------------------------------------------------------------------------
void fillAttribute(const ModelHighAPI_Selection & theValue,
                   const std::shared_ptr<ModelAPI_AttributeSelection> & theAttribute)
{
  theValue.fillAttribute(theAttribute);
}

//--------------------------------------------------------------------------------------
void fillAttribute(const std::list<ModelHighAPI_Selection> & theValue,
                   const std::shared_ptr<ModelAPI_AttributeSelectionList> & theAttribute)
{
  theAttribute->clear();

  if(!theValue.empty()) {
    std::string aSelectionType;
    const ModelHighAPI_Selection& aSelection = theValue.front();
    theAttribute->setSelectionType(aSelection.shapeType());
  }

  for (auto it = theValue.begin(); it != theValue.end(); ++it)
    it->appendToList(theAttribute);
}

//--------------------------------------------------------------------------------------
void fillAttribute(const std::string & theValue,
                   const std::shared_ptr<ModelAPI_AttributeString> & theAttribute)
{
  theAttribute->setValue(theValue);
}

//--------------------------------------------------------------------------------------
void fillAttribute(const char * theValue,
                   const std::shared_ptr<ModelAPI_AttributeString> & theAttribute)
{
  theAttribute->setValue(theValue);
}

//--------------------------------------------------------------------------------------
void fillAttribute(const std::list<std::string> & theValue,
                   const std::shared_ptr<ModelAPI_AttributeStringArray> & theAttribute)
{
  theAttribute->setSize(int(theValue.size()));

  int anIndex = 0;
  for (auto it = theValue.begin(); it != theValue.end(); ++it, ++anIndex)
    theAttribute->setValue(anIndex, *it);
}

//--------------------------------------------------------------------------------------
void fillAttribute(const std::list<ModelHighAPI_Integer> & theValue,
                   const std::shared_ptr<ModelAPI_AttributeIntArray> & theAttribute)
{
  theAttribute->setSize(int(theValue.size()));

  int anIndex = 0;
  for (auto it = theValue.begin(); it != theValue.end(); ++it, ++anIndex)
    theAttribute->setValue(anIndex, it->intValue()); // use only values, no text support in array
}

//==================================================================================================
GeomAPI_Shape::ShapeType shapeTypeByStr(std::string theShapeTypeStr)
{
  GeomAPI_Shape::ShapeType aShapeType = GeomAPI_Shape::SHAPE;

  std::transform(theShapeTypeStr.begin(), theShapeTypeStr.end(),
                 theShapeTypeStr.begin(), ::tolower);

  if(theShapeTypeStr == "compound") {
    aShapeType = GeomAPI_Shape::COMPOUND;
  } else if(theShapeTypeStr == "compsolid") {
    aShapeType = GeomAPI_Shape::COMPSOLID;
  } else if(theShapeTypeStr == "solid") {
    aShapeType = GeomAPI_Shape::SOLID;
  } else if(theShapeTypeStr == "shell") {
    aShapeType = GeomAPI_Shape::SHELL;
  } else if(theShapeTypeStr == "face") {
    aShapeType = GeomAPI_Shape::FACE;
  } else if(theShapeTypeStr == "wire") {
    aShapeType = GeomAPI_Shape::WIRE;
  } else if(theShapeTypeStr == "edge") {
    aShapeType = GeomAPI_Shape::EDGE;
  } else if(theShapeTypeStr == "vertex") {
    aShapeType = GeomAPI_Shape::VERTEX;
  } else if(theShapeTypeStr == "shape") {
    aShapeType = GeomAPI_Shape::SHAPE;
  }

  return aShapeType;
}

//==================================================================================================
GeomAPI_Shape::ShapeType getShapeType(const ModelHighAPI_Selection& theSelection)
{
  GeomAPI_Shape::ShapeType aShapeType = GeomAPI_Shape::SHAPE;

  switch(theSelection.variantType()) {
    case ModelHighAPI_Selection::VT_ResultSubShapePair: {
      ResultSubShapePair aPair = theSelection.resultSubShapePair();
      GeomShapePtr aShape = aPair.second;
      if(!aShape.get()) {
        aShape = aPair.first->shape();
      }
      if(!aShape.get()) {
        return aShapeType;
      }
      aShapeType = aShape->shapeType();
      break;
    }
    case ModelHighAPI_Selection::VT_TypeSubShapeNamePair: {
      TypeSubShapeNamePair aPair = theSelection.typeSubShapeNamePair();
      std::string aType = aPair.first;
      aShapeType = shapeTypeByStr(aType);
      break;
    }
  }

  return aShapeType;
}

//--------------------------------------------------------------------------------------
ModelAPI_AttributeTables::ValueType valueTypeByStr(const std::string& theValueTypeStr)
{
  std::string aType = theValueTypeStr;
  std::transform(aType.begin(), aType.end(), aType.begin(), ::tolower);
  if (aType == "boolean")
    return ModelAPI_AttributeTables::BOOLEAN;
  else if (aType == "integer")
    return ModelAPI_AttributeTables::INTEGER;
  else if (aType == "string")
    return ModelAPI_AttributeTables::STRING;
  return ModelAPI_AttributeTables::DOUBLE; // default
}

//--------------------------------------------------------------------------------------
std::string strByValueType(const ModelAPI_AttributeTables::ValueType theType)
{
  switch(theType) {
  case ModelAPI_AttributeTables::BOOLEAN: return "BOOLEAN";
  case ModelAPI_AttributeTables::INTEGER: return "INTEGER";
  case ModelAPI_AttributeTables::DOUBLE: return "DOUBLE";
  case ModelAPI_AttributeTables::STRING: return "STRING";
  }
  return ""; // bad case
}

/// stores the features information, recoursively stores sub-documetns features
std::string storeFeatures(const std::string& theDocName, DocumentPtr theDoc,
  std::map<std::string, std::map<std::string, ModelHighAPI_FeatureStore> >& theStore,
  const bool theCompare) // if false => store
{
  std::map<std::string, std::map<std::string, ModelHighAPI_FeatureStore> >::iterator aDocFind;
  if (theCompare) {
     aDocFind = theStore.find(theDocName);
     if (aDocFind == theStore.end()) {
       return "Document '" + theDocName + "' not found";
     }
  }
  // store the model features information: iterate all features
  int aFeaturesCount = 0; // stores the number of compared features for this document to compate
  std::set<std::string> aProcessed; // processed features names (that are in the current document)
  std::list<FeaturePtr> allFeatures = theDoc->allFeatures();
  std::list<FeaturePtr>::iterator allIter = allFeatures.begin();
  for(; allIter != allFeatures.end(); allIter++) {
    FeaturePtr aFeat = *allIter;
    if (theCompare) {
      std::map<std::string, ModelHighAPI_FeatureStore>::iterator
        aFeatFind = aDocFind->second.find(aFeat->name());
      if (aFeatFind == aDocFind->second.end()) {
        return "Document '" + theDocName + "' feature '" + aFeat->name() + "' not found";
      }
      std::string anError = aFeatFind->second.compare(aFeat);
      if (!anError.empty()) {
        return anError;
      }
      aFeaturesCount++;
      aProcessed.insert(aFeat->name());
    } else {
      theStore[theDocName][aFeat->name()] = ModelHighAPI_FeatureStore(aFeat);
    }
    // iterate all results of this feature
    std::list<ResultPtr> allResults;
    ModelAPI_Tools::allResults(aFeat, allResults);
    std::list<ResultPtr>::iterator aRes = allResults.begin();
    for(; aRes != allResults.end(); aRes++) {
      // recoursively store features of sub-documents
      if ((*aRes)->groupName() == ModelAPI_ResultPart::group()) {
        DocumentPtr aDoc = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aRes)->partDoc();
        if (aDoc.get()) {
          std::string anError = storeFeatures((*aRes)->data()->name(), aDoc, theStore, theCompare);
          if (!anError.empty())
            return anError;
        }
      }
    }
  }
  // checks the number of compared features
  if (theCompare) {
    if (aDocFind->second.size() != aFeaturesCount) {
      // search for disappeared feature
      std::string aLostName;
      std::map<std::string, ModelHighAPI_FeatureStore>::iterator aLostIter;
      for(aLostIter = aDocFind->second.begin(); aLostIter != aDocFind->second.end(); aLostIter++) {
        if (aProcessed.find(aLostIter->first) == aProcessed.end()) {
          aLostName = aLostIter->first;
        }
      }
      return "For document '" + theDocName +
        "' the number of features is decreased, there is no feature '" + aLostName + "'";
    }
  }
  return ""; // ok
}

//==================================================================================================
bool checkPythonDump()
{
  SessionPtr aSession = ModelAPI_Session::get();
  // dump all to the python file
  aSession->startOperation("Check python dump");
  FeaturePtr aDump = aSession->moduleDocument()->addFeature("Dump");
  if (aDump.get()) {
    aDump->string("file_path")->setValue("check_dump.py"); // to the current folder
    aDump->string("file_format")->setValue("py"); // to the current folder
    aDump->execute();
  }
  bool isProblem = !aDump.get() || !aDump->error().empty(); // after "finish" dump will be removed
  if (isProblem && aDump.get()) {
    std::cout<<"Dump feature error "<<aDump->error()<<std::endl;
    Events_InfoMessage anErrorMsg(std::string("checkPythonDump"), aDump->error());
    anErrorMsg.send();
  }
  aSession->finishOperation();
  if (isProblem) {
    return false; // something is wrong during dump
  }

   // map from document name to feature name to feature data
  std::map<std::string, std::map<std::string, ModelHighAPI_FeatureStore> > aStore;
  std::string anError = storeFeatures(
    aSession->moduleDocument()->kind(), aSession->moduleDocument(), aStore, false);
  if (!anError.empty()) {
    Events_InfoMessage anErrorMsg(std::string("checkPythonDump"), anError);
    anErrorMsg.send();
    return false;
  }
  // close all before importation of the script
  aSession->closeAll();
  // execute the dumped
  PyGILState_STATE gstate = PyGILState_Ensure(); /* acquire python thread */
  PyObject* PyFileObject = PyFile_FromString("./check_dump.py", "r");
  PyRun_SimpleFileEx(PyFile_AsFile(PyFileObject), "./check_dump.py", 1);
  PyGILState_Release(gstate); /* release python thread */

  // compare with the stored data
  anError = storeFeatures(
    aSession->moduleDocument()->kind(), aSession->moduleDocument(), aStore, true);
  if (!anError.empty()) {
    std::cout<<anError<<std::endl;
    Events_InfoMessage anErrorMsg(std::string("checkPythonDump"), anError);
    anErrorMsg.send();
    return false;
  }

  return true;
}

//--------------------------------------------------------------------------------------
