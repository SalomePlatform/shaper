// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ModuleBase_OperationFeature.cpp
 *
 *  Created on: Apr 2, 2014
 *      Author: sbh
 */

#include "ModuleBase_OperationFeature.h"

#include "ModuleBase_OperationDescription.h"
#include "ModuleBase_ModelWidget.h"
#include "ModuleBase_ViewerPrs.h"
#include "ModuleBase_IPropertyPanel.h"
#include "ModuleBase_ISelection.h"
#include "ModuleBase_IViewer.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>

#include <GeomAPI_Pnt2d.h>

#include <Events_Loop.h>

#include <QTimer>

#ifdef _DEBUG
#include <QDebug>
#endif

ModuleBase_OperationFeature::ModuleBase_OperationFeature(const QString& theId, QObject* theParent)
: ModuleBase_Operation(theId, theParent),
  myIsEditing(false)
{
}

ModuleBase_OperationFeature::~ModuleBase_OperationFeature()
{
  clearPreselection();
}

void ModuleBase_OperationFeature::setEditOperation(const bool theRestartTransaction)
{
  if (isEditOperation())
    return;

  myIsEditing = true;
  if (theRestartTransaction) {
    SessionPtr aMgr = ModelAPI_Session::get();
    ModelAPI_Session::get()->finishOperation();

    QString anId = getDescription()->operationId();
    if (myIsEditing) {
      anId = anId.append(EditSuffix());
    }
    ModelAPI_Session::get()->startOperation(anId.toStdString());
    emit beforeStarted();
  }

  propertyPanel()->setEditingMode(isEditOperation());
}

FeaturePtr ModuleBase_OperationFeature::feature() const
{
  return myFeature;
}

bool ModuleBase_OperationFeature::isValid() const
{
  if (!myFeature || !myFeature->data()->isValid())
    return true; // rename operation
  if (myFeature->isAction())
    return true;
  //Get validators for the Id
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  bool aValid = aFactory->validate(myFeature);

  // the feature exec state should be checked in order to do not apply features, which result can not
  // be built. E.g. extrusion on sketch, where the "to" is a perpendicular plane to the sketch
  bool isDone = ( myFeature->data()->execState() == ModelAPI_StateDone
               || myFeature->data()->execState() == ModelAPI_StateMustBeUpdated );

  return aValid && isDone;
}

void ModuleBase_OperationFeature::startOperation()
{
  FeaturePtr aFeature = feature();
  if (!aFeature.get() || !isEditOperation())
    return;

  if (aFeature.get() && isEditOperation())
    aFeature->setStable(false);

  myVisualizedObjects.clear();
  // store hidden result features
  std::list<ResultPtr> aResults = aFeature->results();
  std::list<ResultPtr>::const_iterator aIt;
  for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
    ObjectPtr anObject = *aIt;
    if (anObject.get() && !anObject->isDisplayed()) {
      myVisualizedObjects.insert(*aIt);
      anObject->setDisplayed(true);
    }
  }
  if (!aFeature->isDisplayed()) {
    myVisualizedObjects.insert(aFeature);
    aFeature->setDisplayed(true);
  }
  Events_Loop::loop()->flush(Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY));
}

void ModuleBase_OperationFeature::stopOperation()
{
  FeaturePtr aFeature = feature();
  if (!aFeature.get() || !isEditOperation())
    return;

  // store hidden result features
  std::list<ResultPtr> aResults = aFeature->results();
  std::list<ResultPtr>::const_iterator aIt;
  for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
    ObjectPtr anObject = *aIt;
    if (anObject.get() && myVisualizedObjects.find(anObject) != myVisualizedObjects.end()) {
      anObject->setDisplayed(false);
    }
  }
  if (myVisualizedObjects.find(aFeature) != myVisualizedObjects.end()) {
    aFeature->setDisplayed(false);
  }
  if (myVisualizedObjects.size() > 0)
    Events_Loop::loop()->flush(Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY));
}

FeaturePtr ModuleBase_OperationFeature::createFeature(const bool theFlushMessage)
{
  if (myParentFeature.get()) {
    myFeature = myParentFeature->addFeature(getDescription()->operationId().toStdString());
  } else {
    std::shared_ptr<ModelAPI_Document> aDoc = ModelAPI_Session::get()->activeDocument();
    myFeature = aDoc->addFeature(getDescription()->operationId().toStdString());
  }
  if (myFeature) {  // TODO: generate an error if feature was not created
    setIsModified(true);
    // Model update should call "execute" of a feature.
    //myFeature->execute();
    // Init default values
    /*QList<ModuleBase_ModelWidget*> aWidgets = getDescription()->modelWidgets();
     QList<ModuleBase_ModelWidget*>::const_iterator anIt = aWidgets.begin(), aLast = aWidgets.end();
     for (; anIt != aLast; anIt++) {
     (*anIt)->storeValue(aFeature);
     }*/
  }

  if (theFlushMessage)
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  return myFeature;
}

