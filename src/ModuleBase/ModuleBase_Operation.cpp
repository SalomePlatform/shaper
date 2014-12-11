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
  //Get validators for the Id
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  return aFactory->validate(myFeature);
}

bool ModuleBase_Operation::isNestedOperationsEnabled() const
{
  return true;
}

//void ModuleBase_Operation::storeCustomValue()
//{
//  if (!myFeature) {
//#ifdef _DEBUG
//    qDebug() << "ModuleBase_Operation::storeCustom: " <<
//    "trying to store value without opening a transaction.";
//#endif
//    return;
//  }
//
//  ModuleBase_ModelWidget* aCustom = dynamic_cast<ModuleBase_ModelWidget*>(sender());
//  if (aCustom)
//    aCustom->storeValue();
//}


bool ModuleBase_Operation::canBeCommitted() const
{
  return isValid();
}

void ModuleBase_Operation::flushUpdated()
{
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
}

void ModuleBase_Operation::flushCreated()
{
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_CREATED));
}

FeaturePtr ModuleBase_Operation::createFeature(const bool theFlushMessage)
{
  if (myParentFeature) {
    myFeature = myParentFeature->addFeature(getDescription()->operationId().toStdString());
  } else {
    std::shared_ptr<ModelAPI_Document> aDoc = document();
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
    flushCreated();
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


std::shared_ptr<ModelAPI_Document> ModuleBase_Operation::document() const
{
  return ModelAPI_Session::get()->moduleDocument();
}


void ModuleBase_Operation::start()
{
  ModelAPI_Session::get()->startOperation();

  if (!myIsEditing)
    createFeature();

  startOperation();
  emit started();
}

void ModuleBase_Operation::postpone()
{
  if (myPropertyPanel)
    disconnect(myPropertyPanel, 0, this, 0);
  emit postponed();
}

void ModuleBase_Operation::resume()
{
  //  connect(myPropertyPanel, SIGNAL(widgetActivated(ModuleBase_ModelWidget*)),
  //          this,            SLOT(onWidgetActivated(ModuleBase_ModelWidget*)));
  emit resumed();
}

void ModuleBase_Operation::abort()
{
  abortOperation();
  emit aborted();
  if (myPropertyPanel)
    disconnect(myPropertyPanel, 0, this, 0);

  stopOperation();

  ModelAPI_Session::get()->abortOperation();
  emit stopped();
}

bool ModuleBase_Operation::commit()
{
  if (canBeCommitted()) {
    if (myPropertyPanel)
      disconnect(myPropertyPanel, 0, this, 0);

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
  if (!theState) {
    abort();
  }
}

bool ModuleBase_Operation::activateByPreselection()
{
  if (!myPropertyPanel || myPreSelection.empty()) {
    myPropertyPanel->activateNextWidget(NULL);
    return false;
  }
  const QList<ModuleBase_ModelWidget*>& aWidgets = myPropertyPanel->modelWidgets();
  if (aWidgets.empty()) {
    myPropertyPanel->activateNextWidget(NULL);
    return false;
  }
  
  ModuleBase_ModelWidget* aWgt, *aFilledWgt = 0;
  QList<ModuleBase_ModelWidget*>::const_iterator aWIt;
  QList<ModuleBase_ViewerPrs>::const_iterator aPIt;
  bool isSet = false;
  for (aWIt = aWidgets.constBegin(), aPIt = myPreSelection.constBegin();
       (aWIt != aWidgets.constEnd()) && (aPIt != myPreSelection.constEnd());
       ++aWIt) {
    aWgt = (*aWIt);
    ModuleBase_ViewerPrs aValue = (*aPIt);
    if (!aWgt->canSetValue())
      continue;

    ++aPIt;
    if (!aWgt->setSelection(aValue)) {
      isSet = false;
      break;
    } else {
      isSet = true;
      aFilledWgt = aWgt;
    }
  }

  if (aFilledWgt) {
    myPropertyPanel->activateNextWidget(aFilledWgt);
    emit activatedByPreselection();
    return true;
  }
  else
    myPropertyPanel->activateNextWidget(NULL);
  return false;
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
  //connect(myPropertyPanel, SIGNAL(widgetActivated(ModuleBase_ModelWidget*)), this,
  //        SLOT(onWidgetActivated(ModuleBase_ModelWidget*)));
}

bool ModuleBase_Operation::isGranted(QString theId) const
{
  return myNestedFeatures.contains(theId);
}