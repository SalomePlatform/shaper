// File:        Model_PluginManager.cxx
// Created:     20 Mar 2014
// Author:      Mikhail PONIKAROV

#include <Model_PluginManager.h>
#include <ModelAPI_Feature.h>
#include <Model_Feature.h>
#include <Event_Loop.h>
#include <Config_FeatureMessage.h>
#include <Config_ModuleReader.h>

#include <OSD_SharedLibrary.hxx>

using namespace std;

static Model_PluginManager* myImpl = new Model_PluginManager();

boost::shared_ptr<ModelAPI_Feature> Model_PluginManager::CreateFeature(string theFeatureID)
{
  if (this != myImpl) return myImpl->CreateFeature(theFeatureID);

  LoadPluginsInfo();
  if (myPlugins.find(theFeatureID) != myPlugins.end()) {
    string aLibName = myPlugins[theFeatureID];
#ifdef WIN32
    aLibName += ".dll";
#else
    aLibName += ".so";
#endif
    Standard_CString aLibNameCStr = aLibName.c_str();
    OSD_SharedLibrary aLib(aLibNameCStr);
    if (aLib.DlOpen(OSD_RTLD_NOW)) {
      OSD_Function aFunc = aLib.DlSymb("CreateFeature");
    }
  }

  return boost::shared_ptr<ModelAPI_Feature>(); // return nothing
}

Model_PluginManager::Model_PluginManager()
{
  myPluginsInfoLoaded = false;
  static Event_ID aFeatureEvent = Event_Loop::EventByName("Feature");

  ModelAPI_PluginManager::SetPluginManager(boost::shared_ptr<ModelAPI_PluginManager>(this));
  // register the configuration reading listener
  Event_Loop* aLoop = Event_Loop::Loop();
  aLoop->RegisterListener(myImpl, aFeatureEvent);
}

void Model_PluginManager::ProcessEvent(const Event_Message* theMessage)
{
  const Config_FeatureMessage* aMsg =
    dynamic_cast<const Config_FeatureMessage*>( theMessage );
  if (aMsg) {
    // proccess the plugin info, load plugin
    if (myPlugins.find(aMsg->id()) == myPlugins.end()) {
      myPlugins[aMsg->id()] = "PartSetPlugin"; // TO DO: plugin name must be also imported from XMLs
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
