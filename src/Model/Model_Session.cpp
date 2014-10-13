// File:        Model_Session.cxx
// Created:     20 Mar 2014
// Author:      Mikhail PONIKAROV

#include <Model_Session.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Plugin.h>
#include <Model_Data.h>
#include <Model_Document.h>
#include <Model_Application.h>
#include <Model_Events.h>
#include <Model_Validator.h>
#include <Events_Loop.h>
#include <Events_Error.h>
#include <Config_FeatureMessage.h>
#include <Config_ValidatorMessage.h>
#include <Config_ModuleReader.h>

#include <TDF_CopyTool.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_ClosureTool.hxx>

using namespace std;

static Model_Session* myImpl = new Model_Session();

// t oredirect all calls to the root document
#define ROOT_DOC boost::dynamic_pointer_cast<Model_Document>(moduleDocument())

bool Model_Session::load(const char* theFileName)
{
  return ROOT_DOC->load(theFileName);
}

bool Model_Session::save(const char* theFileName, std::list<std::string>& theResults)
{
  return ROOT_DOC->save(theFileName, theResults);
}

void Model_Session::startOperation()
{
  ROOT_DOC->startOperation();
}

void Model_Session::finishOperation()
{
  ROOT_DOC->finishOperation();
  static boost::shared_ptr<Events_Message> aFinishMsg
    (new Events_Message(Events_Loop::eventByName("FinishOperation")));
  Events_Loop::loop()->send(aFinishMsg);
}

void Model_Session::abortOperation()
{
  ROOT_DOC->abortOperation();
  static boost::shared_ptr<Events_Message> anAbortMsg
    (new Events_Message(Events_Loop::eventByName("AbortOperation")));
  Events_Loop::loop()->send(anAbortMsg);
}

bool Model_Session::isOperation()
{
  return ROOT_DOC->isOperation();
}

bool Model_Session::isModified()
{
  return ROOT_DOC->isModified();
}

bool Model_Session::canUndo()
{
  return ROOT_DOC->canUndo();
}

void Model_Session::undo()
{
  ROOT_DOC->undo();
}

bool Model_Session::canRedo()
{
  return ROOT_DOC->canRedo();
}

void Model_Session::redo()
{
  ROOT_DOC->redo();
}

FeaturePtr Model_Session::createFeature(string theFeatureID)
{
  if (this != myImpl)
    return myImpl->createFeature(theFeatureID);

  LoadPluginsInfo();
  if (myPlugins.find(theFeatureID) != myPlugins.end()) {
    std::pair<std::string, std::string>& aPlugin = myPlugins[theFeatureID]; // plugin and doc kind
    if (!aPlugin.second.empty() && aPlugin.second != activeDocument()->kind()) {
      Events_Error::send(
          string("Feature '") + theFeatureID + "' can be created only in document '"
              + aPlugin.second + "' by the XML definition");
      return FeaturePtr();
    }
    myCurrentPluginName = aPlugin.first;
    if (myPluginObjs.find(myCurrentPluginName) == myPluginObjs.end()) {
      // load plugin library if not yet done
      Config_ModuleReader::loadLibrary(myCurrentPluginName);
    }
    if (myPluginObjs.find(myCurrentPluginName) != myPluginObjs.end()) {
      FeaturePtr aCreated = myPluginObjs[myCurrentPluginName]->createFeature(theFeatureID);
      if (!aCreated) {
        Events_Error::send(
            string("Can not initialize feature '") + theFeatureID + "' in plugin '"
                + myCurrentPluginName + "'");
      }
      return aCreated;
    } else {
      Events_Error::send(string("Can not load plugin '") + myCurrentPluginName + "'");
    }
  } else {
    Events_Error::send(string("Feature '") + theFeatureID + "' not found in any plugin");
  }

  return FeaturePtr();  // return nothing
}

boost::shared_ptr<ModelAPI_Document> Model_Session::moduleDocument()
{
  return boost::shared_ptr<ModelAPI_Document>(
      Model_Application::getApplication()->getDocument("root"));
}

bool Model_Session::hasModuleDocument()
{
  return Model_Application::getApplication()->hasDocument("root");
}

boost::shared_ptr<ModelAPI_Document> Model_Session::activeDocument()
{
  if (!myCurrentDoc || !Model_Application::getApplication()->hasDocument(myCurrentDoc->id()))
    myCurrentDoc = moduleDocument();
  return myCurrentDoc;
}

void Model_Session::setActiveDocument(boost::shared_ptr<ModelAPI_Document> theDoc)
{
  if (myCurrentDoc != theDoc) {
    myCurrentDoc = theDoc;
    static boost::shared_ptr<Events_Message> aMsg(new Events_Message(Events_Loop::eventByName("CurrentDocumentChanged")));
    Events_Loop::loop()->send(aMsg);
  }
}

