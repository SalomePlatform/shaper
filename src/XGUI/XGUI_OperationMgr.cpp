// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        XGUI_OperationMgr.cpp
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "XGUI_OperationMgr.h"
#include "XGUI_ModuleConnector.h"
#include "XGUI_Workshop.h"
#include "XGUI_ErrorMgr.h"

#include <ModuleBase_IPropertyPanel.h>
#include <ModuleBase_ModelWidget.h>
#include "ModuleBase_Operation.h"
#include "ModuleBase_IWorkshop.h"
#include "ModuleBase_IModule.h"
#include <ModuleBase_IViewer.h>
#include "ModuleBase_OperationDescription.h"
#include "ModuleBase_OperationFeature.h"
#include "ModuleBase_Tools.h"

#include "ModelAPI_CompositeFeature.h"
#include "ModelAPI_Session.h"

#include <QMessageBox>
#include <QApplication>
#include <QKeyEvent>

//#define DEBUG_CURRENT_FEATURE

XGUI_OperationMgr::XGUI_OperationMgr(QObject* theParent,
                                     ModuleBase_IWorkshop* theWorkshop)
: QObject(theParent), myIsApplyEnabled(false), myWorkshop(theWorkshop)
{
}

XGUI_OperationMgr::~XGUI_OperationMgr()
{
}

ModuleBase_Operation* XGUI_OperationMgr::currentOperation() const
{
  return myOperations.count() > 0 ? myOperations.last() : 0;
}

bool XGUI_OperationMgr::isCurrentOperation(ModuleBase_Operation* theOperation)
{
  if(!hasOperation())
    return false;
  return currentOperation() == theOperation;
}

bool XGUI_OperationMgr::hasOperation() const
{
  return !myOperations.isEmpty() && (myOperations.last() != NULL);
}

bool XGUI_OperationMgr::hasOperation(const QString& theId) const
{
  foreach(ModuleBase_Operation* aOp, myOperations) {
    if (aOp->id() == theId)
      return true;
  }
  return false;
}

ModuleBase_Operation* XGUI_OperationMgr::findOperation(const QString& theId) const
{
  foreach(ModuleBase_Operation* aOp, myOperations) {
    if (aOp->id() == theId)
      return aOp;
  }
  return 0;
}


int XGUI_OperationMgr::operationsCount() const
{
  return myOperations.count();
}

QStringList XGUI_OperationMgr::operationList() const
{
  QStringList result;
  foreach(ModuleBase_Operation* eachOperation, myOperations) {
    ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>(eachOperation);
    if (aFOperation) {
      FeaturePtr aFeature = aFOperation->feature();
      if(aFeature) {
        result << QString::fromStdString(aFeature->getKind());
      }
    }
  }
  return result;
}

ModuleBase_Operation* XGUI_OperationMgr::previousOperation(ModuleBase_Operation* theOperation) const
{
  int idx = myOperations.lastIndexOf(theOperation);
  if(idx == -1 || idx == 0) {
    return NULL;
  }
  return myOperations.at(idx - 1);
}

bool XGUI_OperationMgr::eventFilter(QObject *theObject, QEvent *theEvent)
{
  if (theEvent->type() == QEvent::KeyRelease) {
    QKeyEvent* aKeyEvent = dynamic_cast<QKeyEvent*>(theEvent);
    if(aKeyEvent) {
      return onKeyReleased(aKeyEvent);
    }
  }
  return QObject::eventFilter(theObject, theEvent);
}

bool XGUI_OperationMgr::startOperation(ModuleBase_Operation* theOperation)
{
  if (hasOperation())
    currentOperation()->postpone();
  myOperations.append(theOperation);

  connect(theOperation, SIGNAL(beforeStarted()), SLOT(onBeforeOperationStarted()));
  connect(theOperation, SIGNAL(beforeAborted()), SLOT(onBeforeOperationAborted()));
  connect(theOperation, SIGNAL(beforeCommitted()), SLOT(onBeforeOperationCommitted()));

  connect(theOperation, SIGNAL(started()), SLOT(onOperationStarted()));
  connect(theOperation, SIGNAL(aborted()), SLOT(onOperationAborted()));
  connect(theOperation, SIGNAL(committed()), SLOT(onOperationCommitted()));

  connect(theOperation, SIGNAL(stopped()), SLOT(onOperationStopped()));
  connect(theOperation, SIGNAL(resumed()), SLOT(onOperationResumed()));
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                                        (theOperation);
  if (aFOperation)
    connect(aFOperation, SIGNAL(activatedByPreselection()),
            SIGNAL(operationActivatedByPreselection()));

  bool isStarted = theOperation->start();
  if (isStarted)
    onValidateOperation();
  return isStarted;
}

