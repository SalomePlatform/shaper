// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#include "XGUI_WorkshopListener.h"
#include "XGUI_Workshop.h"
#include "XGUI_Displayer.h"
#include "XGUI_ErrorMgr.h"
#include "XGUI_OperationMgr.h"
#include "XGUI_SalomeConnector.h"
#include "XGUI_ActionsMgr.h"
#include "XGUI_PropertyPanel.h"
#include "XGUI_ModuleConnector.h"
#include "XGUI_QtEvents.h"

#ifndef HAVE_SALOME
#include <AppElements_Workbench.h>
#include <AppElements_Command.h>
#include <AppElements_MainMenu.h>
#include <AppElements_MainWindow.h>
#include <AppElements_MenuGroupPanel.h>
#include <AppElements_Button.h>
#endif

#include <ModuleBase_IModule.h>

#include <ModelAPI_Object.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultCompSolid.h>
#include <ModelAPI_Tools.h>

#include <Events_Loop.h>
#include <Events_Error.h>
#include <Events_LongOp.h>

#include <ModuleBase_IWorkshop.h>

#include <ModuleBase_Operation.h>
#include <ModuleBase_OperationDescription.h>
#include <ModuleBase_OperationFeature.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_FilterFactory.h>

#include <Config_FeatureMessage.h>
#include <Config_PointerMessage.h>
#include <Config_SelectionFilterMessage.h>
#include <Config_Keywords.h>

#include <QApplication>
#include <QMainWindow>
#include <QThread>
#include <QAction>

#ifdef _DEBUG
#include <QDebug>
#include <iostream>
#endif

//#define DEBUG_FEATURE_CREATED
//#define DEBUG_FEATURE_REDISPLAY
//#define DEBUG_FEATURE_UPDATED
//#define DEBUG_RESULT_COMPSOLID

#ifdef DEBUG_FEATURE_REDISPLAY
const std::string DebugFeatureKind = "Extrusion";
#endif

XGUI_WorkshopListener::XGUI_WorkshopListener(ModuleBase_IWorkshop* theWorkshop)
  : myWorkshop(theWorkshop),
    myUpdatePrefs(false)
{
  XGUI_OperationMgr* anOperationMgr = workshop()->operationMgr();
}

//******************************************************
XGUI_WorkshopListener::~XGUI_WorkshopListener(void)
{
}

//******************************************************
void XGUI_WorkshopListener::initializeEventListening()
{
  //Initialize event listening
  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->registerListener(this, Events_Error::errorID());  //!< Listening application errors.
  aLoop->registerListener(this, Events_Loop::eventByName(Config_FeatureMessage::GUI_EVENT()));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OPERATION_LAUNCHED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
  aLoop->registerListener(this, Events_LongOp::eventID());
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_PLUGIN_LOADED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_SELFILTER_LOADED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_ERROR_CHANGED));

  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_UPDATE_VIEWER_BLOCKED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_UPDATE_VIEWER_UNBLOCKED));
}

