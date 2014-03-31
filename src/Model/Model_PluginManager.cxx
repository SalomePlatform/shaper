// File:        Model_PluginManager.cxx
// Created:     20 Mar 2014
// Author:      Mikhail PONIKAROV

#include <Model_PluginManager.hxx>
#include <ModelAPI_Feature.hxx>
#include <Model_Feature.hxx>
#include <Event_Loop.hxx>
#include <Config_FeatureMessage.h>

using namespace std;

boost::shared_ptr<ModelAPI_Feature> Model_PluginManager::CreateFeature(string theFeatureID)
{
  return boost::shared_ptr<ModelAPI_Feature>(new Model_Feature());
}

Model_PluginManager::Model_PluginManager()
{
  static Event_ID aFeatureEvent = Event_Loop::EventByName("Feature");

  static Model_PluginManager* myImpl = new Model_PluginManager();
  ModelAPI_PluginManager::SetPluginManager(boost::shared_ptr<ModelAPI_PluginManager>(myImpl));
  // register the configuration reading listener
  Event_Loop* aLoop = Event_Loop::Loop();
  aLoop->RegisterListener(myImpl, aFeatureEvent);
}

void Model_PluginManager::ProcessEvent(const Event_Message* theMessage)
{
  const Config_FeatureMessage* aMsg =
    dynamic_cast<const Config_FeatureMessage*>( theMessage );
  if(aMsg) {
    // proccess the plugin info, load plugin
  }
}