bool XGUI_OperationMgr::abortAllOperations()
{
  bool aResult = true;
  if(!hasOperation())
    return aResult;

  if (operationsCount() == 1) {
    ModuleBase_Operation* aCurrentOperation = currentOperation();
    if (canStopOperation(aCurrentOperation)) {
      abortOperation(aCurrentOperation);
    }
    else
      aResult = false;
  }
  else {
    aResult = QMessageBox::question(qApp->activeWindow(),
                                    tr("Abort operation"),
                                    tr("All active operations will be aborted."),
                                    QMessageBox::Ok | QMessageBox::Cancel,
                                    QMessageBox::Cancel) == QMessageBox::Ok;
    while(aResult && hasOperation()) {
      abortOperation(currentOperation());
    }
  }
  return aResult;
}

bool XGUI_OperationMgr::commitAllOperations()
{
  bool isCompositeCommitted = false;
  while (hasOperation()) {
    ModuleBase_Operation* anOperation = currentOperation();
    if (isApplyEnabled()) {
      onCommitOperation();
    } else {
      abortOperation(anOperation);
    }
    ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                                            (anOperation);
    if (aFOperation) {
      FeaturePtr aFeature = aFOperation->feature();
      CompositeFeaturePtr aComposite = 
          std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aFeature);
      isCompositeCommitted = aComposite.get();
      if (isCompositeCommitted)
        break;
    }
  }
  return true;
}

void XGUI_OperationMgr::onValidateOperation()
{
  if (!hasOperation())
    return;
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                                          (currentOperation());
  if(aFOperation && aFOperation->feature().get()) {
    QString anError = myWorkshop->module()->getFeatureError(aFOperation->feature());
    if (anError.isEmpty()) {
      ModuleBase_IPropertyPanel* aPanel = aFOperation->propertyPanel();
      if (aPanel) {
        ModuleBase_ModelWidget* anActiveWidget = aPanel->activeWidget();
        if (anActiveWidget)
          anError = myWorkshop->module()->getWidgetError(anActiveWidget);
      }
    }
    setApplyEnabled(anError.isEmpty());
  }
}

void XGUI_OperationMgr::setApplyEnabled(const bool theEnabled)
{
  myIsApplyEnabled = theEnabled;
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                                          (currentOperation());
  if (aFOperation) {
    workshop()->errorMgr()->updateActions(aFOperation->feature());
  }
  //emit validationStateChanged(theEnabled);
}

void XGUI_OperationMgr::updateApplyOfOperations(ModuleBase_Operation* theOperation)
{
  XGUI_ErrorMgr* anErrorMgr = workshop()->errorMgr();
  if (theOperation) {
    ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>(theOperation);
    if (aFOperation)
      anErrorMgr->updateAcceptAllAction(aFOperation->feature());
    //emit nestedStateChanged(theOperation->getDescription()->operationId().toStdString(),
    //                        theOperation->isValid());
  }
  else {
    foreach(ModuleBase_Operation* anOperation, myOperations) {
      if (anOperation)
        updateApplyOfOperations(anOperation);
      //emit nestedStateChanged(anOperation->getDescription()->operationId().toStdString(),
      //                        anOperation->isValid());
    }
  }
}

bool XGUI_OperationMgr::isApplyEnabled() const
{
  return myIsApplyEnabled;
}

bool XGUI_OperationMgr::isParentOperationValid() const
{
  bool isValid = false;
  // the enable state of the parent operation of the nested one is defined by the rules that
  // firstly there are nested operations and secondly the parent operation is valid
  ModuleBase_Operation* aPrevOp = 0;
  Operations::const_iterator anIt = myOperations.end();
  if (anIt != myOperations.begin()) { // there are items in the operations list
    --anIt;
    aPrevOp = *anIt; // the last top operation, the operation which is started
    if (anIt != myOperations.begin()) { // find the operation where the started operation is nested
      --anIt;
      aPrevOp = *anIt;
    }
  }
  return aPrevOp && aPrevOp->isValid();
}

