// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "XGUI_OperationMgr.h"
#include "XGUI_ModuleConnector.h"
#include "XGUI_Workshop.h"
#include "XGUI_ErrorMgr.h"
#include "XGUI_Tools.h"
#include "XGUI_ObjectsBrowser.h"
#include "XGUI_ContextMenuMgr.h"

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

#include <XGUI_PropertyPanel.h>
#include <QToolButton>
#include <QLineEdit>

#include <QMessageBox>
#include <QApplication>
#include <QKeyEvent>

//#define DEBUG_CURRENT_FEATURE

/// Processes "Delete" key event of application. This key is used by several application actions.
/// There is a logical order of the actions processing. So the key can not be set for actions
/// as a shortcut. The class listens the key event and call operation manager processor.
class XGUI_ShortCutListener : public QObject
{
public:
  /// Constructor
  /// \param theParent the parent to be deleted when the parent is deleted
  /// \param theOperationMgr the class to perform deletion
  XGUI_ShortCutListener(QObject* theParent, XGUI_OperationMgr* theOperationMgr)
    : QObject(theParent), myOperationMgr(theOperationMgr)
  {
    qApp->installEventFilter(this);
  }
  ~XGUI_ShortCutListener() {}

  /// Switch on short cut listener
  void setActive(const bool theIsActive) { myIsActive = theIsActive; }

  /// Redefinition of virtual function to process Delete key release
  virtual bool eventFilter(QObject *theObject, QEvent *theEvent)
  {
    bool isAccepted = false;
    if (myIsActive && theEvent->type() == QEvent::KeyRelease) {
      QKeyEvent* aKeyEvent = dynamic_cast<QKeyEvent*>(theEvent);
      if (aKeyEvent) {
        switch (aKeyEvent->key()) {
          case Qt::Key_Delete:
            isAccepted = myOperationMgr->onProcessDelete(theObject);
          break;
          default:
            isAccepted = myOperationMgr->onKeyReleased(theObject, aKeyEvent);
            break;
        }
      }
    }
    if (!isAccepted)
      isAccepted = QObject::eventFilter(theObject, theEvent);
    return isAccepted;
  }

private:
  XGUI_OperationMgr* myOperationMgr; /// processor for key event
  bool myIsActive; /// boolean state whether the event filter perform own signal processing
};

XGUI_OperationMgr::XGUI_OperationMgr(QObject* theParent,
                                     ModuleBase_IWorkshop* theWorkshop)
: QObject(theParent), myWorkshop(theWorkshop)
{
  /// we need to install filter to the application in order to react to 'Delete' key button
  /// this key can not be a short cut for a corresponded action because we need to set
  /// the actions priority
  myShortCutListener = new XGUI_ShortCutListener(theParent, this);
}

XGUI_OperationMgr::~XGUI_OperationMgr()
{
}

void XGUI_OperationMgr::activate()
{
  myShortCutListener->setActive(true);
}

