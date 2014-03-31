// File:        Model_PluginManager.cxx
// Created:     20 Mar 2014
// Author:      Mikhail PONIKAROV

#include <Model_PluginManager.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Plugin.h>
#include <Model_Feature.h>
#include <Event_Loop.h>
#include <Config_FeatureMessage.h>
#include <Config_ModuleReader.h>

using namespace std;

static Model_PluginManager* myImpl = new Model_PluginManager();

boost::shared_ptr<ModelAPI_Feature> Model_PluginManager::createFeature(string theFeatureID)
{
  if (this != myImpl) return myImpl->createFeature(theFeatureID);

  LoadPluginsInfo();
  if (myPlugins.find(theFeatureID) != myPlugins.end()) {
    if (myPluginObjs.find(myPlugins[theFeatureID]) == myPluginObjs.end()) {
      // load plugin library if not yet done
      myCurrentPluginName = myPlugins[theFeatureID];
      loadLibrary(myCurrentPluginName);
    }
    if (myPluginObjs.find(myCurrentPluginName) != myPluginObjs.end()) {
      return myPluginObjs[myCurrentPluginName]->createFeature(theFeatureID);
    }
  }

  return boost::shared_ptr<ModelAPI_Feature>(); // return nothing
}

Model_PluginManager::Model_PluginManager()
{
  myPluginsInfoLoaded = false;
  static Event_ID aFeatureEvent = Event_Loop::eventByName("RegisterFeature");

  ModelAPI_PluginManager::SetPluginManager(boost::shared_ptr<ModelAPI_PluginManager>(this));
  // register the configuration reading listener
  Event_Loop* aLoop = Event_Loop::loop();
  aLoop->registerListener(this, aFeatureEvent);
}

void Model_PluginManager::processEvent(const Event_Message* theMessage)
{
  const Config_FeatureMessage* aMsg =
    dynamic_cast<const Config_FeatureMessage*>(theMessage);
  if (aMsg) {
    // proccess the plugin info, load plugin
    if (myPlugins.find(aMsg->id()) == myPlugins.end()) {
      myPlugins[aMsg->id()] = aMsg->pluginLibrary(); // TO DO: plugin name must be also imported from XMLs
    }
  }
  // plugins information was started to load, so, it will be loaded
  myPluginsInfoLoaded = true;
}

void Model_PluginManager::LoadPluginsInfo()
{
  if (myPluginsInfoLoaded) // nothing to do
    return;

  // Read plugins information from XML files
  Config_ModuleReader aXMLReader;
  aXMLReader.setAutoImport(true);
  aXMLReader.readAll();
}

void Model_PluginManager::registerPlugin(ModelAPI_Plugin* thePlugin)
{
  myPluginObjs[myCurrentPluginName] = thePlugin;
}
