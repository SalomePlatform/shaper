// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <ModuleBase_WidgetValidated.h>
#include <ModuleBase_FilterFactory.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_ISelection.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_AttributeValidator.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_ResultCompSolid.h>
#include <ModelAPI_Tools.h>

#include <SelectMgr_ListIteratorOfListOfFilter.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <StdSelect_BRepOwner.hxx>

#include <Events_Loop.h>

#include <QWidget>

//#define DEBUG_VALID_STATE

ModuleBase_WidgetValidated::ModuleBase_WidgetValidated(QWidget* theParent,
                                                       ModuleBase_IWorkshop* theWorkshop,
                                                       const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData),
  myWorkshop(theWorkshop), myIsInValidate(false)
{
}

ModuleBase_WidgetValidated::~ModuleBase_WidgetValidated()
{
}

//********************************************************************
bool ModuleBase_WidgetValidated::setSelection(QList<ModuleBase_ViewerPrs>& theValues,
                                              const bool theToValidate)
{
  if (theValues.empty()) {
    // In order to make reselection possible, set empty object and shape should be done
    setSelectionCustom(ModuleBase_ViewerPrs());
    return false;
  }
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
void ModuleBase_WidgetValidated::clearValidatedCash()
{
  myValidPrs.clear();
  myInvalidPrs.clear();
}

//********************************************************************
void ModuleBase_WidgetValidated::storeAttributeValue()
{
  myIsInValidate = true;
}

//********************************************************************
void ModuleBase_WidgetValidated::restoreAttributeValue(const bool theValid)
{
  myIsInValidate = false;
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
    if (aResult.get() && aResult->shape().get()) {
      // some results have no shape, e.g. the parameter one. So, they should not be validated
      GeomShapePtr aShape = aResult->shape();
      const TopoDS_Shape aTDShape = aShape->impl<TopoDS_Shape>();
      Handle(AIS_InteractiveObject) anIO = myWorkshop->selection()->getIO(thePrs);
      anOwner = new StdSelect_BRepOwner(aTDShape, anIO);
      myPresentedObject = aResult;
    }
    else
      aValid = false; // only results with a shape can be filtered
  }
  // checks the owner by the AIS context activated filters
  if (!anOwner.IsNull()) {
    // the widget validator filter should be active, but during check by preselection
    // it is not yet activated, so we need to activate/deactivate it manually
    bool isActivated = isFilterActivated();
    if (!isActivated)
      activateFilters(true);

    Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
    if (!aContext.IsNull()) {
      const SelectMgr_ListOfFilter& aFilters = aContext->Filters();
      SelectMgr_ListIteratorOfListOfFilter anIt(aFilters);
      for (; anIt.More() && aValid; anIt.Next()) {
        Handle(SelectMgr_Filter) aFilter = anIt.Value();
        aValid = aFilter->IsOk(anOwner);
      }
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

  // stores the current values of the widget attribute
  bool isFlushesActived, isAttributeSetInitializedBlocked;
  blockAttribute(true, isFlushesActived, isAttributeSetInitializedBlocked);

  storeAttributeValue();

  // saves the owner value to the widget attribute
  aValid = setSelectionCustom(theValue);
  if (aValid)
    // checks the attribute validity
    aValid = isValidAttribute();

  // restores the current values of the widget attribute
  restoreAttributeValue(aValid);

  blockAttribute(false, isFlushesActived, isAttributeSetInitializedBlocked);
  // In particular case the results are deleted and called as redisplayed inside of this
  // highlight-selection, to they must be flushed as soon as possible.
  // Example: selection of group-vertices subshapes with shift pressend on body. Without
  //  these 4 lines below the application crashes because of left presentations on
  //  removed results still in the viewer.
  static Events_ID aDeletedEvent = Events_Loop::eventByName(EVENT_OBJECT_DELETED);
  static Events_ID aRedispEvent = Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY);
  Events_Loop::loop()->flush(aDeletedEvent);
  Events_Loop::loop()->flush(aRedispEvent);

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
  AttributePtr anAttribute = myFeature->attribute(attributeID());
  std::string aValidatorID, anError;
  return aFactory->validate(anAttribute, aValidatorID, anError);
}

bool ModuleBase_WidgetValidated::isFilterActivated() const
{
  bool isActivated = false;

  Handle(SelectMgr_Filter) aSelFilter = myWorkshop->validatorFilter();
  ModuleBase_IViewer* aViewer = myWorkshop->viewer();

  return aViewer->hasSelectionFilter(aSelFilter);
}

bool ModuleBase_WidgetValidated::activateFilters(const bool toActivate)
{
  ModuleBase_IViewer* aViewer = myWorkshop->viewer();

  Handle(SelectMgr_Filter) aSelFilter = myWorkshop->validatorFilter();
  bool aHasSelectionFilter = aViewer->hasSelectionFilter(aSelFilter);

  if (toActivate)
    aViewer->addSelectionFilter(aSelFilter);
  else {
    aViewer->removeSelectionFilter(aSelFilter);
    clearValidState();
  }

  return aHasSelectionFilter;
}

//********************************************************************
void ModuleBase_WidgetValidated::blockAttribute(const bool& theToBlock, bool& isFlushesActived,
                                                bool& isAttributeSetInitializedBlocked)
{
  Events_Loop* aLoop = Events_Loop::loop();
  DataPtr aData = myFeature->data();
  AttributePtr anAttribute = myFeature->attribute(attributeID());
  if (theToBlock) {
    // blocks the flush signals to avoid the temporary objects visualization in the viewer
    // they should not be shown in order to do not lose highlight by erasing them
    isFlushesActived = aLoop->activateFlushes(false);

    aData->blockSendAttributeUpdated(true);
    isAttributeSetInitializedBlocked = anAttribute->blockSetInitialized(true);
  }
  else {
    aData->blockSendAttributeUpdated(false);
    anAttribute->blockSetInitialized(isAttributeSetInitializedBlocked);
    aLoop->activateFlushes(isFlushesActived);
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

  filterCompSolids(aSelected);

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

//********************************************************************
void ModuleBase_WidgetValidated::filterCompSolids(QList<ModuleBase_ViewerPrs>& theValues)
{
  std::set<ResultCompSolidPtr> aCompSolids;
  QList<ModuleBase_ViewerPrs> aValidatedValues;

  // Collect compsolids.
  QList<ModuleBase_ViewerPrs>::const_iterator anIt = theValues.begin(), aLast = theValues.end();
  for (; anIt != aLast; anIt++) {
    const ModuleBase_ViewerPrs& aViewerPrs = *anIt;
    ObjectPtr anObject = aViewerPrs.object();
    ResultCompSolidPtr aResultCompSolid = std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(anObject);
    if(aResultCompSolid.get()) {
      aCompSolids.insert(aResultCompSolid);
    }
  }

  // Filter sub-solids of compsolids.
  anIt = theValues.begin();
  for (; anIt != aLast; anIt++) {
    const ModuleBase_ViewerPrs& aViewerPrs = *anIt;
    ObjectPtr anObject = aViewerPrs.object();
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
    ResultCompSolidPtr aResCompSolidPtr = ModelAPI_Tools::compSolidOwner(aResult);
    if(aResCompSolidPtr.get() && (aCompSolids.find(aResCompSolidPtr) != aCompSolids.end())) {
      // Skip sub-solid of compsolid.
      continue;
    } else {
      aValidatedValues.append(*anIt);
    }
  }

  if (aValidatedValues.size() != theValues.size()) {
    theValues.clear();
    theValues = aValidatedValues;
  }
}