//******************************************************
void XGUI_WorkshopListener::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  if (QApplication::instance()->thread() != QThread::currentThread()) {
    #ifdef _DEBUG
    std::cout << "XGUI_Workshop::processEvent: " << "Working in another thread." << std::endl;
    #endif
    SessionPtr aMgr = ModelAPI_Session::get();
    PostponeMessageQtEvent* aPostponeEvent = new PostponeMessageQtEvent(theMessage);
    QApplication::postEvent(this, aPostponeEvent);
    return;
  }

  //A message to start feature creation received.
  if (theMessage->eventID() == Events_Loop::loop()->eventByName(Config_FeatureMessage::GUI_EVENT())) {
    std::shared_ptr<Config_FeatureMessage> aFeatureMsg =
       std::dynamic_pointer_cast<Config_FeatureMessage>(theMessage);
    if (!aFeatureMsg->isInternal()) {
      addFeature(aFeatureMsg);
    }
  }
  // Process creation of Part
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_CREATED)) {
    std::shared_ptr<ModelAPI_ObjectUpdatedMessage> aUpdMsg =
        std::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    onFeatureCreatedMsg(aUpdMsg);
    if (myUpdatePrefs) {
      XGUI_SalomeConnector* aSalomeConnector = workshop()->salomeConnector();
      if (aSalomeConnector)
        aSalomeConnector->createPreferences();
      myUpdatePrefs = false;
    }
  }
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_PLUGIN_LOADED)) {
    myUpdatePrefs = true;
  }
  // Redisplay feature
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY)) {
    std::shared_ptr<ModelAPI_ObjectUpdatedMessage> aUpdMsg =
        std::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    onFeatureRedisplayMsg(aUpdMsg);
  }
  //Update property panel on corresponding message. If there is no current operation (no
  //property panel), or received message has different feature to the current - do nothing.
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_UPDATED)) {
    std::shared_ptr<ModelAPI_ObjectUpdatedMessage> anUpdateMsg =
        std::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    onFeatureUpdatedMsg(anUpdateMsg);
  } else if (theMessage->eventID() == Events_LongOp::eventID()) {
    if (Events_LongOp::isPerformed()) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    } else {
      QApplication::restoreOverrideCursor();
    }
  }
  //An operation passed by message. Start it, process and commit.
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OPERATION_LAUNCHED)) {
    std::shared_ptr<Config_PointerMessage> aPartSetMsg =
        std::dynamic_pointer_cast<Config_PointerMessage>(theMessage);
    //myPropertyPanel->cleanContent();
    ModuleBase_Operation* anOperation = (ModuleBase_Operation*) aPartSetMsg->pointer();
    XGUI_OperationMgr* anOperationMgr = workshop()->operationMgr();

    if (anOperationMgr->startOperation(anOperation)) {
      ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>(anOperation);
      if (aFOperation)
        workshop()->propertyPanel()->updateContentWidget(aFOperation->feature());
      if (!anOperation->getDescription()->hasXmlRepresentation()) {
        if (anOperation->commit())
          workshop()->updateCommandStatus();
      }
    }
  } 
  else if (theMessage->eventID() == Events_Loop::eventByName(EVENT_SELFILTER_LOADED)) {
    std::shared_ptr<Config_SelectionFilterMessage> aMsg = 
      std::dynamic_pointer_cast<Config_SelectionFilterMessage>(theMessage);
    if (aMsg) {
      ModuleBase_FilterFactory* aFactory = myWorkshop->selectionFilters();
      if (!aMsg->attributeId().empty()) {
        aFactory->assignFilter(aMsg->selectionFilterId(), aMsg->featureId(), aMsg->attributeId(),
                               aMsg->parameters());
      }
    }
  } else if (theMessage->eventID() == Events_Loop::eventByName(EVENT_UPDATE_VIEWER_BLOCKED)) {
    // the viewer's update context will not happens until viewer updated is emitted
      workshop()->displayer()->enableUpdateViewer(false);
  } else if (theMessage->eventID() == Events_Loop::eventByName(EVENT_UPDATE_VIEWER_UNBLOCKED)) {
    // the viewer's update context is unblocked, the viewer's update works
    XGUI_Displayer* aDisplayer = workshop()->displayer();
    aDisplayer->enableUpdateViewer(true);
  } else if (theMessage->eventID() == Events_Loop::eventByName(EVENT_OBJECT_ERROR_CHANGED)) {
    std::shared_ptr<ModelAPI_ObjectUpdatedMessage> aUpdMsg =
        std::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    std::set<ObjectPtr> anObjects = aUpdMsg->objects();

    ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                              (workshop()->operationMgr()->currentOperation());
    bool aFeatureChanged = false;
    if(aFOperation ) {
      FeaturePtr aFeature = aFOperation->feature();
      if (aFeature.get()) {
        std::set<ObjectPtr>::const_iterator aIt;
        for (aIt = anObjects.begin(); aIt != anObjects.end() && !aFeatureChanged; ++aIt) {
          aFeatureChanged = ModelAPI_Feature::feature(*aIt) == aFeature;
        }
      }
      if (aFeatureChanged)
        workshop()->operationMgr()->onValidateOperation();
    }
  } else {
    //Show error dialog if error message received.
    std::shared_ptr<Events_Error> anAppError = std::dynamic_pointer_cast<Events_Error>(theMessage);
    if (anAppError) {
      emit errorOccurred(QString::fromLatin1(anAppError->description()));
    }
    return;
  }
#ifndef HAVE_SALOME
    SessionPtr aMgr = ModelAPI_Session::get();
    AppElements_MainWindow* aMainWindow = workshop()->mainWindow();
    if (aMgr->isModified() != aMainWindow->isModifiedState())
      aMainWindow->setModifiedState(aMgr->isModified());
