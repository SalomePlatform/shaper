// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ModuleBase_Operation.cpp
 *
 *  Created on: Apr 2, 2014
 *      Author: sbh
 */

#include "ModuleBase_Operation.h"

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

#include <GeomAPI_Pnt2d.h>

#include <Events_Loop.h>

#include <QTimer>

#ifdef _DEBUG
#include <QDebug>
#endif

ModuleBase_Operation::ModuleBase_Operation(const QString& theId, QObject* theParent)
    : QObject(theParent),
      myIsEditing(false),
      myIsModified(false),
      myPropertyPanel(NULL)
{
  myDescription = new ModuleBase_OperationDescription(theId);
}

ModuleBase_Operation::~ModuleBase_Operation()
{
  delete myDescription;
  clearPreselection();
}

QString ModuleBase_Operation::id() const
{
  return getDescription()->operationId();
}

FeaturePtr ModuleBase_Operation::feature() const
{
  return myFeature;
}

bool ModuleBase_Operation::isValid() const
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


bool ModuleBase_Operation::canBeCommitted() const
{
  return isValid();
}

FeaturePtr ModuleBase_Operation::createFeature(const bool theFlushMessage)
{
  if (myParentFeature.get()) {
    myFeature = myParentFeature->addFeature(getDescription()->operationId().toStdString());
  } else {
    std::shared_ptr<ModelAPI_Document> aDoc = ModelAPI_Session::get()->activeDocument();
    myFeature = aDoc->addFeature(getDescription()->operationId().toStdString());
  }
  if (myFeature) {  // TODO: generate an error if feature was not created
    myIsModified = true;
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

void ModuleBase_Operation::setFeature(FeaturePtr theFeature)
{
  myFeature = theFeature;
  myIsEditing = true;
}

bool ModuleBase_Operation::hasObject(ObjectPtr theObj) const
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

void ModuleBase_Operation::start()
{
  myIsModified = false;
  QString anId = getDescription()->operationId();
  if (myIsEditing) {
    anId = anId.append(EditSuffix());
  }
  ModelAPI_Session::get()->startOperation(anId.toStdString());

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
  /// Set current feature and remeber old current feature
  if (myIsEditing) {
    SessionPtr aMgr = ModelAPI_Session::get();
    DocumentPtr aDoc = aMgr->activeDocument();
    myCurrentFeature = aDoc->currentFeature(true);
    aDoc->setCurrentFeature(feature(), false);
  }

  startOperation();
  emit started();

}

void ModuleBase_Operation::postpone()
{
  postponeOperation();
  emit postponed();
}

void ModuleBase_Operation::resume()
{
  resumeOperation();
  emit resumed();
}

void ModuleBase_Operation::abort()
{
  // the viewer update should be blocked in order to avoid the features blinking before they are
  // hidden
  std::shared_ptr<Events_Message> aMsg = std::shared_ptr<Events_Message>(
      new Events_Message(Events_Loop::eventByName(EVENT_UPDATE_VIEWER_BLOCKED)));
  Events_Loop::loop()->send(aMsg);

  SessionPtr aMgr = ModelAPI_Session::get();
  if (myIsEditing) {
    DocumentPtr aDoc = aMgr->activeDocument();
    bool aIsOp = aMgr->isOperation();
    if (!aIsOp)
      aMgr->startOperation();
    aDoc->setCurrentFeature(myCurrentFeature, true);
    if (!aIsOp)
      aMgr->finishOperation();
    myCurrentFeature = FeaturePtr();
  }
  abortOperation();

  stopOperation();
  // is is necessary to deactivate current widgets before the model operation is aborted
  // because abort removes the feature and activated filters should not check it
  propertyPanel()->cleanContent();

  aMgr->abortOperation();
  emit stopped();
  // the viewer update should be unblocked in order to avoid the features blinking before they are
  // hidden
  aMsg = std::shared_ptr<Events_Message>(
                new Events_Message(Events_Loop::eventByName(EVENT_UPDATE_VIEWER_UNBLOCKED)));

  Events_Loop::loop()->send(aMsg);

  emit aborted();
}

bool ModuleBase_Operation::commit()
{
  if (canBeCommitted()) {
    SessionPtr aMgr = ModelAPI_Session::get();
    /// Set current feature and remeber old current feature
    if (myIsEditing) {
      DocumentPtr aDoc = aMgr->activeDocument();
      bool aIsOp = aMgr->isOperation();
      if (!aIsOp)
        aMgr->startOperation();
      aDoc->setCurrentFeature(myCurrentFeature, true);
      if (!aIsOp)
        aMgr->finishOperation();
      myCurrentFeature = FeaturePtr();
    }
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

void ModuleBase_Operation::setRunning(bool theState)
{
  emit triggered(theState);
}

void ModuleBase_Operation::onValuesChanged()
{
  myIsModified = true;
}

//TODO: nds stabilization hotfix
void ModuleBase_Operation::commitOperation()
{
  if(!myPropertyPanel) {
    return;
  }
  ModuleBase_ModelWidget* aWidget = myPropertyPanel->activeWidget();
  if (aWidget)
    aWidget->disconnectSignals();
}

void ModuleBase_Operation::activateByPreselection()
{
  ModuleBase_ModelWidget* aFilledWgt = 0;

  if (myPropertyPanel && !myPreSelection.empty()) {
    const QList<ModuleBase_ModelWidget*>& aWidgets = myPropertyPanel->modelWidgets();
    if (!aWidgets.empty()) {
      ModuleBase_ModelWidget* aWgt = 0;
      QList<ModuleBase_ModelWidget*>::const_iterator aWIt;
      bool isSet = false;
      // 1. apply the selection to controls
      for (aWIt = aWidgets.constBegin(); aWIt != aWidgets.constEnd(); ++aWIt) {
        aWgt = (*aWIt);
        if (!aWgt->canSetValue())
          continue;
        myPropertyPanel->setPreselectionWidget(aWgt);
        if (!aWgt->setSelection(myPreSelection, true)) {
          isSet = false;
          break;
        } else {
          isSet = true;
          aFilledWgt = aWgt;
        }
      }
      myPropertyPanel->setPreselectionWidget(NULL);
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
  }

  // 4. activate the next obligatory widget
  myPropertyPanel->activateNextWidget(aFilledWgt);
  clearPreselection();
}

void ModuleBase_Operation::setParentFeature(CompositeFeaturePtr theParent)
{
  myParentFeature = theParent;
}

CompositeFeaturePtr ModuleBase_Operation::parentFeature() const
{
  return myParentFeature;
}

void ModuleBase_Operation::initSelection(ModuleBase_ISelection* theSelection,
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

  // convert the selection values to the values, which are set to the operation widgets

  //Handle(V3d_View) aView = theViewer->activeView();
  //foreach (ModuleBase_ViewerPrs aPrs, aPreSelected) {
  //  ModuleBase_WidgetValueFeature* aValue = new ModuleBase_WidgetValueFeature();
  //  aValue->setObject(aPrs.object());

  //  double aX, anY;
  //  if (getViewerPoint(aPrs, theViewer, aX, anY))
  //    aValue->setPoint(std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aX, anY)));
  //  myPreSelection.append(aValue);
  //}
  myPreSelection = aPreSelected;
}


bool ModuleBase_Operation::getViewerPoint(ModuleBase_ViewerPrs thePrs,
                                               ModuleBase_IViewer* theViewer,
                                               double& theX, double& theY)
{
  return false;
}

void ModuleBase_Operation::clearPreselection()
{
  myPreSelection.clear();
}

void ModuleBase_Operation::setPropertyPanel(ModuleBase_IPropertyPanel* theProp) 
{ 
  myPropertyPanel = theProp; 
  myPropertyPanel->setEditingMode(isEditOperation());

  if (myPropertyPanel) {
    const QList<ModuleBase_ModelWidget*>& aWidgets = myPropertyPanel->modelWidgets();
    QList<ModuleBase_ModelWidget*>::const_iterator aWIt;
    for (aWIt = aWidgets.constBegin(); aWIt != aWidgets.constEnd(); ++aWIt) {
      ModuleBase_ModelWidget* aWgt = (*aWIt);
      connect(aWgt, SIGNAL(valuesChanged()), this, SLOT(onValuesChanged()));
    }
  }

  // Do not activate widgets by default if the current operation is editing operation
  // Because we don't know which widget is going to be edited. 
  if (!isEditOperation())
    activateByPreselection();
}

bool ModuleBase_Operation::isGranted(QString theId) const
{
  return myNestedFeatures.contains(theId);
}