void ModuleBase_OperationFeature::setFeature(FeaturePtr theFeature)
{
  myFeature = theFeature;
  myIsEditing = true;
}

bool ModuleBase_OperationFeature::hasObject(ObjectPtr theObj) const
{
  FeaturePtr aFeature = feature();
  if (aFeature) {
    if (aFeature == theObj)
      return true;
    std::list<ResultPtr> aResults = aFeature->results();
    std::list<ResultPtr>::const_iterator aIt;
    for (aIt = aResults.cbegin(); aIt != aResults.cend(); ++aIt) {
      if (theObj == (*aIt))
        return true;
    }
  }
  return false;
}

bool ModuleBase_OperationFeature::isDisplayedOnStart(ObjectPtr theObject)
{
  return myVisualizedObjects.find(theObject) != myVisualizedObjects.end();
}

void ModuleBase_OperationFeature::start()
{
  setIsModified(false);
  QString anId = getDescription()->operationId();
  if (myIsEditing) {
    anId = anId.append(EditSuffix());
  }
  ModelAPI_Session::get()->startOperation(anId.toStdString());

  emit beforeStarted();
  startOperation();

  if (!myIsEditing) {
    FeaturePtr aFeature = createFeature();
    // if the feature is not created, there is no sense to start the operation
    if (aFeature.get() == NULL) {
      // it is necessary to abor the operation in the session and emit the aborted signal
      // in order to update commands status in the workshop, to be exact the feature action
      // to be unchecked
      abort();
      return;
    }
  }
  //Already called startOperation();
  emit started();

}

void ModuleBase_OperationFeature::abort()
{
  emit beforeAborted();

  // the viewer update should be blocked in order to avoid the features blinking before they are
  // hidden
  std::shared_ptr<Events_Message> aMsg = std::shared_ptr<Events_Message>(
      new Events_Message(Events_Loop::eventByName(EVENT_UPDATE_VIEWER_BLOCKED)));
  Events_Loop::loop()->send(aMsg);

  // the widgets of property panel should not process any events come from data mode
  // after abort clicked. Some signal such as redisplay/create influence on content
  // of the object browser and viewer context. Therefore it influence to the current
  // selection and if the active widget listens it, the attribute value is errnoneous
  // changed.
  ModuleBase_IPropertyPanel* aPropertyPanel = propertyPanel();
  if (aPropertyPanel)
    aPropertyPanel->cleanContent();

  myFeature->setStable(true);

  abortOperation();
  stopOperation();

  SessionPtr aMgr = ModelAPI_Session::get();
  aMgr->abortOperation();
  emit stopped();
  // the viewer update should be unblocked in order to avoid the features blinking before they are
  // hidden
  aMsg = std::shared_ptr<Events_Message>(
                new Events_Message(Events_Loop::eventByName(EVENT_UPDATE_VIEWER_UNBLOCKED)));

  Events_Loop::loop()->send(aMsg);

  emit aborted();
}

bool ModuleBase_OperationFeature::commit()
{
  if (canBeCommitted()) {
    emit beforeCommitted();
    // the widgets of property panel should not process any events come from data mode
    // after commit clicked. Some signal such as redisplay/create influence on content
    // of the object browser and viewer context. Therefore it influence to the current
    // selection and if the active widget listens it, the attribute value is errnoneous
    // changed.
    ModuleBase_IPropertyPanel* aPropertyPanel = propertyPanel();
    if (aPropertyPanel)
      aPropertyPanel->cleanContent();

    myFeature->setStable(true);

    SessionPtr aMgr = ModelAPI_Session::get();
    /// Set current feature and remeber old current feature

    commitOperation();
    aMgr->finishOperation();

    stopOperation();
    emit stopped();
    emit committed();

    afterCommitOperation();
    return true;
  }
  return false;
}

