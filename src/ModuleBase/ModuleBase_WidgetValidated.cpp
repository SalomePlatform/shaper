// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

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
#include <StdSelect_BRepOwner.hxx>

#include <Events_Loop.h>

#include <QWidget>

//#define DEBUG_VALID_STATE

ModuleBase_WidgetValidated::ModuleBase_WidgetValidated(QWidget* theParent,
                                                       ModuleBase_IWorkshop* theWorkshop,
                                                       const Config_WidgetAPI* theData,
                                                       const std::string& theParentId)
: ModuleBase_ModelWidget(theParent, theData, theParentId),
  myWorkshop(theWorkshop)
{
}

ModuleBase_WidgetValidated::~ModuleBase_WidgetValidated()
{
}

//********************************************************************
bool ModuleBase_WidgetValidated::setSelection(QList<ModuleBase_ViewerPrs>& theValues,
                                              const bool theToValidate)
{
  if (theValues.empty())
    return false;
  // it removes the processed value from the parameters list
  ModuleBase_ViewerPrs aValue = theValues.takeFirst();
  bool isDone = false;

  if (!theToValidate || isValidInFilters(aValue)) {
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
ObjectPtr ModuleBase_WidgetValidated::findPresentedObject(const AISObjectPtr& theAIS) const
{
  return myPresentedObject;
}

//********************************************************************
bool ModuleBase_WidgetValidated::isValidInFilters(const ModuleBase_ViewerPrs& thePrs)
{
  bool aValid = true;
  Handle(SelectMgr_EntityOwner) anOwner = thePrs.owner();

  // if an owner is null, the selection happens in the Object browser.
  // creates a selection owner on the base of object shape and the object AIS object
  if (anOwner.IsNull() && thePrs.owner().IsNull() && thePrs.object().get()) {
    ResultPtr aResult = myWorkshop->selection()->getResult(thePrs);
    if (aResult.get()) {
      GeomShapePtr aShape = aResult->shape();
      const TopoDS_Shape aTDShape = aShape->impl<TopoDS_Shape>();
      Handle(AIS_InteractiveObject) anIO = myWorkshop->selection()->getIO(thePrs);
      anOwner = new StdSelect_BRepOwner(aTDShape, anIO);
      myPresentedObject = aResult;
    }
    else
      aValid = false; // only results can be filtered
  }
  // checks the owner by the AIS context activated filters
  if (!anOwner.IsNull()) {
    // the widget validator filter should be active, but during check by preselection
    // it is not yet activated, so we need to activate/deactivate it manually
    bool isActivated = isFilterActivated();
    if (!isActivated)
      activateFilters(true);

    const SelectMgr_ListOfFilter& aFilters = myWorkshop->viewer()->AISContext()->Filters();
    SelectMgr_ListIteratorOfListOfFilter anIt(aFilters);
    for (; anIt.More() && aValid; anIt.Next()) {
      Handle(SelectMgr_Filter) aFilter = anIt.Value();
      aValid = aFilter->IsOk(anOwner);
    }
    if (!isActivated)
      activateFilters(false);
  }

  // removes created owner
  if (!anOwner.IsNull() && anOwner != thePrs.owner()) {
    anOwner.Nullify();
    myPresentedObject = ObjectPtr();
  }
  return aValid;
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
  bool isActive = aLoop->activateFlushes(false);

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
  aLoop->activateFlushes(isActive);

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

bool ModuleBase_WidgetValidated::isFilterActivated() const
{
  bool isActivated = false;

  Handle(SelectMgr_Filter) aSelFilter = myWorkshop->validatorFilter();

  const SelectMgr_ListOfFilter& aFilters = myWorkshop->viewer()->AISContext()->Filters();
  SelectMgr_ListIteratorOfListOfFilter aIt(aFilters);
  for (; aIt.More(); aIt.Next()) {
    if (aSelFilter.Access() == aIt.Value().Access())
      isActivated = true;
  }
  return isActivated;
}


void ModuleBase_WidgetValidated::activateFilters(const bool toActivate)
{
  ModuleBase_IViewer* aViewer = myWorkshop->viewer();

  Handle(SelectMgr_Filter) aSelFilter = myWorkshop->validatorFilter();
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

//********************************************************************
QList<ModuleBase_ViewerPrs> ModuleBase_WidgetValidated::getFilteredSelected()
{
  QList<ModuleBase_ViewerPrs> aSelected = myWorkshop->selection()->getSelected(
                                                       ModuleBase_ISelection::Viewer);

  QList<ModuleBase_ViewerPrs> anOBSelected = myWorkshop->selection()->getSelected(
                                                       ModuleBase_ISelection::Browser);
  // filter the OB presentations
  filterPresentations(anOBSelected);
  if (!anOBSelected.isEmpty())
    ModuleBase_ISelection::appendSelected(anOBSelected, aSelected);

  return aSelected;
}

//********************************************************************
void ModuleBase_WidgetValidated::filterPresentations(QList<ModuleBase_ViewerPrs>& theValues)
{
  QList<ModuleBase_ViewerPrs> aValidatedValues;

  QList<ModuleBase_ViewerPrs>::const_iterator anIt = theValues.begin(), aLast = theValues.end();
  bool isDone = false;
  for (; anIt != aLast; anIt++) {
    if (isValidInFilters(*anIt))
      aValidatedValues.append(*anIt);
  }
  if (aValidatedValues.size() != theValues.size()) {
    theValues.clear();
    theValues = aValidatedValues;
  }
}