#endif
}

//******************************************************
void XGUI_WorkshopListener::onFeatureUpdatedMsg(
                                     const std::shared_ptr<ModelAPI_ObjectUpdatedMessage>& theMsg)
{
#ifdef DEBUG_FEATURE_UPDATED
  std::set<ObjectPtr> anObjects = theMsg->objects();
  std::set<ObjectPtr>::const_iterator aIt;
  QStringList anInfo;
  for (aIt = anObjects.begin(); aIt != anObjects.end(); ++aIt) {
    anInfo.append(ModuleBase_Tools::objectInfo((*aIt)));
  }
  QString anInfoStr = anInfo.join(";\t");
  qDebug(QString("onFeatureUpdatedMsg: %1, %2").arg(anObjects.size()).arg(anInfoStr).toStdString().c_str());
#endif
  std::set<ObjectPtr> aFeatures = theMsg->objects();
  XGUI_OperationMgr* anOperationMgr = workshop()->operationMgr();
  if (anOperationMgr->hasOperation()) {
    ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                      (anOperationMgr->currentOperation());
    if (aFOperation) {
      FeaturePtr aCurrentFeature = aFOperation->feature();
      std::set<ObjectPtr>::const_iterator aIt;
      for (aIt = aFeatures.begin(); aIt != aFeatures.end(); ++aIt) {
        ObjectPtr aNewFeature = (*aIt);
        if (aNewFeature == aCurrentFeature) {
          workshop()->propertyPanel()->updateContentWidget(aCurrentFeature);
          break;
        }
      }
    }
  }
  //anOperationMgr->onValidateOperation();

  //if (myObjectBrowser)
  //  myObjectBrowser->processEvent(theMsg);
}

