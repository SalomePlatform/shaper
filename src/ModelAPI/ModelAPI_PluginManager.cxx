// File:        ModelAPI_PluginManager.hxx
// Created:     20 Mar 2014
// Author:      Mikhail PONIKAROV

#include <ModelAPI_PluginManager.hxx>
// to avoid unresolved ModelAPI_Document()
#include <ModelAPI_Document.hxx>
// to avoid unresolved ModelAPI_Feature()
#include <ModelAPI_Feature.hxx>

/// Manager that will be initialized from Model package, one per application
boost::shared_ptr<ModelAPI_PluginManager> MY_MANAGER;

ModelAPI_PluginManager::ModelAPI_PluginManager() 
{}

boost::shared_ptr<ModelAPI_PluginManager> ModelAPI_PluginManager::Get()
{
  return MY_MANAGER;
}

void ModelAPI_PluginManager::SetPluginManager(
  boost::shared_ptr<ModelAPI_PluginManager> theManager)
{
  MY_MANAGER = theManager;
}
