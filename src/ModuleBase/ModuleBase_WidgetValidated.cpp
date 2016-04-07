// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <ModuleBase_WidgetValidated.h>
#include <ModuleBase_FilterFactory.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_WidgetSelectorStore.h>
#include <ModuleBase_ViewerPrs.h>

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
  myAttributeStore = new ModuleBase_WidgetSelectorStore();
}

ModuleBase_WidgetValidated::~ModuleBase_WidgetValidated()
{
  delete myAttributeStore;
}

//********************************************************************
ObjectPtr ModuleBase_WidgetValidated::findPresentedObject(const AISObjectPtr& theAIS) const
{
  return myPresentedObject;
}

//********************************************************************
void ModuleBase_WidgetValidated::clearValidatedCash()
{
#ifdef DEBUG_VALID_STATE
  qDebug("clearValidatedState");
#endif
  myValidPrs.clear();
  myInvalidPrs.clear();
}

//********************************************************************
void ModuleBase_WidgetValidated::storeAttributeValue(const AttributePtr& theAttribute)
{
  myIsInValidate = true;
  myAttributeStore->storeAttributeValue(theAttribute, myWorkshop);
}

//********************************************************************
void ModuleBase_WidgetValidated::restoreAttributeValue(const AttributePtr& theAttribute,
                                                       const bool theValid)
{
  myIsInValidate = false;
  myAttributeStore->restoreAttributeValue(theAttribute, myWorkshop);
}

//********************************************************************
bool ModuleBase_WidgetValidated::isValidInFilters(const ModuleBase_ViewerPrsPtr& thePrs)
{
  bool aValid = true;
  Handle(SelectMgr_EntityOwner) anOwner = thePrs->owner();

  // if an owner is null, the selection happens in the Object browser.
  // creates a selection owner on the base of object shape and the object AIS object
  if (anOwner.IsNull() && thePrs->owner().IsNull() && thePrs->object().get()) {
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
  if (!anOwner.IsNull() && anOwner != thePrs->owner()) {
    anOwner.Nullify();
    myPresentedObject = ObjectPtr();
  }
  return aValid;
}

//********************************************************************
AttributePtr ModuleBase_WidgetValidated::attribute() const
{
  return myFeature->attribute(attributeID());
}

//********************************************************************
bool ModuleBase_WidgetValidated::isValidSelection(const ModuleBase_ViewerPrsPtr& theValue)
{
  bool aValid = false;
  if (getValidState(theValue, aValid)) {
    return aValid;
  }
  aValid = isValidSelectionCustom(theValue);
  if (aValid)
    aValid = isValidSelectionForAttribute(theValue, attribute());

  storeValidState(theValue, aValid);
  return aValid;
}

//********************************************************************
bool ModuleBase_WidgetValidated::isValidSelectionForAttribute(const ModuleBase_ViewerPrsPtr& theValue,
                                                              const AttributePtr& theAttribute)
{
  bool aValid = false;

  // stores the current values of the widget attribute
  bool isFlushesActived, isAttributeSetInitializedBlocked;

  blockAttribute(theAttribute, true, isFlushesActived, isAttributeSetInitializedBlocked);

  storeAttributeValue(theAttribute);

  // saves the owner value to the widget attribute
  aValid = setSelectionCustom(theValue);
  if (aValid)
    // checks the attribute validity
    aValid = isValidAttribute(theAttribute);

  // restores the current values of the widget attribute
  restoreAttributeValue(theAttribute, aValid);

  blockAttribute(theAttribute, false, isFlushesActived, isAttributeSetInitializedBlocked);
  // In particular case the results are deleted and called as redisplayed inside of this
  // highlight-selection, to they must be flushed as soon as possible.
  // Example: selection of group-vertices subshapes with shift pressend on body. Without
  //  these 4 lines below the application crashes because of left presentations on
  //  removed results still in the viewer.
  static Events_ID aDeletedEvent = Events_Loop::eventByName(EVENT_OBJECT_DELETED);
  static Events_ID aRedispEvent = Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY);
  Events_Loop::loop()->flush(aDeletedEvent);
  Events_Loop::loop()->flush(aRedispEvent);

  return aValid;
}