//******************************************************
void XGUI_WorkshopListener::onFeatureRedisplayMsg(const std::shared_ptr<ModelAPI_ObjectUpdatedMessage>& theMsg)
{
  std::set<ObjectPtr> anObjects = theMsg->objects();
  std::set<ObjectPtr>::const_iterator aIt;

#ifdef DEBUG_FEATURE_REDISPLAY
  QStringList anInfo;
  for (aIt = anObjects.begin(); aIt != anObjects.end(); ++aIt) {
    anInfo.append(ModuleBase_Tools::objectInfo((*aIt)));
  }
  QString anInfoStr = anInfo.join(";\t");
  qDebug(QString("onFeatureRedisplayMsg: %1, %2").arg(anObjects.size()).arg(anInfoStr).toStdString().c_str());
#endif

  XGUI_Workshop* aWorkshop = workshop();
  XGUI_Displayer* aDisplayer = aWorkshop->displayer();
  bool aFirstVisualizedBody = false;

  bool aRedisplayed = false;
  //std::list<ObjectPtr> aHiddenObjects;
  for (aIt = anObjects.begin(); aIt != anObjects.end(); ++aIt) {
    ObjectPtr aObj = (*aIt);

    // Hide the object if it is invalid or concealed one
    bool aHide = !aObj->data() || !aObj->data()->isValid() || 
      aObj->isDisabled() || (!aObj->isDisplayed());
    if (!aHide) { // check that this is not hidden result
      ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(aObj);
      aHide = aRes && aRes->isConcealed();
    }

#ifdef DEBUG_RESULT_COMPSOLID
    ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(aObj);
    if (aRes.get()) {
      ResultCompSolidPtr aCompSolidRes = std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(aRes);
      if (aCompSolidRes.get()) {
          qDebug(QString("COMPSOLID, numberOfSubs = %1").arg(aCompSolidRes->numberOfSubs()).toStdString().c_str());
      }
      if (ModelAPI_Tools::compSolidOwner(aRes))
        qDebug("COMPSOLID sub-object");
    }
#endif
    #ifdef DEBUG_FEATURE_REDISPLAY
      QString anObjInfo = ModuleBase_Tools::objectInfo((aObj));
      FeaturePtr aFeature = ModelAPI_Feature::feature(aObj);
      if (aFeature.get()) {
        std::string aKind = aFeature->getKind();
        if (aKind == DebugFeatureKind) {
          qDebug(QString("visible=%1, hide=%2 : display= %2").arg(aDisplayer->isVisible(aObj))
                                            .arg(aHide).arg(anObjInfo).toStdString().c_str());
        }
      }
    #endif
    if (aHide) {
      //we should provide objects which are hidden in the viewer, e.g. sketch always should visualizes
      // all sub-features, if some features are to be hidden, sould be proposed may be to removed #1223
      //aHiddenObjects.push_back(aObj);
      aRedisplayed = aDisplayer->erase(aObj, false) || aRedisplayed;
      #ifdef DEBUG_FEATURE_REDISPLAY
        // Redisplay the visible object or the object of the current operation
        bool isVisibleObject = aDisplayer->isVisible(aObj);

        QString anObjInfo = ModuleBase_Tools::objectInfo((aObj));
        //qDebug(QString("visible=%1 : erase  = %2").arg(isVisibleObject).arg(anObjInfo).toStdString().c_str());
      #endif
    }
    else {
      // Redisplay the visible object or the object of the current operation
      bool isVisibleObject = aDisplayer->isVisible(aObj);
      #ifdef DEBUG_FEATURE_REDISPLAY
        QString anObjInfo = ModuleBase_Tools::objectInfo((aObj));
        //qDebug(QString("visible=%1 : display= %2").arg(isVisibleObject).arg(anObjInfo).toStdString().c_str());
      #endif

      if (isVisibleObject)  { // redisplay visible object
        //displayObject(aObj);  // In order to update presentation
        // in order to avoid the check whether the object can be redisplayed, the exact method
        // of redisplay is called. This modification is made in order to have the line is updated
        // by creation of a horizontal constraint on the line by preselection
        if (ModelAPI_Tools::hasSubResults(std::dynamic_pointer_cast<ModelAPI_Result>(aObj))) {
          aRedisplayed = aDisplayer->erase(aObj, false) || aRedisplayed;
        }
        else {
          aRedisplayed = aDisplayer->redisplay(aObj, false) || aRedisplayed;
          // Deactivate object of current operation from selection
          aWorkshop->deactivateActiveObject(aObj, false);
        }
      } else { // display object if the current operation has it
        if (displayObject(aObj, aFirstVisualizedBody)) {
          aRedisplayed = true;
          // Deactivate object of current operation from selection
          aWorkshop->deactivateActiveObject(aObj, false);
        }
      }
    }
  }
  // this processing should be moved in another place in order to do not cause problems in
  // flush messages chain
  //if (aHiddenObjects.size() > 0)
  //  myWorkshop->module()->processHiddenObject(aHiddenObjects);

  bool isCustomized = customizeCurrentObject(anObjects, aRedisplayed);
  if (aRedisplayed || isCustomized) {
    //VSV FitAll updated viewer by it self
    if (aFirstVisualizedBody)
      myWorkshop->viewer()->fitAll();
    else 
      aDisplayer->updateViewer();
  }
}
//******************************************************
void XGUI_WorkshopListener::onFeatureCreatedMsg(const std::shared_ptr<ModelAPI_ObjectUpdatedMessage>& theMsg)
{
  std::set<ObjectPtr> anObjects = theMsg->objects();
  std::set<ObjectPtr>::const_iterator aIt;
#ifdef DEBUG_FEATURE_CREATED
  QStringList anInfo;
  for (aIt = anObjects.begin(); aIt != anObjects.end(); ++aIt) {
    anInfo.append(ModuleBase_Tools::objectInfo((*aIt)));
  }
  QString anInfoStr = anInfo.join(";\t");
  qDebug(QString("onFeatureCreatedMsg: %1, %2").arg(anObjects.size()).arg(anInfoStr).toStdString().c_str());
#endif

  bool aFirstVisualizedBody = false;

  //bool aHasPart = false;
  bool aDisplayed = false;
  for (aIt = anObjects.begin(); aIt != anObjects.end(); ++aIt) {
    ObjectPtr anObject = *aIt;

#ifdef DEBUG_RESULT_COMPSOLID
    ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
    if (aRes.get()) {
      ResultCompSolidPtr aCompSolidRes = std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(aRes);
      if (aCompSolidRes.get()) {
          qDebug(QString("COMPSOLID, numberOfSubs = %1").arg(aCompSolidRes->numberOfSubs()).toStdString().c_str());
      }
      if (ModelAPI_Tools::compSolidOwner(aRes))
        qDebug("COMPSOLID sub-object");
    }
#endif
    // the validity of the data should be checked here in order to avoid display of the objects,
    // which were created, then deleted, but flush for the creation event happens after that
    // we should not display disabled objects
    bool aHide = !anObject->data()->isValid() || 
                 anObject->isDisabled() ||
                 !anObject->isDisplayed();
    if (!aHide) { // check that this is not hidden result
      ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
      bool isConcealed = aRes && aRes->isConcealed();
      aHide = aRes && aRes->isConcealed();
    }
    if (!aHide) {
      // setDisplayed has to be called in order to synchronize internal state of the object 
      // with list of displayed objects
      if (myWorkshop->module()->canDisplayObject(anObject)) {
        anObject->setDisplayed(true);
        aDisplayed = displayObject(*aIt, aFirstVisualizedBody);
      } else 
        anObject->setDisplayed(false);
    }
  }

  bool isCustomized = customizeCurrentObject(anObjects, aDisplayed);

  //if (myObjectBrowser)
  //  myObjectBrowser->processEvent(theMsg);
  if (aDisplayed) {
    //VSV FitAll updated viewer by it self
    if (aFirstVisualizedBody)
      myWorkshop->viewer()->fitAll();
    else
      workshop()->displayer()->updateViewer();
  }
  //if (aHasPart) { // TODO: Avoid activate last part on loading of document
  //  activateLastPart();
  //}
}

