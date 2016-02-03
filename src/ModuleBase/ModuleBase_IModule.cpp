// Copyright (C) 2014-20xx CEA/DEN, EDF R&D


#include "ModuleBase_IModule.h"
#include "ModuleBase_IViewer.h"
#include "ModuleBase_ViewerPrs.h"
#include "ModuleBase_Operation.h"
#include "ModuleBase_ISelection.h"
#include "ModuleBase_OperationDescription.h"
#include "ModuleBase_OperationFeature.h"
#include "ModuleBase_ModelWidget.h"

#include <Events_Loop.h>

#include <ModelAPI_Events.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Session.h>
#include "ModelAPI_Tools.h"

#include <Config_PointerMessage.h>
#include <Config_WidgetReader.h>
#include <Config_ModuleReader.h>

#include <QAction>

ModuleBase_IModule::ModuleBase_IModule(ModuleBase_IWorkshop* theParent)
  : QObject(theParent), myWorkshop(theParent) 
{
  connect(myWorkshop, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));


  //connect(myWorkshop->viewer(), SIGNAL(mousePress(QMouseEvent*)), this,
  //        SLOT(onMousePressed(QMouseEvent*)));
  //connect(myWorkshop->viewer(), SIGNAL(mouseRelease(QMouseEvent*)), this,
  //        SLOT(onMouseReleased(QMouseEvent*)));
  //connect(myWorkshop->viewer(), SIGNAL(mouseMove(QMouseEvent*)), this,
  //        SLOT(onMouseMoved(QMouseEvent*)));
  //connect(myWorkshop->viewer(), SIGNAL(keyRelease(QKeyEvent*)), this,
  //        SLOT(onKeyRelease(QKeyEvent*)));
  //connect(myWorkshop->viewer(), SIGNAL(mouseDoubleClick(QMouseEvent*)), this,
  //        SLOT(onMouseDoubleClick(QMouseEvent*)));
}

void ModuleBase_IModule::launchOperation(const QString& theCmdId)
{
  if (!myWorkshop->canStartOperation(theCmdId))
    return;

  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                             (createOperation(theCmdId.toStdString()));
  if (aFOperation) {
    ModuleBase_ISelection* aSelection = myWorkshop->selection();
    // Initialise operation with preliminary selection
    aFOperation->initSelection(aSelection, myWorkshop->viewer());
    sendOperation(aFOperation);
  }
}


void ModuleBase_IModule::sendOperation(ModuleBase_Operation* theOperation)
{
  static Events_ID aModuleEvent = Events_Loop::eventByName(EVENT_OPERATION_LAUNCHED);
  std::shared_ptr<Config_PointerMessage> aMessage =
      std::shared_ptr<Config_PointerMessage>(new Config_PointerMessage(aModuleEvent, this));
  aMessage->setPointer(theOperation);
  Events_Loop::loop()->send(aMessage);
}

QString ModuleBase_IModule::getFeatureError(const FeaturePtr& theFeature)
{
  return ModelAPI_Tools::getFeatureError(theFeature).c_str();
}

void ModuleBase_IModule::grantedOperationIds(ModuleBase_Operation* theOperation,
                                             QStringList& theIds) const
{
}

ModuleBase_Operation* ModuleBase_IModule::getNewOperation(const std::string& theFeatureId)
{
  return new ModuleBase_OperationFeature(theFeatureId.c_str(), this);
}

bool ModuleBase_IModule::customizeObject(ObjectPtr theObject,
                                         const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag,
                                         const bool theUpdateViewer)
{
  return false;
}

ModuleBase_Operation* ModuleBase_IModule::createOperation(const std::string& theFeatureId)
{
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                          (getNewOperation(theFeatureId));
  // If the operation is launched as sub-operation of another then we have to initialize
  // parent feature
  ModuleBase_OperationFeature* aCurOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                         (myWorkshop->currentOperation());
  if (aCurOperation) {
    FeaturePtr aFeature = aCurOperation->feature();
    CompositeFeaturePtr aCompFeature =
        std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aFeature);
    if (aCompFeature) {
      aFOperation->setParentFeature(aCompFeature);
    }
  }

  std::string aXmlCfg, aDescription;
  getXMLRepresentation(theFeatureId, aXmlCfg, aDescription);
  aFOperation->getDescription()->setDescription(QString::fromStdString(aDescription));
  aFOperation->getDescription()->setXmlRepresentation(QString::fromStdString(aXmlCfg));

  return aFOperation;
}

void ModuleBase_IModule::createFeatures()
{
  registerValidators();
  registerFilters();
  registerProperties();

  Config_ModuleReader aXMLReader = Config_ModuleReader();
  aXMLReader.readAll();
  myFeaturesInFiles = aXMLReader.featuresInFiles();
}


void ModuleBase_IModule::actionCreated(QAction* theFeature)
{
  theFeature->setStatusTip(theFeature->text());
  connect(theFeature, SIGNAL(triggered(bool)), this, SLOT(onFeatureTriggered()));
}

bool ModuleBase_IModule::canEraseObject(const ObjectPtr& theObject) const
{
  return true;
}

bool ModuleBase_IModule::canDisplayObject(const ObjectPtr& theObject) const
{
  return true;
}

bool ModuleBase_IModule::canUndo() const
{
  SessionPtr aMgr = ModelAPI_Session::get();
  return aMgr->hasModuleDocument() && aMgr->canUndo() && !aMgr->isOperation();
}

bool ModuleBase_IModule::canRedo() const
{
  SessionPtr aMgr = ModelAPI_Session::get();
  return aMgr->hasModuleDocument() && aMgr->canRedo() && !aMgr->isOperation();
}

void ModuleBase_IModule::onFeatureTriggered()
{
  QAction* aCmd = dynamic_cast<QAction*>(sender());
  //Do nothing on uncheck
  if (aCmd->isCheckable() && !aCmd->isChecked()) {
    ModuleBase_Operation* anOperation = myWorkshop->findStartedOperation(aCmd->data().toString());
    if (myWorkshop->canStopOperation(anOperation))
      myWorkshop->abortOperation(anOperation);
    else {
      aCmd->setChecked(true);
    }
  }
  else {
    launchOperation(aCmd->data().toString());
    emit operationLaunched();
  }
}

void ModuleBase_IModule::editFeature(FeaturePtr theFeature)
{
  std::string aFeatureId = theFeature->getKind();
  if (!myWorkshop->canStartOperation(aFeatureId.c_str()))
    return;

  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                         (createOperation(aFeatureId));
  if (aFOperation) {
    aFOperation->setFeature(theFeature);
    sendOperation(aFOperation);
  }
}

bool ModuleBase_IModule::canActivateSelection(const ObjectPtr& theObject) const
{
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                     (myWorkshop->currentOperation());
  return !aFOperation || !aFOperation->hasObject(theObject);
}

void ModuleBase_IModule::operationResumed(ModuleBase_Operation* theOperation) 
{
  emit resumed(theOperation);
}

void ModuleBase_IModule::getXMLRepresentation(const std::string& theFeatureId,
                                              std::string& theXmlCfg, std::string& theDescription)
{
  std::string aPluginFileName = myFeaturesInFiles[theFeatureId];
  Config_WidgetReader aWdgReader = Config_WidgetReader(aPluginFileName);
  aWdgReader.readAll();

  theXmlCfg = aWdgReader.featureWidgetCfg(theFeatureId);
  theDescription = aWdgReader.featureDescription(theFeatureId);
}
