// Copyright (C) 2014-20xx CEA/DEN, EDF R&D


#include "ModuleBase_IModule.h"
#include "ModuleBase_IViewer.h"
#include "ModuleBase_ViewerPrs.h"
#include "ModuleBase_Operation.h"
#include "ModuleBase_ISelection.h"
#include "ModuleBase_OperationDescription.h"
#include "ModuleBase_OperationFeature.h"

#include <Events_Loop.h>

#include <ModelAPI_Events.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Session.h>

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

const char* toString(ModelAPI_ExecState theExecState) 
{
#define TO_STRING(__NAME__) case __NAME__: return #__NAME__;
  switch (theExecState) {
  TO_STRING(ModelAPI_StateDone)
  TO_STRING(ModelAPI_StateMustBeUpdated)
  TO_STRING(ModelAPI_StateExecFailed)
  TO_STRING(ModelAPI_StateInvalidArgument)
  TO_STRING(ModelAPI_StateNothing)
  default: return "Unknown ExecState.";
  }
#undef TO_STRING
}

QString ModuleBase_IModule::getFeatureError(const FeaturePtr& theFeature)
{
  QString anError;
  if (!theFeature.get() || !theFeature->data()->isValid() || theFeature->isAction())
    return anError;

  // to be removed later, this error should be got from the feature
  if (theFeature->data()->execState() == ModelAPI_StateDone ||
      theFeature->data()->execState() == ModelAPI_StateMustBeUpdated)
    return anError;

  // set error indication
  anError = QString::fromStdString(theFeature->error());
  if (anError.isEmpty()) {
    bool isDone = ( theFeature->data()->execState() == ModelAPI_StateDone
                 || theFeature->data()->execState() == ModelAPI_StateMustBeUpdated );
    if (!isDone)
      anError = toString(theFeature->data()->execState());
  }

  return anError;
}

void ModuleBase_IModule::grantedOperationIds(ModuleBase_Operation* theOperation,
                                             QStringList& theIds) const
{
}

ModuleBase_Operation* ModuleBase_IModule::getNewOperation(const std::string& theFeatureId)
{
  return new ModuleBase_OperationFeature(theFeatureId.c_str(), this);
}

bool ModuleBase_IModule::customizeObject(ObjectPtr theObject, const bool theUpdateViewer)
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

  std::string aPluginFileName = myFeaturesInFiles[theFeatureId];
  Config_WidgetReader aWdgReader = Config_WidgetReader(aPluginFileName);
  aWdgReader.readAll();
  std::string aXmlCfg = aWdgReader.featureWidgetCfg(theFeatureId);
  std::string aDescription = aWdgReader.featureDescription(theFeatureId);

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

/*/bool ModuleBase_IModule::canCommitOperation() const
{
  return true;
}*/

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
