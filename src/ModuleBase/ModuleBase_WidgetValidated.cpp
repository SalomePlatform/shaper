﻿// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <ModuleBase_WidgetValidated.h>
#include <ModuleBase_FilterFactory.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_ISelection.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_AttributeValidator.h>
#include <ModelAPI_Events.h>

#include <SelectMgr_ListIteratorOfListOfFilter.hxx>
#include <SelectMgr_EntityOwner.hxx>

#include <Events_Loop.h>

#include <QWidget>

//#define DEBUG_VALID_STATE

ModuleBase_WidgetValidated::ModuleBase_WidgetValidated(QWidget* theParent,
                                                       const Config_WidgetAPI* theData,
                                                       const std::string& theParentId)
 : ModuleBase_ModelWidget(theParent, theData, theParentId)
{
}

ModuleBase_WidgetValidated::~ModuleBase_WidgetValidated()
{
}

//********************************************************************
bool ModuleBase_WidgetValidated::setSelection(QList<ModuleBase_ViewerPrs>& theValues)
{
  if (theValues.empty())
    return false;
  // it removes the processed value from the parameters list
  ModuleBase_ViewerPrs aValue = theValues.takeFirst();
  bool isDone = false;

  if (isValidSelection(aValue)) {
    isDone = setSelectionCustom(aValue);
    // updateObject - to update/redisplay feature
    // it is commented in order to perfom it outside the method
    //updateObject(myFeature);
    // to storeValue()
    //emit valuesChanged();
  }
  return isDone;
}

//********************************************************************
bool ModuleBase_WidgetValidated::isValidSelection(const ModuleBase_ViewerPrs& theValue)
{
  bool aValid = false;
  if (getValidState(theValue, aValid)) {
    return aValid;
  }

  aValid = isValidSelectionCustom(theValue);
  if (!aValid) {
    storeValidState(theValue, aValid);
    return aValid;
  }

  DataPtr aData = myFeature->data();
  AttributePtr anAttribute = myFeature->attribute(attributeID());

  // stores the current values of the widget attribute
  Events_Loop* aLoop = Events_Loop::loop();
  // blocks the flush signals to avoid the temporary objects visualization in the viewer
  // they should not be shown in order to do not lose highlight by erasing them
  aLoop->activateFlushes(false);

  aData->blockSendAttributeUpdated(true);
  bool isAttributeBlocked = anAttribute->blockSetInitialized(true);
  storeAttributeValue();

  // saves the owner value to the widget attribute
  aValid = setSelectionCustom(theValue);
  if (aValid)
    // checks the attribute validity
    aValid = isValidAttribute();

  // restores the current values of the widget attribute
  restoreAttributeValue(aValid);
  aData->blockSendAttributeUpdated(false);
  anAttribute->blockSetInitialized(isAttributeBlocked);
  aLoop->activateFlushes(true);

  // In particular case the results are deleted and called as redisplayed inside of this
  // highlight-selection, to they must be flushed as soon as possible.
  // Example: selection of group-vertices subshapes with shift pressend on body. Without
  //  these 4 lines below the application crashes because of left presentations on
  //  removed results still in the viewer.
  static Events_ID aDeletedEvent = Events_Loop::eventByName(EVENT_OBJECT_DELETED);
  static Events_ID aRedispEvent = Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY);
  aLoop->flush(aDeletedEvent);
  aLoop->flush(aRedispEvent);

  storeValidState(theValue, aValid);
  return aValid;
}

//********************************************************************
bool ModuleBase_WidgetValidated::isValidSelectionCustom(const ModuleBase_ViewerPrs& thePrs)
{
  return true;
}

//********************************************************************
bool ModuleBase_WidgetValidated::isValidAttribute() const
{
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  std::list<ModelAPI_Validator*> aValidators;
  std::list<std::list<std::string> > anArguments;
  aFactory->validators(myFeature->getKind(), attributeID(), aValidators, anArguments);

  DataPtr aData = myFeature->data();
  AttributePtr anAttribute = myFeature->attribute(attributeID());

  std::list<ModelAPI_Validator*>::iterator aValidator = aValidators.begin();
  std::list<std::list<std::string> >::iterator aArgs = anArguments.begin();
  bool aValid = true;
  for (; aValidator != aValidators.end() && aValid; aValidator++, aArgs++) {
    const ModelAPI_AttributeValidator* aAttrValidator =
        dynamic_cast<const ModelAPI_AttributeValidator*>(*aValidator);
    if (aAttrValidator) {
      aValid = aAttrValidator->isValid(anAttribute, *aArgs);
    }
  }
  return aValid;
}

void ModuleBase_WidgetValidated::activateFilters(ModuleBase_IWorkshop* theWorkshop,
                                                 const bool toActivate)
{
  ModuleBase_IViewer* aViewer = theWorkshop->viewer();

  Handle(SelectMgr_Filter) aSelFilter = theWorkshop->validatorFilter();
  if (toActivate)
    aViewer->addSelectionFilter(aSelFilter);
  else {
    aViewer->removeSelectionFilter(aSelFilter);
    clearValidState();
  }
}

//********************************************************************
void ModuleBase_WidgetValidated::storeValidState(const ModuleBase_ViewerPrs& theValue, const bool theValid)
{
  bool aValidPrs = myInvalidPrs.contains(theValue);
  bool anInvalidPrs = myInvalidPrs.contains(theValue);

  if (theValid) {
    if (!aValidPrs)
      myValidPrs.append(theValue);
    // the commented code will be useful when the valid state of the presentation
    // will be changable between activate/deactivate. Currently it does not happen.
    //if (anInvalidPrs)
    //  myInvalidPrs.removeOne(theValue);
  }
  else { // !theValid
    if (!anInvalidPrs)
      myInvalidPrs.append(theValue);
    //if (!aValidPrs)
    //  myValidPrs.removeOne(theValue);
  }
#ifdef DEBUG_VALID_STATE
  qDebug(QString("storeValidState: myValidPrs.size() = %1, myInvalidPrs.size() = %2").arg(myValidPrs.count())
                 .arg(myInvalidPrs.count()).toStdString().c_str());
#endif
}

//********************************************************************
bool ModuleBase_WidgetValidated::getValidState(const ModuleBase_ViewerPrs& theValue, bool& theValid)
{
  bool aValidPrs = myValidPrs.contains(theValue);
  bool anInvalidPrs = myInvalidPrs.contains(theValue);

  if (aValidPrs)
    theValid = true;
  else if (anInvalidPrs)
    theValid = false;

  return aValidPrs || anInvalidPrs;
}

//********************************************************************
void ModuleBase_WidgetValidated::clearValidState()
{
#ifdef DEBUG_VALID_STATE
  qDebug("clearValidState");
#endif
  myValidPrs.clear();
  myInvalidPrs.clear();
}