bool XGUI_WorkshopListener::event(QEvent * theEvent)
{
  PostponeMessageQtEvent* aPostponedEv = dynamic_cast<PostponeMessageQtEvent*>(theEvent);
  if (aPostponedEv) {
    std::shared_ptr<Events_Message> aEventPtr = aPostponedEv->postponedMessage();
    processEvent(aEventPtr);
    return true;
  }
  return false;
}

void XGUI_WorkshopListener::addFeature(const std::shared_ptr<Config_FeatureMessage>& theMessage)
{
  if (!theMessage) {
#ifdef _DEBUG
    qDebug() << "XGUI_WorkshopListener::addFeature: NULL message.";
#endif
    return;
  }
  ActionInfo aFeatureInfo;
  aFeatureInfo.initFrom(theMessage);

  XGUI_Workshop* aWorkshop = workshop();

  QString aWchName = QString::fromStdString(theMessage->workbenchId());
  QStringList aNestedFeatures =
      QString::fromStdString(theMessage->nestedFeatures()).split(" ", QString::SkipEmptyParts);
  QString aDocKind = QString::fromStdString(theMessage->documentKind());
  QList<QAction*> aNestedActList;
  bool isColumnButton = !aNestedFeatures.isEmpty();
  if (isColumnButton) {
    QString aNestedActions = QString::fromStdString(theMessage->actionsWhenNested());
    XGUI_OperationMgr* anOperationMgr = aWorkshop->operationMgr();
    XGUI_ActionsMgr* anActionsMgr = aWorkshop->actionsMgr();
    if (aNestedActions.contains(FEATURE_WHEN_NESTED_ACCEPT)) {
      QAction* anAction = anActionsMgr->operationStateAction(XGUI_ActionsMgr::AcceptAll, NULL);
      connect(anAction, SIGNAL(triggered()), anOperationMgr, SLOT(commitAllOperations()));
      aNestedActList << anAction;
    }
    if (aNestedActions.contains(FEATURE_WHEN_NESTED_ABORT)) {
      QAction* anAction = anActionsMgr->operationStateAction(XGUI_ActionsMgr::AbortAll, NULL);
      connect(anAction, SIGNAL(triggered()), anOperationMgr, SLOT(abortAllOperations()));
      aNestedActList << anAction;
    }
  }

#ifdef HAVE_SALOME
  XGUI_SalomeConnector* aSalomeConnector = aWorkshop->salomeConnector();
  QAction* aAction;
  if (isColumnButton) {
    aAction = aSalomeConnector->addFeatureOfNested(aWchName, aFeatureInfo, aNestedActList);
  } else {
    //Issue #650: in the SALOME mode the tooltip should be same as text
    aFeatureInfo.toolTip = aFeatureInfo.text;
    aAction = aSalomeConnector->addFeature(aWchName, aFeatureInfo);
  }
  aSalomeConnector->setNestedActions(aFeatureInfo.id, aNestedFeatures);
  aSalomeConnector->setDocumentKind(aFeatureInfo.id, aDocKind);

  aWorkshop->actionsMgr()->addCommand(aAction);
  aWorkshop->module()->actionCreated(aAction);
#else 
  //Find or create Workbench
  AppElements_MainMenu* aMenuBar = aWorkshop->mainWindow()->menuObject();
  AppElements_Workbench* aPage = aMenuBar->findWorkbench(aWchName);
  if (!aPage) {
    aPage = aWorkshop->addWorkbench(aWchName);
  }
  //Find or create Group
  QString aGroupName = QString::fromStdString(theMessage->groupId());
  AppElements_MenuGroupPanel* aGroup = aPage->findGroup(aGroupName);
  if (!aGroup) {
    aGroup = aPage->addGroup(aGroupName);
  }
  // Check if hotkey sequence is already defined:
  XGUI_ActionsMgr* anActionsMgr = aWorkshop->actionsMgr();
  QKeySequence aHotKey = anActionsMgr->registerShortcut(aFeatureInfo.shortcut);
  if(aHotKey != aFeatureInfo.shortcut) {
    aFeatureInfo.shortcut = aHotKey;
  }
  // Create feature...
  AppElements_Command* aCommand = aGroup->addFeature(aFeatureInfo,
                                                      aDocKind,
                                                      aNestedFeatures);
  // Enrich created button with accept/abort buttons if necessary
  AppElements_Button* aButton = aCommand->button();
  if (aButton->isColumnButton()) {
    aButton->setAdditionalButtons(aNestedActList);
  }
  aWorkshop->actionsMgr()->addCommand(aCommand);
  aWorkshop->module()->actionCreated(aCommand);
#endif
}