std::list<boost::shared_ptr<ModelAPI_Document> > Model_Session::allOpenedDocuments()
{
  list<boost::shared_ptr<ModelAPI_Document> > aResult;
  aResult.push_back(moduleDocument());
  // add subs recursively
  list<boost::shared_ptr<ModelAPI_Document> >::iterator aDoc = aResult.begin();
  for(; aDoc != aResult.end(); aDoc++) {
    DocumentPtr anAPIDoc = *aDoc;
    boost::shared_ptr<Model_Document> aDoc = boost::dynamic_pointer_cast<Model_Document>(anAPIDoc);
    if (aDoc) {
      std::set<std::string>::const_iterator aSubIter = aDoc->subDocuments().cbegin();
      for(; aSubIter != aDoc->subDocuments().cend(); aSubIter++) {
        if (!Model_Application::getApplication()->isLoadByDemand(*aSubIter)) {
          aResult.push_back(Model_Application::getApplication()->getDocument(*aSubIter));
        }
      }
    }
  }
  return aResult;
}

boost::shared_ptr<ModelAPI_Document> Model_Session::copy(
    boost::shared_ptr<ModelAPI_Document> theSource, std::string theID)
{
  // create a new document
  boost::shared_ptr<Model_Document> aNew = boost::dynamic_pointer_cast<Model_Document>(
      Model_Application::getApplication()->getDocument(theID));
  // make a copy of all labels
  TDF_Label aSourceRoot = boost::dynamic_pointer_cast<Model_Document>(theSource)->document()->Main()
      .Father();
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

Model_Session::Model_Session()
{
  myPluginsInfoLoaded = false;
  myCheckTransactions = true;
  ModelAPI_Session::setSession(boost::shared_ptr<ModelAPI_Session>(this));
  // register the configuration reading listener
  Events_Loop* aLoop = Events_Loop::loop();
  static const Events_ID kFeatureEvent = Events_Loop::eventByName("FeatureRegisterEvent");
  aLoop->registerListener(this, kFeatureEvent);
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_CREATED), 0, true);
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_UPDATED), 0, true);
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_DELETED), 0, true);
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_VALIDATOR_LOADED));
}

void Model_Session::processEvent(const boost::shared_ptr<Events_Message>& theMessage)
{
  static const Events_ID kFeatureEvent = Events_Loop::eventByName("FeatureRegisterEvent");
  static const Events_ID kValidatorEvent = Events_Loop::eventByName(EVENT_VALIDATOR_LOADED);
  if (theMessage->eventID() == kFeatureEvent) {
    const boost::shared_ptr<Config_FeatureMessage> aMsg = 
      boost::dynamic_pointer_cast<Config_FeatureMessage>(theMessage);
    if (aMsg) {
      // proccess the plugin info, load plugin
      if (myPlugins.find(aMsg->id()) == myPlugins.end()) {
        myPlugins[aMsg->id()] = std::pair<std::string, std::string>(
          aMsg->pluginLibrary(), aMsg->documentKind());
      }
    }
    // plugins information was started to load, so, it will be loaded
    myPluginsInfoLoaded = true;
  } else if (theMessage->eventID() == kValidatorEvent) {
    boost::shared_ptr<Config_ValidatorMessage> aMsg = 
      boost::dynamic_pointer_cast<Config_ValidatorMessage>(theMessage);
    if (aMsg) {
      if (aMsg->attributeId().empty()) {  // feature validator
        validators()->assignValidator(aMsg->validatorId(), aMsg->featureId(), aMsg->parameters());
      } else {  // attribute validator
        validators()->assignValidator(aMsg->validatorId(), aMsg->featureId(), aMsg->attributeId(),
                                      aMsg->parameters());
      }
    }
  } else {  // create/update/delete
    if (myCheckTransactions && !isOperation())
      Events_Error::send("Modification of data structure outside of the transaction");
  }
}

void Model_Session::LoadPluginsInfo()
{
  if (myPluginsInfoLoaded)  // nothing to do
    return;

  // Read plugins information from XML files
  Config_ModuleReader aXMLReader("FeatureRegisterEvent");
  aXMLReader.readAll();
}

void Model_Session::registerPlugin(ModelAPI_Plugin* thePlugin)
{
  myPluginObjs[myCurrentPluginName] = thePlugin;
  static Events_ID EVENT_LOAD = Events_Loop::loop()->eventByName(EVENT_PLUGIN_LOADED);
  ModelAPI_EventCreator::get()->sendUpdated(ObjectPtr(), EVENT_LOAD);
  Events_Loop::loop()->flush(EVENT_LOAD);
}

ModelAPI_ValidatorsFactory* Model_Session::validators()
{
  static Model_ValidatorsFactory* aFactory = new Model_ValidatorsFactory;
  return aFactory;
}