//********************************************************************
bool ModuleBase_WidgetValidated::isValidSelectionCustom(const ModuleBase_ViewerPrsPtr& thePrs)
{
  return true;
}

//********************************************************************
bool ModuleBase_WidgetValidated::isValidAttribute(const AttributePtr& theAttribute) const
{
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  std::string aValidatorID, anError;
  return aFactory->validate(theAttribute, aValidatorID, anError);
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
    clearValidatedCash();
  }

  return aHasSelectionFilter;
}

//********************************************************************
void ModuleBase_WidgetValidated::blockAttribute(const AttributePtr& theAttribute,
                                                const bool& theToBlock,
                                                bool& isFlushesActived,
                                                bool& isAttributeSetInitializedBlocked)
{
  Events_Loop* aLoop = Events_Loop::loop();
  DataPtr aData = myFeature->data();
  if (theToBlock) {
    // blocks the flush signals to avoid the temporary objects visualization in the viewer
    // they should not be shown in order to do not lose highlight by erasing them
    isFlushesActived = aLoop->activateFlushes(false);

    aData->blockSendAttributeUpdated(true);
    isAttributeSetInitializedBlocked = theAttribute->blockSetInitialized(true);
  }
  else {
    aData->blockSendAttributeUpdated(false);
    theAttribute->blockSetInitialized(isAttributeSetInitializedBlocked);
    aLoop->activateFlushes(isFlushesActived);
  }
}

//********************************************************************
void ModuleBase_WidgetValidated::storeValidState(const ModuleBase_ViewerPrsPtr& theValue, const bool theValid)
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
bool ModuleBase_WidgetValidated::getValidState(const ModuleBase_ViewerPrsPtr& theValue, bool& theValid)
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
QList<ModuleBase_ViewerPrsPtr> ModuleBase_WidgetValidated::getFilteredSelected()
{
  QList<ModuleBase_ViewerPrsPtr> aSelected = myWorkshop->selection()->getSelected(
                                                       ModuleBase_ISelection::Viewer);

  QList<ModuleBase_ViewerPrsPtr> anOBSelected = myWorkshop->selection()->getSelected(
                                                       ModuleBase_ISelection::Browser);
  // filter the OB presentations
  filterPresentations(anOBSelected);
  if (!anOBSelected.isEmpty())
    ModuleBase_ISelection::appendSelected(anOBSelected, aSelected);

  filterCompSolids(aSelected);

  return aSelected;
}

//********************************************************************
void ModuleBase_WidgetValidated::filterPresentations(QList<ModuleBase_ViewerPrsPtr>& theValues)
{
  QList<ModuleBase_ViewerPrsPtr> aValidatedValues;

  QList<ModuleBase_ViewerPrsPtr>::const_iterator anIt = theValues.begin(), aLast = theValues.end();
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
void ModuleBase_WidgetValidated::filterCompSolids(QList<ModuleBase_ViewerPrsPtr>& theValues)
{
  std::set<ResultCompSolidPtr> aCompSolids;
  QList<ModuleBase_ViewerPrsPtr> aValidatedValues;

  // Collect compsolids.
  QList<ModuleBase_ViewerPrsPtr>::const_iterator anIt = theValues.begin(), aLast = theValues.end();
  for (; anIt != aLast; anIt++) {
    const ModuleBase_ViewerPrsPtr& aViewerPrs = *anIt;
    ObjectPtr anObject = aViewerPrs->object();
    ResultCompSolidPtr aResultCompSolid = std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(anObject);
    if(aResultCompSolid.get()) {
      aCompSolids.insert(aResultCompSolid);
    }
  }

  // Filter sub-solids of compsolids.
  anIt = theValues.begin();
  for (; anIt != aLast; anIt++) {
    const ModuleBase_ViewerPrsPtr& aViewerPrs = *anIt;
    ObjectPtr anObject = aViewerPrs->object();
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