bool XGUI_OperationMgr::canStopOperation(ModuleBase_Operation* theOperation)
{
  //in case of nested (sketch) operation no confirmation needed
  if (isGrantedOperation(theOperation->id()))
    return true;
  if (theOperation && theOperation->isModified()) {
    QString aMessage = tr("%1 operation will be aborted.").arg(theOperation->id());
    int anAnswer = QMessageBox::question(qApp->activeWindow(),
                                         tr("Abort operation"),
                                         aMessage,
                                         QMessageBox::Ok | QMessageBox::Cancel,
                                         QMessageBox::Cancel);
    return anAnswer == QMessageBox::Ok;
  }
  return true;
}

bool XGUI_OperationMgr::commitOperation()
{
  if (hasOperation() && currentOperation()->isValid()) {
    onCommitOperation();
    return true;
  }
  return false;
}

void XGUI_OperationMgr::resumeOperation(ModuleBase_Operation* theOperation)
{
  theOperation->resume();
}

bool XGUI_OperationMgr::isGrantedOperation(const QString& theId)
{
  bool isGranted = false;

  QListIterator<ModuleBase_Operation*> anIt(myOperations);
  anIt.toBack();
  ModuleBase_Operation* aPreviousOperation = 0;
  while (anIt.hasPrevious() && !isGranted) {
    ModuleBase_Operation* anOp = anIt.previous();
    if (anOp)
      isGranted = anOp->isGranted(theId);
  }
  return isGranted;
}

void XGUI_OperationMgr::setCurrentFeature(const FeaturePtr& theFeature)
{
  SessionPtr aMgr = ModelAPI_Session::get();
  DocumentPtr aDoc = aMgr->activeDocument();
  bool aIsOp = aMgr->isOperation();
  if (!aIsOp)
    aMgr->startOperation();
  aDoc->setCurrentFeature(theFeature, false);
  if (!aIsOp)
    aMgr->finishOperation();
}

bool XGUI_OperationMgr::canStartOperation(const QString& theId)
{
  bool aCanStart = true;
  ModuleBase_Operation* aCurrentOp = currentOperation();
  if (aCurrentOp) {
    bool aGranted = aCurrentOp->isGranted(theId);
    // the started operation is granted for the current one,
    // e.g. current - Sketch, started - Line
    if (aGranted) {
      aCanStart = true;
    }
    else {
      if (!isGrantedOperation(theId)) {
        // the operation is not granted in the current list of operations
        // e.g. Edit Parameter when Sketch, Line in Sketch is active.
        aCanStart = abortAllOperations();
      }
      else if (canStopOperation(aCurrentOp)) {
        // the started operation is granted in the parrent operation,
        // e.g. current - Line in Sketch, started Circle 
        if (myIsApplyEnabled && aCurrentOp->isModified())
          aCurrentOp->commit();
        else
          abortOperation(aCurrentOp);
      } else {
        aCanStart = false;
      }
    }
  }
  return aCanStart;
}

void XGUI_OperationMgr::abortOperation(ModuleBase_Operation* theOperation)
{
  ModuleBase_Operation* aCurrentOperation = currentOperation();
  if (theOperation == aCurrentOperation)
    theOperation->abort();
  else {
    // it is possible to trigger upper operation(e.g. sketch, current is sketch line)
    // all operation from the current to triggered should also be aborted
    // operations over the parameter one are not aborted(e.g. extrusion cut, sketch abort)
    while(hasOperation()) {
      ModuleBase_Operation* aCurrentOperation = currentOperation();
      aCurrentOperation->abort();
      if(theOperation == aCurrentOperation)
        break;
    }
  }
}

void XGUI_OperationMgr::onCommitOperation()
{
  ModuleBase_Operation* anOperation = currentOperation();
  if (anOperation)
    anOperation->commit();
}

void XGUI_OperationMgr::onAbortOperation()
{
  ModuleBase_Operation* aCurrentOperation = currentOperation();
  if (aCurrentOperation && canStopOperation(aCurrentOperation)) {
    abortOperation(aCurrentOperation);
  }
}