void ModuleBase_OperationFeature::activateByPreselection()
{
  if (myPreSelection.empty())
    return;

  ModuleBase_ISelection::filterSelectionOnEqualPoints(myPreSelection);

  ModuleBase_ModelWidget* aFilledWgt = 0;
  ModuleBase_IPropertyPanel* aPropertyPanel = propertyPanel();
  if (aPropertyPanel) {
    const QList<ModuleBase_ModelWidget*>& aWidgets = aPropertyPanel->modelWidgets();
    if (!aWidgets.empty()) {
      ModuleBase_ModelWidget* aWgt = 0;
      QList<ModuleBase_ModelWidget*>::const_iterator aWIt;
      bool isSet = false;
      // 1. apply the selection to controls
      for (aWIt = aWidgets.constBegin(); aWIt != aWidgets.constEnd(); ++aWIt) {
        aWgt = (*aWIt);
        if (!aWgt->canSetValue())
          continue;
        aPropertyPanel->setPreselectionWidget(aWgt);
        if (!aWgt->setSelection(myPreSelection, true)) {
          isSet = false;
          break;
        } else {
          isSet = true;
          aFilledWgt = aWgt;
        }
      }
      aPropertyPanel->setPreselectionWidget(NULL);
      // in order to redisplay object in the viewer, the update/redisplay signals should be flushed
      // it is better to perform it not in setSelection of each widget, but do it here,
      // after the preselection is processed
      ModuleBase_ModelWidget::updateObject(myFeature);

      // 3. a signal should be emitted before the next widget activation
      // because, the activation of the next widget will give a focus to the widget. As a result
      // the value of the widget is initialized. And commit may happens until the value is entered.
      if (aFilledWgt)
        emit activatedByPreselection();
    }
    // 4. activate the next obligatory widget
    aPropertyPanel->activateNextWidget(aFilledWgt);
  }

  clearPreselection();
}

void ModuleBase_OperationFeature::setParentFeature(CompositeFeaturePtr theParent)
{
  myParentFeature = theParent;
}

CompositeFeaturePtr ModuleBase_OperationFeature::parentFeature() const
{
  return myParentFeature;
}

void ModuleBase_OperationFeature::setPreviousCurrentFeature(const FeaturePtr& theFeature)
{
  myPreviousCurrentFeature = theFeature;
}

FeaturePtr ModuleBase_OperationFeature::previousCurrentFeature()
{
  return myPreviousCurrentFeature;
}

void ModuleBase_OperationFeature::initSelection(ModuleBase_ISelection* theSelection,
                                         ModuleBase_IViewer* theViewer)
{
  clearPreselection();

  QList<ModuleBase_ViewerPrs> aPreSelected;
  // Check that the selected result are not results of operation feature
  FeaturePtr aFeature = feature();
  if (aFeature) {
    QList<ModuleBase_ViewerPrs> aSelected = theSelection->getSelected(ModuleBase_ISelection::AllControls);

    std::list<ResultPtr> aResults = aFeature->results();
    QObjectPtrList aResList;
    std::list<ResultPtr>::const_iterator aIt;
    for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt)
      aResList.append(*aIt);

    foreach (ModuleBase_ViewerPrs aPrs, aSelected) {
      if ((!aResList.contains(aPrs.object())) && (aPrs.object() != aFeature))
        aPreSelected.append(aPrs);
    }
  } else
    aPreSelected = theSelection->getSelected(ModuleBase_ISelection::AllControls);

  myPreSelection = aPreSelected;
}

void ModuleBase_OperationFeature::clearPreselection()
{
  myPreSelection.clear();
}

void ModuleBase_OperationFeature::setPropertyPanel(ModuleBase_IPropertyPanel* theProp) 
{
  ModuleBase_Operation::setPropertyPanel(theProp);

  theProp->setEditingMode(isEditOperation());

  if (theProp) {
    const QList<ModuleBase_ModelWidget*>& aWidgets = theProp->modelWidgets();
    QList<ModuleBase_ModelWidget*>::const_iterator aWIt;
    for (aWIt = aWidgets.constBegin(); aWIt != aWidgets.constEnd(); ++aWIt) {
      ModuleBase_ModelWidget* aWgt = (*aWIt);
      connect(aWgt, SIGNAL(valuesChanged()), this, SLOT(onValuesChanged()));
    }
  }

  // Do not activate widgets by default if the current operation is editing operation
  // Because we don't know which widget is going to be edited. 
  if (!isEditOperation()) {
    // 4. activate the first obligatory widget
    theProp->activateNextWidget(NULL);
  }
  else {
    // set focus on Ok button in order to operation manager could process Enter press
    if (theProp)
      theProp->setFocusOnOkButton();
  }
}
