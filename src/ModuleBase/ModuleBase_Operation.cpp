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
  if (!myFeature)
    return true; // rename operation
  if (myFeature->isAction())
    return true;
  //Get validators for the Id
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  bool aValid = aFactory->validate(myFeature);

  // the feature exec state should be checked in order to do not apply features, which result can not
  // be built. E.g. extrusion on sketch, where the "to" is a perpendicular plane to the sketch
  bool isDone = myFeature->data()->execState() == ModelAPI_StateDone;

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
  if (myIsEditing) {
    SessionPtr aMgr = ModelAPI_Session::get();
    DocumentPtr aDoc = aMgr->activeDocument();
    aDoc->setCurrentFeature(myCurrentFeature, true);
    myCurrentFeature = FeaturePtr();
  }
  abortOperation();
  emit aborted();

  stopOperation();

  ModelAPI_Session::get()->abortOperation();
  emit stopped();
}

bool ModuleBase_Operation::commit()
{
  if (canBeCommitted()) {
    /// Set current feature and remeber old current feature
    if (myIsEditing) {
      SessionPtr aMgr = ModelAPI_Session::get();
      DocumentPtr aDoc = aMgr->activeDocument();
      aDoc->setCurrentFeature(myCurrentFeature, true);
      myCurrentFeature = FeaturePtr();
    }
    commitOperation();
    // check whether there are modifications performed during the current operation
    // in the model
    // in case if there are no modifications, do not increase the undo/redo stack
    if (ModelAPI_Session::get()->isModified())
      ModelAPI_Session::get()->finishOperation();
    else
      ModelAPI_Session::get()->abortOperation();

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

void ModuleBase_Operation::activateByPreselection()
{
  if (!myPropertyPanel || myPreSelection.empty()) {
    myPropertyPanel->activateNextWidget(NULL);
    return;
  }
  const QList<ModuleBase_ModelWidget*>& aWidgets = myPropertyPanel->modelWidgets();
  if (aWidgets.empty()) {
    myPropertyPanel->activateNextWidget(NULL);
    return;
  }
  
  ModuleBase_ModelWidget* aWgt, *aFilledWgt = 0;
  QList<ModuleBase_ModelWidget*>::const_iterator aWIt;
  bool isSet = false;
  // 1. apply the selection to controls
  int aCurrentPosition = 0;
  for (aWIt = aWidgets.constBegin(); aWIt != aWidgets.constEnd(); ++aWIt) {
    aWgt = (*aWIt);
    if (!aWgt->canSetValue())
      continue;

    if (!aWgt->setSelection(myPreSelection, aCurrentPosition/*aValue*/)) {
      isSet = false;
      break;
    } else {
      isSet = true;
      aFilledWgt = aWgt;
    }
  }
  // 2. ignore not obligatory widgets
  /*for (; aWIt != aWidgets.constEnd(); ++aWIt) {
    aWgt = (*aWIt);
    if (aWgt && aWgt->isObligatory())
      continue;
    aFilledWgt = aWgt;
  }*/

  // 3. activate the next obligatory widget
  myPropertyPanel->activateNextWidget(aFilledWgt);
  if (aFilledWgt)
    emit activatedByPreselection();

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
    QList<ModuleBase_ViewerPrs> aSelected = theSelection->getSelected();

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
    aPreSelected = theSelection->getSelected();

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

//void ModuleBase_Operation::onWidgetActivated(ModuleBase_ModelWidget* theWidget)
//{
//  //activateByPreselection();
//  //if (theWidget && myPropertyPanel) {
//  //  myPropertyPanel->activateNextWidget();
//  ////  //emit activateNextWidget(myActiveWidget);
//  //}
//}

//bool ModuleBase_Operation::setWidgetValue(ObjectPtr theFeature, double theX, double theY)
//{
//  ModuleBase_ModelWidget* aActiveWgt = myPropertyPanel->activeWidget();
//  if (!aActiveWgt)
//    return false;
//  ModuleBase_WidgetValueFeature* aValue = new ModuleBase_WidgetValueFeature();
//  aValue->setObject(theFeature);
//  aValue->setPoint(std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(theX, theY)));
//  bool isApplyed = aActiveWgt->setValue(aValue);
//
//  delete aValue;
//  myIsModified = (myIsModified || isApplyed);
//  return isApplyed;
//}

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

  // Do not activate widgets by default if the current operation is editing operation
  // Because we don't know which widget is going to be edited. 
  if (!isEditOperation())
    activateByPreselection();
}

bool ModuleBase_Operation::isGranted(QString theId) const
{
  return myNestedFeatures.contains(theId);
}