void XGUI_OperationMgr::deactivate()
{
  myShortCutListener->setActive(false);
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
  QList<ModuleBase_Operation*>::const_iterator anIt = myOperations.end();
  while (anIt != myOperations.begin()) {
    --anIt;
    ModuleBase_Operation* anOperation = *anIt;
    if (anOperation->id() == theId)
      return anOperation;
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
    ModuleBase_OperationFeature* aFOperation =
      dynamic_cast<ModuleBase_OperationFeature*>(eachOperation);
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
  bool isCompositeCommitted = false, anOperationProcessed = false;
  while (hasOperation()) {
    ModuleBase_Operation* anOperation = currentOperation();
    if (XGUI_Tools::workshop(myWorkshop)->errorMgr()->isApplyEnabled()) {
      anOperationProcessed = onCommitOperation();
    } else {
      abortOperation(anOperation);
      anOperationProcessed = true;
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
    // not processed[committed] operation might be used in composite feature,
    // so the while will be stopped by the previous check.
    // this code is not necessary, but logically should be done when the processing will not
    // be done for not composite feature by some reasons
    if (!anOperationProcessed)
      break;
  }
  return true;
}

void XGUI_OperationMgr::onValidateOperation()
{
  if (!hasOperation())
    return;
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                                          (currentOperation());
  if(aFOperation && aFOperation->feature().get())
    XGUI_Tools::workshop(myWorkshop)->errorMgr()->updateActions(aFOperation->feature());
}

void XGUI_OperationMgr::updateApplyOfOperations(ModuleBase_Operation* theOperation)
{
  XGUI_ErrorMgr* anErrorMgr = XGUI_Tools::workshop(myWorkshop)->errorMgr();
  if (theOperation) {
    ModuleBase_OperationFeature* aFOperation =
      dynamic_cast<ModuleBase_OperationFeature*>(theOperation);
    if (aFOperation)
      anErrorMgr->updateAcceptAllAction(aFOperation->feature());
  }
  else {
    foreach(ModuleBase_Operation* anOperation, myOperations) {
      if (anOperation)
        updateApplyOfOperations(anOperation);
    }
  }
  // Apply button of the current operation should also be updated
  onValidateOperation();
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
  //if (hasOperation() && currentOperation()->isValid()) {
  //  onCommitOperation();
  //  return true;
  //}
  //return false;
  return onCommitOperation();
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
    aMgr->startOperation(QString("Set current feature: %1")
    .arg(theFeature->getKind().c_str()).toStdString());
  aDoc->setCurrentFeature(theFeature, false);
#ifdef DEBUG_CURRENT_FEATURE
  qDebug(QString("   document->setCurrentFeature(false) = %1    SET").arg(
         ModuleBase_Tools::objectName(
         ModelAPI_Session::get()->activeDocument()->currentFeature(false))).toStdString().c_str());
#endif

  if (!aIsOp)
    aMgr->finishOperation();
}

bool XGUI_OperationMgr::canStartOperation(const QString& theId, bool& isCommitted)
{
  bool aCanStart = true;
  isCommitted = false;
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
        stopOperation(aCurrentOp, isCommitted);
      } else {
        aCanStart = false;
      }
    }
  }
  return aCanStart;
}

