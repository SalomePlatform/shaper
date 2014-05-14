// File:        Model_PluginManager.cxx
// Created:     20 Mar 2014
// Author:      Mikhail PONIKAROV

#include <Model_PluginManager.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Plugin.h>
#include <Model_Data.h>
#include <Model_Document.h>
#include <Model_Application.h>
#include <Events_Loop.h>
#include <Config_FeatureMessage.h>
#include <Config_ModuleReader.h>

using namespace std;

static Model_PluginManager* myImpl = new Model_PluginManager();

boost::shared_ptr<ModelAPI_Feature> Model_PluginManager::createFeature(string theFeatureID)
{
  if (this != myImpl) return myImpl->createFeature(theFeatureID);

  LoadPluginsInfo();
  if (myPlugins.find(theFeatureID) != myPlugins.end()) {
    myCurrentPluginName = myPlugins[theFeatureID];
    if (myPluginObjs.find(myCurrentPluginName) == myPluginObjs.end()) {
      // load plugin library if not yet done
      Config_ModuleReader::loadLibrary(myCurrentPluginName);
    }
    if (myPluginObjs.find(myCurrentPluginName) != myPluginObjs.end()) {
      boost::shared_ptr<ModelAPI_Feature> aCreated = 
        myPluginObjs[myCurrentPluginName]->createFeature(theFeatureID);
      return aCreated;
    }
  }

  return boost::shared_ptr<ModelAPI_Feature>(); // return nothing
}

boost::shared_ptr<ModelAPI_Document> Model_PluginManager::rootDocument()
{
  return boost::shared_ptr<ModelAPI_Document>(
    Model_Application::getApplication()->getDocument("root"));
}

bool Model_PluginManager::hasRootDocument()
{
  return Model_Application::getApplication()->hasDocument("root");
}

boost::shared_ptr<ModelAPI_Document> Model_PluginManager::currentDocument()
{
  if (!myCurrentDoc)
    myCurrentDoc = rootDocument();
  return myCurrentDoc;
}

void Model_PluginManager::setCurrentDocument(boost::shared_ptr<ModelAPI_Document> theDoc)
{
  myCurrentDoc = theDoc;
}

Model_PluginManager::Model_PluginManager()
{
  myPluginsInfoLoaded = false;
  //TODO(sbh): Implement static method to extract event id [SEID]
  static Events_ID aFeatureEvent = Events_Loop::eventByName("FeatureRegisterEvent");

  ModelAPI_PluginManager::SetPluginManager(boost::shared_ptr<ModelAPI_PluginManager>(this));
  // register the configuration reading listener
  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->registerListener(this, aFeatureEvent);
}

void Model_PluginManager::processEvent(const Events_Message* theMessage)
{
  const Config_FeatureMessage* aMsg =
    dynamic_cast<const Config_FeatureMessage*>(theMessage);
  if (aMsg) {
    // proccess the plugin info, load plugin
    if (myPlugins.find(aMsg->id()) == myPlugins.end()) {
      myPlugins[aMsg->id()] = aMsg->pluginLibrary();
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
  Config_ModuleReader aXMLReader("FeatureRegisterEvent");
  aXMLReader.readAll();
}

void Model_PluginManager::registerPlugin(ModelAPI_Plugin* thePlugin)
{
  myPluginObjs[myCurrentPluginName] = thePlugin;
}
