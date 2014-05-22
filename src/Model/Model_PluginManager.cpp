// File:        Model_PluginManager.cxx
// Created:     20 Mar 2014
// Author:      Mikhail PONIKAROV

#include <Model_PluginManager.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Plugin.h>
#include <Model_Data.h>
#include <Model_Document.h>
#include <Model_Application.h>
#include <Model_Events.h>
#include <Events_Loop.h>
#include <Events_Error.h>
#include <Config_FeatureMessage.h>
#include <Config_ModuleReader.h>

#include <TDF_CopyTool.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_ClosureTool.hxx>


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
      if (!aCreated) {
        Events_Error::send(string("Can not initialize feature '") + theFeatureID +
          "' in plugin '" + myCurrentPluginName + "'");
      }
      return aCreated;
    } else {
      Events_Error::send(string("Can not load plugin '") + myCurrentPluginName + "'");
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
  if (!myCurrentDoc || !Model_Application::getApplication()->hasDocument(myCurrentDoc->id()))
    myCurrentDoc = rootDocument();
  return myCurrentDoc;
}

void Model_PluginManager::setCurrentDocument(boost::shared_ptr<ModelAPI_Document> theDoc)
{
  myCurrentDoc = theDoc;
}

boost::shared_ptr<ModelAPI_Document> Model_PluginManager::copy(
  boost::shared_ptr<ModelAPI_Document> theSource, std::string theID) 
{
  // create a new document
  boost::shared_ptr<Model_Document> aNew = boost::dynamic_pointer_cast<Model_Document>(
    Model_Application::getApplication()->getDocument(theID));
  // make a copy of all labels
  TDF_Label aSourceRoot = 
    boost::dynamic_pointer_cast<Model_Document>(theSource)->document()->Main().Father();
  TDF_Label aTargetRoot = aNew->document()->Main().Father();
  Handle(TDF_DataSet) aDS = new TDF_DataSet;
  aDS->AddLabel(aSourceRoot);
  TDF_ClosureTool::Closure(aDS);
  Handle(TDF_RelocationTable) aRT = new TDF_RelocationTable;
  aRT->SetRelocation(aSourceRoot, aTargetRoot);
  TDF_CopyTool::Copy(aDS, aRT);

  aNew->synchronizeFeatures();
  return aNew;
}

Model_PluginManager::Model_PluginManager()
{
  myPluginsInfoLoaded = false;
  ModelAPI_PluginManager::SetPluginManager(boost::shared_ptr<ModelAPI_PluginManager>(this));
  // register the configuration reading listener
  Events_Loop* aLoop = Events_Loop::loop();
  static Events_ID FeatureEvent = Events_Loop::eventByName("FeatureRegisterEvent");
  aLoop->registerListener(this, FeatureEvent);
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_FEATURE_CREATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_FEATURE_UPDATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_FEATURE_DELETED));
}

void Model_PluginManager::processEvent(const Events_Message* theMessage)
{
  static Events_ID FeatureEvent = Events_Loop::eventByName("FeatureRegisterEvent");
  if (theMessage->eventID() == FeatureEvent) {
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
  } else { // create/update/delete
    if (!rootDocument()->isOperation())
      Events_Error::send("Modification of data structure outside of the transaction");
  }
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