void XGUI_OperationMgr::stopOperation(ModuleBase_Operation* theOperation, bool& isCommitted)
{
  if (XGUI_Tools::workshop(myWorkshop)->errorMgr()->isApplyEnabled() && theOperation->isModified())
    isCommitted = theOperation->commit();
  else {
    isCommitted = false;
    abortOperation(theOperation);
  }
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

bool XGUI_OperationMgr::onCommitOperation()
{
  bool isCommitted = false;
  ModuleBase_Operation* anOperation = currentOperation();
  if (anOperation && myWorkshop->module()->canCommitOperation())
    isCommitted = anOperation->commit();
  return isCommitted;
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
  ModuleBase_OperationFeature* aFOperation =
    dynamic_cast<ModuleBase_OperationFeature*>(aCurrentOperation);
  if (aFOperation) {
    SessionPtr aMgr = ModelAPI_Session::get();
    DocumentPtr aDoc = aMgr->activeDocument();
    // the parameter of current feature should be false, we should use all feature, not only
    // visible in order to correctly save the previous feature of the nested operation, where the
    // features can be not visible in the tree. The problem case is Edit sketch entitity(line)
    // in the Sketch, created in ExtrusionCut operation. The entity disappears by commit.
    // When sketch entity operation started, the sketch should be cashed here as the current.
    // Otherwise(the flag is true), the ExtrusionCut is cashed, when commit happens, the sketch
    // is disabled, sketch entity is disabled as extrusion cut is created earliest then sketch.
    // As a result the sketch disappears from the viewer.
    // However after commit it is displayed back.
    aFOperation->setPreviousCurrentFeature(aDoc->currentFeature(false));

#ifdef DEBUG_CURRENT_FEATURE
    FeaturePtr aFeature = aFOperation->feature();
    QString aKind = aFeature ? aFeature->getKind().c_str() : "";
    qDebug("");
    qDebug(QString("onBeforeOperationStarted() isEditOperation = %1, feature = %2")
            .arg(aFOperation->isEditOperation())
            .arg(ModuleBase_Tools::objectName(aFeature)).toStdString().c_str());
    qDebug(QString("   document->currentFeature(false) = %1 : DO: setPreviousCurrentFeature").arg(
            ModuleBase_Tools::objectName(aDoc->currentFeature(false))).toStdString().c_str());
#endif

    if (aFOperation->isEditOperation()) {// it should be performed by the feature edit only
      // in create operation, the current feature is changed by addFeature()
      aDoc->setCurrentFeature(aFOperation->feature(), false);
#ifdef DEBUG_CURRENT_FEATURE
      qDebug(QString("   document->setCurrentFeature(false) = %1").arg(
             ModuleBase_Tools::objectName(aDoc->currentFeature(false))).toStdString().c_str());
#endif
      // this is the only place where flushes must be called after setCurrentFeature for the
      // current moment: after this the opertion is not finished, so, the ObjectBrowser
      // state may be corrupted (issue #1457)
      static Events_Loop* aLoop = Events_Loop::loop();
      static Events_ID aCreateEvent = aLoop->eventByName(EVENT_OBJECT_CREATED);
      aLoop->flush(aCreateEvent);
      static Events_ID aDeleteEvent = aLoop->eventByName(EVENT_OBJECT_DELETED);
      aLoop->flush(aDeleteEvent);
    }
  }
}

void XGUI_OperationMgr::onOperationStarted()
{
  ModuleBase_Operation* aSenderOperation = dynamic_cast<ModuleBase_Operation*>(sender());
  updateApplyOfOperations(aSenderOperation);
  XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(myWorkshop);
  aWorkshop->operationStarted(aSenderOperation);
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
  ModuleBase_OperationFeature* aFOperation =
    dynamic_cast<ModuleBase_OperationFeature*>(aCurrentOperation);
  if (aFOperation) {
#ifdef DEBUG_CURRENT_FEATURE
    QString aKind = aFOperation->feature()->getKind().c_str();
    qDebug(QString("onBeforeOperationCommitted() isEditOperation = %1, feature = %2")
            .arg(aFOperation->isEditOperation())
            .arg(ModuleBase_Tools::objectName(aFOperation->feature())).toStdString().c_str());
    qDebug(QString("   document->currentFeature(false) = %1").arg(
            ModuleBase_Tools::objectName(
            ModelAPI_Session::get()->activeDocument()->currentFeature(false)))
            .toStdString().c_str());
#endif

    if (aFOperation->isEditOperation()) {
      /// Restore the previous current feature
      setCurrentFeature(aFOperation->previousCurrentFeature());
    }
    else { // create operation
      // the Top created feature should stays the current. In nested operations,
      // like Line in the Sketch or
      // Sketch in ExtrusionCut, a previous feature should be restored on commit.
      // It is performed here
      // in order to perform it in the current transaction without opening a new one.
      if (myOperations.front() != aFOperation)
        setCurrentFeature(aFOperation->previousCurrentFeature());
    }
    ModuleBase_IModule* aModule = myWorkshop->module();
    if (aModule)
      aModule->beforeOperationStopped(aFOperation);
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

bool XGUI_OperationMgr::onKeyReleased(QObject *theObject, QKeyEvent* theEvent)
{
  // Let the manager decide what to do with the given key combination.
  ModuleBase_Operation* anOperation = currentOperation();
  bool isAccepted = false;
  switch (theEvent->key()) {
    case Qt::Key_Escape: {
      ModuleBase_Operation* aOperation = currentOperation();
      if (aOperation) {
        onAbortOperation();
        isAccepted = true;
      }
    }
    break;
    case Qt::Key_Tab:
    case Qt::Key_Backtab:
    {
      ModuleBase_Operation* aOperation = currentOperation();
      if (aOperation) {
        ModuleBase_IPropertyPanel* aPanel = anOperation->propertyPanel();
        if (aPanel) {
          QWidget* aFocusedWidget = qApp->focusWidget();
          bool isPPChildObject = aFocusedWidget && isChildObject(aFocusedWidget, aPanel);
          if (!isPPChildObject) {
            // check for case when the operation is started but property panel is not filled
            XGUI_PropertyPanel* aPP = dynamic_cast<XGUI_PropertyPanel*>(aPanel);
            aPP->setFocusNextPrevChild(theEvent->key() == Qt::Key_Tab);
            isAccepted = true;
          }
        }
      }
    }
    break;
    case Qt::Key_Return:
    case Qt::Key_Enter: {
      isAccepted = onProcessEnter(theObject);
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

bool XGUI_OperationMgr::onProcessEnter(QObject* theObject)
{
  bool isAccepted = false;
  ModuleBase_Operation* aOperation = currentOperation();
  // to avoid enter processing when operation has not been started yet
  if (!aOperation)
    return isAccepted;
  ModuleBase_IPropertyPanel* aPanel = aOperation->propertyPanel();
  // the next code is obsolete as we want to process Enter in property panel always
  // only property panel enter is processed in order to do not process enter in application dialogs
  //bool isPPChild = isChildObject(theObject, aPanel);
  //if (!isPPChild)
  //  return isAccepted;

  ModuleBase_ModelWidget* anActiveWgt = aPanel->activeWidget();
  bool isAborted = false;
  if (!anActiveWgt) {
    QWidget* aFocusWidget = aPanel->focusWidget();
    QToolButton* aCancelBtn =
      dynamic_cast<XGUI_PropertyPanel*>(aPanel)->findButton(PROP_PANEL_CANCEL);
    if (aFocusWidget && aCancelBtn && aFocusWidget == aCancelBtn) {
      abortOperation(aOperation);
      isAccepted = true;
      isAborted = true;
    }
  }
  if (!isAborted) {
    isAccepted = anActiveWgt && anActiveWgt->processEnter();
    if (!isAccepted) {
      isAccepted =
        myWorkshop->module()->processEnter(anActiveWgt ? anActiveWgt->attributeID() : "");
      if (!isAccepted) {
        /// functionality is similar to Apply click
        ModuleBase_OperationFeature* aFOperation =
          dynamic_cast<ModuleBase_OperationFeature*>(currentOperation());
        if (!aFOperation ||
            myWorkshop->module()->getFeatureError(aFOperation->feature()).isEmpty()) {
          // key released is emitted to apply the current value to the model
          // if it was modified in PP
          emit keyEnterReleased();
          commitOperation();
          isAccepted = true;
        }
        else
          isAccepted = false;
      }
    }
  }
  return isAccepted;
}

bool editorControl(QObject* theObject)
{
  QLineEdit* aLineEdit = dynamic_cast<QLineEdit*>(theObject);
  return aLineEdit;
}

bool XGUI_OperationMgr::onProcessDelete(QObject* theObject)
{
  bool isAccepted = false;
  ModuleBase_Operation* aOperation = currentOperation();
  ModuleBase_ModelWidget* anActiveWgt = 0;
  // firstly the widget should process Delete action
  ModuleBase_IPropertyPanel* aPanel;
  bool isPPChildObject = false;
  if (aOperation) {
    aPanel = aOperation->propertyPanel();
    if (aPanel) {
      isPPChildObject = isChildObject(theObject, aPanel);
      // process delete in active widget only if delete sender is child of property panel
      // it is necessary for the case when OB is shown, user perform selection and click Delete
      if (isPPChildObject) {
        anActiveWgt = aPanel->activeWidget();
        if (anActiveWgt) {
          isAccepted = anActiveWgt->processDelete();
        }
      }
    }
  }
  if (!isAccepted) {
    // after widget, object browser and viewer should process delete
    /// other widgets such as line edit controls should not lead to
    /// processing delete by workshop
    XGUI_ObjectsBrowser* aBrowser = XGUI_Tools::workshop(myWorkshop)->objectBrowser();
    QWidget* aViewPort = myWorkshop->viewer()->activeViewPort();
    bool isToDeleteObject = false;
    XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(myWorkshop);
    XGUI_ContextMenuMgr* aContextMenuMgr = aWorkshop->contextMenuMgr();
    if (theObject == aBrowser->treeView()) {
      aContextMenuMgr->updateObjectBrowserMenu();
      isToDeleteObject = aContextMenuMgr->action("DELETE_CMD")->isEnabled();
    }
    else if (isChildObject(theObject, aViewPort)) {
      aContextMenuMgr->updateViewerMenu();
      isToDeleteObject = aContextMenuMgr->action("DELETE_CMD")->isEnabled();
    }
    else if (isPPChildObject) {
      // property panel child object is processed to process delete performed on Apply button of PP
      isToDeleteObject = true;
    }
    else if (editorControl(theObject)) {
      isToDeleteObject = false; /// Line Edit of Rename operation in ObjectBrowser
      isAccepted = true;
    }

    if (isToDeleteObject) {
      aWorkshop->deleteObjects();
      isAccepted = true;
    }
  }

  return isAccepted;
}

bool XGUI_OperationMgr::isChildObject(const QObject* theObject, const QObject* theParent)
{
  bool isPPChild = false;
  if (theParent && theObject) {
    QObject* aParent = (QObject*)theObject;
    while (aParent ) {
      isPPChild = aParent == theParent;
      if (isPPChild)
        break;
      aParent = aParent->parent();
    }
  }
  return isPPChild;
}
