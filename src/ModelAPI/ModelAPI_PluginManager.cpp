// File:        ModelAPI_PluginManager.hxx
// Created:     20 Mar 2014
// Author:      Mikhail PONIKAROV

#include <ModelAPI_PluginManager.h>
// to avoid unresolved ModelAPI_Document()
#include <ModelAPI_Document.h>
// to avoid unresolved ModelAPI_Feature()
#include <ModelAPI_Feature.h>
#include <ModelAPI_Object.h>
// to avoid unresolved ModelAPI_Data()
#include <ModelAPI_Data.h>
// to avoid unresolved ModelAPI_Plugin()
#include <ModelAPI_Plugin.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Validator.h>

#include <Config_ModuleReader.h>

#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

using namespace std;

#ifdef _DEBUG
#include <iostream>
#endif

/// Manager that will be initialized from Model package, one per application
boost::shared_ptr<ModelAPI_PluginManager> MY_MANAGER;

ModelAPI_PluginManager::ModelAPI_PluginManager()
{
}

void ModelAPI_PluginManager::setPluginManager(boost::shared_ptr<ModelAPI_PluginManager> theManager)
{
  MY_MANAGER = theManager;
}

boost::shared_ptr<ModelAPI_PluginManager> ModelAPI_PluginManager::get()
{
  if (!MY_MANAGER) {  // import Model library that implements this interface of ModelAPI
    Config_ModuleReader::loadLibrary("Model");
  }
  return MY_MANAGER;
}

const ModelAPI_EventCreator* MY_API_CREATOR = 0;  ///< instance of the events creator, one pre application

const ModelAPI_EventCreator* ModelAPI_EventCreator::get()
{
  if (!MY_API_CREATOR) {  // import Model library that implements this interface of ModelAPI
    Config_ModuleReader::loadLibrary("Model");
  }
  return MY_API_CREATOR;
}

void ModelAPI_EventCreator::set(const ModelAPI_EventCreator* theCreator)
{
  MY_API_CREATOR = theCreator;
}