void XGUI_OperationMgr::onBeforeOperationStarted()
{
  ModuleBase_Operation* aCurrentOperation = dynamic_cast<ModuleBase_Operation*>(sender());
  if (!aCurrentOperation)
    return;

  /// Set current feature and remeber old current feature
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>(aCurrentOperation);
  if (aFOperation) {
    SessionPtr aMgr = ModelAPI_Session::get();
    DocumentPtr aDoc = aMgr->activeDocument();
    // the parameter of current feature should be false, we should use all feature, not only visible
    // in order to correctly save the previous feature of the nested operation, where the
    // features can be not visible in the tree. The problem case is Edit sketch entitity(line)
    // in the Sketch, created in ExtrusionCut operation. The entity disappears by commit.
    // When sketch entity operation started, the sketch should be cashed here as the current.
    // Otherwise(the flag is true), the ExtrusionCut is cashed, when commit happens, the sketch
    // is disabled, sketch entity is disabled as extrusion cut is created earliest then sketch.
    // As a result the sketch disappears from the viewer. However after commit it is displayed back.
    aFOperation->setPreviousCurrentFeature(aDoc->currentFeature(false));

#ifdef DEBUG_CURRENT_FEATURE
    FeaturePtr aFeature = aFOperation->feature();
    QString aKind = aFeature ? aFeature->getKind().c_str() : "";
    qDebug(QString("onBeforeOperationStarted(), edit operation = %1, feature = %2")
            .arg(aFOperation->isEditOperation())
            .arg(ModuleBase_Tools::objectInfo(aFeature)).toStdString().c_str());

    qDebug(QString("\tdocument->currentFeature(false) = %1").arg(
            ModuleBase_Tools::objectInfo(ModelAPI_Session::get()->activeDocument()->currentFeature(false))).toStdString().c_str());
#endif

    if (aFOperation->isEditOperation()) // it should be performed by the feature edit only
      // in create operation, the current feature is changed by addFeature()
      aDoc->setCurrentFeature(aFOperation->feature(), false);

#ifdef DEBUG_CURRENT_FEATURE
    qDebug("\tdocument->setCurrentFeature");
    qDebug(QString("\tdocument->currentFeature(false) = %1").arg(
            ModuleBase_Tools::objectInfo(ModelAPI_Session::get()->activeDocument()->currentFeature(false))).toStdString().c_str());
#endif
  }
}

void XGUI_OperationMgr::onOperationStarted()
{
  ModuleBase_Operation* aSenderOperation = dynamic_cast<ModuleBase_Operation*>(sender());
  updateApplyOfOperations(aSenderOperation);
  emit operationStarted(aSenderOperation);
}

void XGUI_OperationMgr::onBeforeOperationAborted()
{
  onBeforeOperationCommitted();
}

void XGUI_OperationMgr::onOperationAborted()
{
  ModuleBase_Operation* aSenderOperation = dynamic_cast<ModuleBase_Operation*>(sender());
  emit operationAborted(aSenderOperation);
}

void XGUI_OperationMgr::onBeforeOperationCommitted()
{
  ModuleBase_Operation* aCurrentOperation = dynamic_cast<ModuleBase_Operation*>(sender());
  if (!aCurrentOperation)
    return;

  /// Restore the previous current feature
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>(aCurrentOperation);
  if (aFOperation) {
#ifdef DEBUG_CURRENT_FEATURE
    QString aKind = aFOperation->feature()->getKind().c_str();
    qDebug(QString("onBeforeOperationCommitted(), edit operation = %1, feature = %2")
            .arg(aFOperation->isEditOperation())
            .arg(ModuleBase_Tools::objectInfo(aFOperation->feature())).toStdString().c_str());

    qDebug(QString("\tdocument->currentFeature(false) = %1").arg(
            ModuleBase_Tools::objectInfo(ModelAPI_Session::get()->activeDocument()->currentFeature(false))).toStdString().c_str());
#endif

    if (aFOperation->isEditOperation()) {
      /// Restore the previous current feature
      setCurrentFeature(aFOperation->previousCurrentFeature());
    }
    else { // create operation
      // the Top created feature should stays the current. In nested operations, like Line in the Sketch or
      // Sketch in ExtrusionCut, a previous feature should be restored on commit. It is performed here
      // in order to perform it in the current transaction without opening a new one.
      if (myOperations.front() != aFOperation)
        setCurrentFeature(aFOperation->previousCurrentFeature());
    }
#ifdef DEBUG_CURRENT_FEATURE
    qDebug("\tdocument->setCurrentFeature");
    qDebug(QString("\tdocument->currentFeature(false) = %1").arg(
            ModuleBase_Tools::objectInfo(ModelAPI_Session::get()->activeDocument()->currentFeature(false))).toStdString().c_str());
#endif
  }
}

