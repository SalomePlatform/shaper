// Copyright (C) 2014-20xx CEA/DEN, EDF R&D


#include "ModuleBase_IModule.h"
#include "ModuleBase_IViewer.h"
#include "ModuleBase_ViewerPrs.h"
#include "ModuleBase_Operation.h"
#include "ModuleBase_ISelection.h"
#include "ModuleBase_OperationDescription.h"

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

  ModuleBase_Operation* anOperation = createOperation(theCmdId.toStdString());
  ModuleBase_ISelection* aSelection = myWorkshop->selection();
  // Initialise operation with preliminary selection
  anOperation->initSelection(aSelection, myWorkshop->viewer());
  sendOperation(anOperation);
}


void ModuleBase_IModule::sendOperation(ModuleBase_Operation* theOperation)
{
  static Events_ID aModuleEvent = Events_Loop::eventByName(EVENT_OPERATION_LAUNCHED);
  std::shared_ptr<Config_PointerMessage> aMessage =
      std::shared_ptr<Config_PointerMessage>(new Config_PointerMessage(aModuleEvent, this));
  aMessage->setPointer(theOperation);
  Events_Loop::loop()->send(aMessage);
}

ModuleBase_Operation* ModuleBase_IModule::getNewOperation(const std::string& theFeatureId)
{
  return new ModuleBase_Operation(theFeatureId.c_str(), this);
}

ModuleBase_Operation* ModuleBase_IModule::createOperation(const std::string& theFeatureId)
{
  ModuleBase_Operation* anOperation = getNewOperation(theFeatureId);

  // If the operation is launched as sub-operation of another then we have to initialise
  // parent feature
  ModuleBase_Operation* aCurOperation = myWorkshop->currentOperation();
  if (aCurOperation) {
    FeaturePtr aFeature = aCurOperation->feature();
    CompositeFeaturePtr aCompFeature =
        std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aFeature);
    if (aCompFeature) {
      anOperation->setParentFeature(aCompFeature);
    }
  }

  std::string aPluginFileName = myFeaturesInFiles[theFeatureId];
  Config_WidgetReader aWdgReader = Config_WidgetReader(aPluginFileName);
  aWdgReader.readAll();
  std::string aXmlCfg = aWdgReader.featureWidgetCfg(theFeatureId);
  std::string aDescription = aWdgReader.featureDescription(theFeatureId);

  anOperation->getDescription()->setDescription(QString::fromStdString(aDescription));
  anOperation->getDescription()->setXmlRepresentation(QString::fromStdString(aXmlCfg));

  return anOperation;
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

bool ModuleBase_IModule::canDisplayObject(const ObjectPtr& theObject) const
{
  ModuleBase_Operation* anOperation = myWorkshop->currentOperation();
  return anOperation && anOperation->hasObject(theObject);
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
  if (aCmd->isCheckable() && !aCmd->isChecked())
    return;
  launchOperation(aCmd->data().toString());
}


void ModuleBase_IModule::editFeature(FeaturePtr theFeature)
{
  std::string aFeatureId = theFeature->getKind();
  if (!myWorkshop->canStartOperation(aFeatureId.c_str()))
    return;

  ModuleBase_Operation* anOperation = createOperation(aFeatureId);
  anOperation->setFeature(theFeature);
  sendOperation(anOperation);
}