//**************************************************************
bool XGUI_WorkshopListener::displayObject(ObjectPtr theObj, bool& theFirstVisualizedBody)
{
#ifdef DEBUG_RESULT_COMPSOLID
  ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(theObj);
  if (aRes.get() && (ModelAPI_Tools::hasSubResults(aRes) || ModelAPI_Tools::compSolidOwner(aRes))) {
    ResultCompSolidPtr aCompSolidRes = std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(aRes);
    if (aCompSolidRes.get()) {
      qDebug("COMPSOLID: displayObject");
    }
  }
#endif

  bool aDisplayed = false;
  XGUI_Workshop* aWorkshop = workshop();
  // do not display the object if it has sub objects. They should be displayed separately.
  if (!aWorkshop->module()->canDisplayObject(theObj) ||
      ModelAPI_Tools::hasSubResults(std::dynamic_pointer_cast<ModelAPI_Result>(theObj)))
    return aDisplayed;

  XGUI_Displayer* aDisplayer = aWorkshop->displayer();
  ResultBodyPtr aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(theObj);
  if (aBody.get() != NULL) {
    int aNb = aDisplayer->objectsCount();
    aDisplayed = aDisplayer->display(theObj, false);
    if (aNb == 0)
      theFirstVisualizedBody = true;
  } else 
    aDisplayed = aDisplayer->display(theObj, false);

  return aDisplayed;
}

bool XGUI_WorkshopListener::customizeCurrentObject(const std::set<ObjectPtr>& theObjects,
                                                   bool theForceRedisplay)
{
  XGUI_OperationMgr* anOperationMgr = workshop()->operationMgr();
  FeaturePtr aCurrentFeature;
  if (anOperationMgr->hasOperation()) {
    ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                      (anOperationMgr->currentOperation());
    if (aFOperation) {
      aCurrentFeature = aFOperation->feature();
    }
  }

  bool aCustomized = false;
  if (aCurrentFeature.get()) {
    // the customize presentation should be redisplayed if force redislayed is true or
    // if a list of message objects contains the operation feature for case when
    // the feature is hidden, but arguments of the feature are modified
    // e.g. extrusion is hidden(h=0) but sketch is chosen
    if (theForceRedisplay || theObjects.find(aCurrentFeature) != theObjects.end()) {
      aCustomized = myWorkshop->module()->customizeObject(aCurrentFeature,
                                           ModuleBase_IModule::CustomizeArguments, false);
      aCustomized = myWorkshop->module()->customizeObject(aCurrentFeature,
                                           ModuleBase_IModule::CustomizeResults, false);
      aCustomized = myWorkshop->module()->customizeObject(aCurrentFeature,
                                           ModuleBase_IModule::CustomizeHighlightedObjects, false);
    }
  }
  return aCustomized;
}

XGUI_Workshop* XGUI_WorkshopListener::workshop() const
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myWorkshop);
  return aConnector->workshop();
}