void XGUI_OperationMgr::onOperationCommitted()
{
  // apply state for all features from the stack of operations should be updated
  updateApplyOfOperations();

  ModuleBase_Operation* aSenderOperation = dynamic_cast<ModuleBase_Operation*>(sender());
  emit operationCommitted(aSenderOperation);
}

void XGUI_OperationMgr::onOperationResumed()
{
  ModuleBase_Operation* aSenderOperation = dynamic_cast<ModuleBase_Operation*>(sender());
  emit operationResumed(aSenderOperation);
}

void XGUI_OperationMgr::onOperationStopped()
{
  ModuleBase_Operation* aSenderOperation = dynamic_cast<ModuleBase_Operation*>(sender());
  ModuleBase_Operation* aCurrentOperation = currentOperation();
  if (!aSenderOperation || !aCurrentOperation || aSenderOperation != aCurrentOperation)
    return;

  myOperations.removeAll(aCurrentOperation);
  aCurrentOperation->deleteLater();

  emit operationStopped(aCurrentOperation);

  // get last operation which can be resumed
  ModuleBase_Operation* aResultOp = 0;
  QListIterator<ModuleBase_Operation*> anIt(myOperations);
  anIt.toBack();
  while (anIt.hasPrevious()) {
    ModuleBase_Operation* anOp = anIt.previous();
    if (anOp) {
      aResultOp = anOp;
      break;
    }
  }
  if (aResultOp) {
    bool isModified = aCurrentOperation->isModified();
    aResultOp->setIsModified(aResultOp->isModified() || isModified);
    resumeOperation(aResultOp);
    onValidateOperation();
  }
}

bool XGUI_OperationMgr::onKeyReleased(QKeyEvent* theEvent)
{
  QObject* aSender = sender();

  // Let the manager decide what to do with the given key combination.
  ModuleBase_Operation* anOperation = currentOperation();
  bool isAccepted = true;
  switch (theEvent->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter: {
      isAccepted = onProcessEnter();
      /*ModuleBase_Operation* aOperation = currentOperation();
      ModuleBase_IPropertyPanel* aPanel = aOperation->propertyPanel();
      ModuleBase_ModelWidget* aActiveWgt = aPanel->activeWidget();
      if (!aActiveWgt || !aActiveWgt->processEnter()) {
        if (!myWorkshop->module()->processEnter(aActiveWgt ? aActiveWgt->attributeID() : "")) {
          ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>(currentOperation());
          if (!aFOperation || myWorkshop->module()->getFeatureError(aFOperation->feature()).isEmpty()) {
            emit keyEnterReleased();
            commitOperation();
          }
          else
            isAccepted = false;
        }
      }*/
    }
    break;
    case Qt::Key_N:
    case Qt::Key_P: {
      bool noModifiers = (theEvent->modifiers() == Qt::NoModifier);
      if (noModifiers) {
        ModuleBase_IViewer* aViewer = myWorkshop->viewer();
        Handle(AIS_InteractiveContext) aContext = aViewer->AISContext();
        if (!aContext.IsNull()) {
          Handle(V3d_View) aView = aViewer->activeView();
          if ((theEvent->key() == Qt::Key_N))
            aContext->HilightNextDetected(aView);
          else if ((theEvent->key() == Qt::Key_P))
            aContext->HilightPreviousDetected(aView);
        }
      }
    }

    break;
    default:
      isAccepted = false;
      break;
  }
  //if(anOperation) {
  //  anOperation->keyReleased(theEvent->key());
  //}
  return isAccepted;
}

bool XGUI_OperationMgr::onProcessEnter()
{
  bool isAccepted = true;
  ModuleBase_Operation* aOperation = currentOperation();
  ModuleBase_IPropertyPanel* aPanel = aOperation->propertyPanel();
  ModuleBase_ModelWidget* aActiveWgt = aPanel->activeWidget();
  if (!aActiveWgt || !aActiveWgt->processEnter()) {
    if (!myWorkshop->module()->processEnter(aActiveWgt ? aActiveWgt->attributeID() : "")) {
      ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>(currentOperation());
      if (!aFOperation || myWorkshop->module()->getFeatureError(aFOperation->feature()).isEmpty()) {
        emit keyEnterReleased();
        commitOperation();
      }
      else
        isAccepted = false;
    }
  }
  return isAccepted;
}

XGUI_Workshop* XGUI_OperationMgr::workshop() const
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myWorkshop);
  return aConnector->workshop();
}

