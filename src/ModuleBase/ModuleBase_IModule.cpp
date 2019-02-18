// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "ModelAPI_IReentrant.h"
#include "ModelAPI_EventReentrantMessage.h"

#include "ModuleBase_IModule.h"
#include "ModuleBase_IViewer.h"
#include "ModuleBase_ViewerPrs.h"
#include "ModuleBase_Operation.h"
#include "ModuleBase_IPropertyPanel.h"
#include "ModuleBase_ISelection.h"
#include "ModuleBase_OperationDescription.h"
#include "ModuleBase_OperationFeature.h"
#include "ModuleBase_ModelWidget.h"
#include "ModuleBase_WidgetFactory.h"
#include "ModuleBase_PageWidget.h"
#include "ModuleBase_Dialog.h"
#include "ModuleBase_IErrorMgr.h"

#include <Events_Loop.h>
#include <Events_Message.h>

#include <ModelAPI_Events.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Session.h>
#include "ModelAPI_Tools.h"

#include <Config_PointerMessage.h>
#include <Config_WidgetReader.h>
#include <Config_ModuleReader.h>

#include <QAction>
#include <QMainWindow>
#include <QDialog>
#include <QLayout>
#include <QDialogButtonBox>
#include <QPushButton>

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

void ModuleBase_IModule::launchModal(const QString& theCmdId)
{
  bool isCommitted;
  if (!myWorkshop->canStartOperation(theCmdId, isCommitted))
    return;

  std::string aXmlCfg, aDescription;
  getXMLRepresentation(theCmdId.toStdString(), aXmlCfg, aDescription);

  SessionPtr aMgr = ModelAPI_Session::get();
  aMgr->startOperation(theCmdId.toStdString());

  ModuleBase_Dialog aDlg(myWorkshop, theCmdId, aXmlCfg);
  if (aDlg.exec() == QDialog::Accepted)
    aMgr->finishOperation();
  else
    aMgr->abortOperation();
  myWorkshop->updateCommandStatus();
}


void ModuleBase_IModule::launchOperation(const QString& theCmdId,
                                         const bool& isStartAfterCommitOnly)
{
  /// selection should be obtained from workshop before ask if the operation can be started as
  /// the canStartOperation method performs commit/abort of previous operation.
  /// Sometimes commit/abort may cause selection clear(Sketch operation) as a result
  /// it will be lost and is not used for preselection.
  ModuleBase_ISelection* aSelection = myWorkshop->selection();
  QList<ModuleBase_ViewerPrsPtr> aPreSelected =
    aSelection->getSelected(ModuleBase_ISelection::AllControls);

  ModuleBase_OperationFeature* aCurOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                         (myWorkshop->currentOperation());
  QString aCurOperationKind = aCurOperation ? aCurOperation->getDescription()->operationId() : "";

  bool isCommitted;
  if (!myWorkshop->canStartOperation(theCmdId, isCommitted))
    return;

  /// reentrant operation(Sketch Line) should not be started if operation is aborted
  if (isStartAfterCommitOnly && !isCommitted)
    return;

  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                             (createOperation(theCmdId.toStdString()));
  if (aFOperation) {
    std::shared_ptr<Events_Message> aMessage = reentrantMessage();
    if (aMessage.get()) {
      setReentrantPreSelection(aMessage);
    }
    else if (canUsePreselection(aCurOperationKind, theCmdId)) {
      // restore of previous opeation is absent or new launched operation has the same kind
      aFOperation->initSelection(aPreSelected);
    }
    workshop()->processLaunchOperation(aFOperation);

    if (aFOperation) {
      FeaturePtr aFeature = aFOperation->feature();
      ModelReentrantPtr aReentrantFeature =
                                      std::dynamic_pointer_cast<ModelAPI_IReentrant>(aFeature);
      if (aReentrantFeature.get()) {
        if (aMessage.get()) {
          ModuleBase_IPropertyPanel* aPanel = workshop()->propertyPanel();
          std::string aPrevAttribute = aReentrantFeature->processEvent(aMessage);
          if (!aPrevAttribute.empty()) {
            workshop()->errorMgr()->updateActions(aFeature);
            ModuleBase_ModelWidget* aPrevWidget = aPanel->modelWidget(aPrevAttribute);
            aPanel->activateNextWidget(aPrevWidget);
          }
        }
      }
    }
  }
}

Handle(AIS_InteractiveObject) ModuleBase_IModule::createPresentation(const ResultPtr& theResult)
{
  return Handle(AIS_InteractiveObject)();
}

bool ModuleBase_IModule::canBeShaded(Handle(AIS_InteractiveObject) theAIS) const
{
  return true;
}

QString ModuleBase_IModule::getFeatureError(const FeaturePtr& theFeature)
{
  // Error already translated.
  std::string aMsg = ModelAPI_Tools::getFeatureError(theFeature);
  return QString::fromUtf8(aMsg.c_str());
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

bool ModuleBase_IModule::canUsePreselection(const QString& thePreviousOperationKind,
                                            const QString& theStartedOperationKind)
{
  // no previous operation
  if (thePreviousOperationKind.isEmpty())
    return true;
  // edit operation
  if (thePreviousOperationKind.endsWith(ModuleBase_OperationFeature::EditSuffix()))
    return true;

  // reentrant operation
  if (thePreviousOperationKind == theStartedOperationKind)
    return true;

  return false;
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
    if (myWorkshop->canStopOperation(anOperation)) {
      bool isCommitted;
      myWorkshop->stopOperation(anOperation, isCommitted);
    }
    else {
      aCmd->setChecked(true);
    }
  }
  else {
    QString aCmdId = aCmd->data().toString();
    std::shared_ptr<Config_FeatureMessage> aInfo = myWorkshop->featureInfo(aCmdId);
    if (aInfo.get() && aInfo->isModal()) {
      launchModal(aCmdId);
    } else {
      launchOperation(aCmdId, false);
    }
  }
}

void ModuleBase_IModule::editFeature(FeaturePtr theFeature)
{
  std::string aFeatureId = theFeature->getKind();
  bool isCommitted;
  if (!myWorkshop->canStartOperation(aFeatureId.c_str(), isCommitted))
    return;

  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                         (createOperation(aFeatureId));
  if (aFOperation) {
    aFOperation->setFeature(theFeature);
    workshop()->processLaunchOperation(aFOperation);
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
