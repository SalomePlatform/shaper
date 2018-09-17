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
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include <ModuleBase_WidgetValidated.h>
#include <ModuleBase_IModule.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_ISelectionActivate.h>
#include <ModuleBase_WidgetSelectorStore.h>
#include <ModuleBase_ViewerPrs.h>

#include <Events_InfoMessage.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_AttributeValidator.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_AttributeSelection.h>

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
void ModuleBase_WidgetValidated::deactivate()
{
  clearValidatedCash();
}

//********************************************************************
void ModuleBase_WidgetValidated::clearValidatedCash()
{
#ifdef DEBUG_VALID_STATE
  qDebug("clearValidatedCash");
#endif
  myValidPrs.Clear();
  myInvalidPrs.Clear();
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
    GeomShapePtr aShape = aResult.get() ? aResult->shape() : GeomShapePtr();
    // some results have no shape, e.g. the parameter one. So, they should not be validated
    if (aShape.get()) {
      const TopoDS_Shape aTDShape = aShape->impl<TopoDS_Shape>();
      Handle(AIS_InteractiveObject) anIO = myWorkshop->selection()->getIO(thePrs);
      anOwner = new StdSelect_BRepOwner(aTDShape, anIO);
      myPresentedObject = aResult;
    }
    else {
      FeaturePtr aFeature = ModelAPI_Feature::feature(thePrs->object());
      if (aFeature.get()) {
        // Use feature as a reference to all its results
        myPresentedObject = aFeature;
        AttributePtr anAttr = attribute();
        std::string aType = anAttr->attributeType();
        // Check that results of Feature is acceptable by filters for selection attribute
        if (aType == ModelAPI_AttributeSelection::typeId()) {
          AttributeSelectionPtr aSelectAttr =
            std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(anAttr);
          aSelectAttr->setValue(myPresentedObject, GeomShapePtr(), true);
          GeomShapePtr aShape = aSelectAttr->value();
          if (aShape.get()) {
            const TopoDS_Shape aTDShape = aShape->impl<TopoDS_Shape>();
            Handle(AIS_InteractiveObject) anIO = myWorkshop->selection()->getIO(thePrs);
            anOwner = new StdSelect_BRepOwner(aTDShape, anIO);
          }
          aSelectAttr->setValue(ObjectPtr(), GeomShapePtr(), true);
        }
        else
          aValid = false; // only results with a shape can be filtered
      } else
        aValid = false; // only results with a shape can be filtered
    }
  }
  // checks the owner by the AIS context activated filters
  if (!anOwner.IsNull()) {
    // the widget validator filter should be active, but during check by preselection
    // it is not yet activated, so we need to activate/deactivate it manually
    bool isActivated = isFilterActivated();
    if (!isActivated) {
      QIntList aModuleSelectionFilters = myWorkshop->module()->selectionFilters();
      SelectMgr_ListOfFilter aSelectionFilters;
      selectionFilters(aModuleSelectionFilters, aSelectionFilters);
      /// after validation, the selection filters should be restored
      myWorkshop->selectionActivate()->activateSelectionFilters(aSelectionFilters);
    }

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
    {
      // reset filters set in activateSelectionFilters above
      myWorkshop->selectionActivate()->updateSelectionFilters();
      clearValidatedCash();
    }
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
bool ModuleBase_WidgetValidated::isValidSelectionForAttribute(
                                            const ModuleBase_ViewerPrsPtr& theValue,
                                            const AttributePtr& theAttribute)
{
  bool aValid = false;

  // stores the current values of the widget attribute
  bool isFlushesActived, isAttributeSetInitializedBlocked, isAttributeSendUpdatedBlocked;

  blockAttribute(theAttribute, true, isFlushesActived, isAttributeSetInitializedBlocked,
                 isAttributeSendUpdatedBlocked);

  storeAttributeValue(theAttribute);

  // saves the owner value to the widget attribute
  aValid = setSelectionCustom(theValue);
  if (aValid)
    // checks the attribute validity
    aValid = isValidAttribute(theAttribute);

  // restores the current values of the widget attribute
  restoreAttributeValue(theAttribute, aValid);

  blockAttribute(theAttribute, false, isFlushesActived, isAttributeSetInitializedBlocked,
                 isAttributeSendUpdatedBlocked);
  /// NDS: The following rows are commented for issue #1452 (to be removed after debug)
  /// This is not correct to perform it here because it might cause update selection and
  /// the selection mechanizm will be circled: use the scenario of the bug with preselected point.
  // In particular case the results are deleted and called as redisplayed inside of this
  // highlight-selection, to they must be flushed as soon as possible.
  // Example: selection of group-vertices subshapes with shift pressend on body. Without
  //  these 4 lines below the application crashes because of left presentations on
  //  removed results still in the viewer.
  /*static Events_ID aDeletedEvent = Events_Loop::eventByName(EVENT_OBJECT_DELETED);
  static Events_ID aRedispEvent = Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY);
  Events_Loop::loop()->flush(aDeletedEvent);
  Events_Loop::loop()->flush(aRedispEvent);
  */
  return aValid;
}

//********************************************************************
bool ModuleBase_WidgetValidated::isValidSelectionCustom(const ModuleBase_ViewerPrsPtr& thePrs)
{
  return true;
}

//********************************************************************
bool ModuleBase_WidgetValidated::isValidAttribute(const AttributePtr& theAttribute)
{
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  std::string aValidatorID;
  Events_InfoMessage anError;
  return aFactory->validate(theAttribute, aValidatorID, anError);
}

//********************************************************************
bool ModuleBase_WidgetValidated::isFilterActivated() const
{
  bool isActivated = false;

  Handle(SelectMgr_Filter) aSelFilter = myWorkshop->validatorFilter();
  ModuleBase_IViewer* aViewer = myWorkshop->viewer();

  return aViewer->hasSelectionFilter(aSelFilter);
}

//********************************************************************
void ModuleBase_WidgetValidated::selectionFilters(QIntList& theModuleSelectionFilters,
                                                  SelectMgr_ListOfFilter& theSelectionFilters)
{
  theSelectionFilters.Append(myWorkshop->validatorFilter());
}

//********************************************************************
void ModuleBase_WidgetValidated::blockAttribute(const AttributePtr& theAttribute,
                                                const bool& theToBlock,
                                                bool& isFlushesActived,
                                                bool& isAttributeSetInitializedBlocked,
                                                bool& isAttributeSendUpdatedBlocked)
{
  blockFeatureAttribute(theAttribute, myFeature, theToBlock, isFlushesActived,
                        isAttributeSetInitializedBlocked, isAttributeSendUpdatedBlocked);
}

//********************************************************************
void ModuleBase_WidgetValidated::blockFeatureAttribute(const AttributePtr& theAttribute,
                                                const FeaturePtr& theFeature,
                                                const bool& theToBlock,
                                                bool& isFlushesActived,
                                                bool& isAttributeSetInitializedBlocked,
                                                bool& isAttributeSendUpdatedBlocked)
{
  Events_Loop* aLoop = Events_Loop::loop();
  DataPtr aData = theFeature->data();
  if (theToBlock) {
    // blocks the flush signals to avoid the temporary objects visualization in the viewer
    // they should not be shown in order to do not lose highlight by erasing them
    isFlushesActived = aLoop->activateFlushes(false);

    isAttributeSendUpdatedBlocked = aData->blockSendAttributeUpdated(true);
    isAttributeSetInitializedBlocked = theAttribute->blockSetInitialized(true);
  }
  else {
    aData->blockSendAttributeUpdated(isAttributeSendUpdatedBlocked, false);
    theAttribute->blockSetInitialized(isAttributeSetInitializedBlocked);
    aLoop->activateFlushes(isFlushesActived);
  }
}

//********************************************************************
void ModuleBase_WidgetValidated::storeValidState(const ModuleBase_ViewerPrsPtr& theValue,
                                                 const bool theValid)
{
  GeomShapePtr aShape = theValue.get() ? theValue->shape() : GeomShapePtr();
  if (aShape.get()) {
    if (theValid) {
      const TopoDS_Shape& aTDShape = aShape->impl<TopoDS_Shape>();
      bool aValidPrsContains = myValidPrs.IsBound(aTDShape) &&
                               theValue.get()->isEqual(myValidPrs.Find(aTDShape).get());
      if (!aValidPrsContains) {
  #ifdef LIST_OF_VALID_PRS
        myValidPrs.append(theValue);
  #else
        myValidPrs.Bind(aTDShape, theValue);
  #endif
      // the commented code will be useful when the valid state of the presentation
      // will be changable between activate/deactivate. Currently it does not happen.
      //if (anInvalidPrs)
      //  myInvalidPrs.removeOne(theValue);
      }
    }
    else { // !theValid
      if (aShape.get()) {
        const TopoDS_Shape& aTDShape = aShape->impl<TopoDS_Shape>();
        bool anIValidPrsContains = myInvalidPrs.IsBound(aTDShape) &&
                                   theValue.get()->isEqual(myInvalidPrs.Find(aTDShape).get());
        if (!anIValidPrsContains) {
    #ifdef LIST_OF_VALID_PRS
          myInvalidPrs.append(theValue);
    #else
          myInvalidPrs.Bind(aTDShape, theValue);
    #endif
        //if (!aValidPrs)
        //  myValidPrs.removeOne(theValue);
        }
      }
    }
  }
  #ifdef DEBUG_VALID_STATE
    qDebug(QString("storeValidState: myValidPrs.size() = %1, myInvalidPrs.size() = %2")
                   .arg(myValidPrs.count())
                   .arg(myInvalidPrs.count()).toStdString().c_str());
  #endif
}

//********************************************************************
bool ModuleBase_WidgetValidated::getValidState(const ModuleBase_ViewerPrsPtr& theValue,
                                               bool& theValid)
{
  if (!theValue.get())
    return false;

#ifdef LIST_OF_VALID_PRS
  bool aValidPrsContains = myValidPrs.contains(theValue);
  bool anInvalidPrsContains = myInvalidPrs.contains(theValue);
#else
  GeomShapePtr aShape = theValue.get() ? theValue->shape() : GeomShapePtr();
  if (!aShape.get())
    return false;

  const TopoDS_Shape& aTDShape = aShape->impl<TopoDS_Shape>();
  bool aValidPrsContains = myValidPrs.IsBound(aTDShape) &&
                           theValue.get()->isEqual(myValidPrs.Find(aTDShape).get());

  bool anInvalidPrsContains = myInvalidPrs.IsBound(aTDShape) &&
                              theValue.get()->isEqual(myInvalidPrs.Find(aTDShape).get());
  /*
  bool aValidPrsContains = false, anInvalidPrsContains = false;
  GeomShapePtr aShape = theValue.get() ? theValue->shape() : GeomShapePtr();
  if (aShape.get()) {
    aValidPrsContains = myValidPrs.contains(aShape);
    anInvalidPrsContains = myInvalidPrs.contains(aShape);

    if (aValidPrsContains)
      aValidPrsContains = theValue == myValidPrs[aShape];
    else
      anInvalidPrsContains = theValue == myInvalidPrs[aShape];*/
#endif

  if (aValidPrsContains)
    theValid = true;
  else if (anInvalidPrsContains)
    theValid = false;

  return aValidPrsContains || anInvalidPrsContains;
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
  std::set<ResultPtr> aFilterOut; // all objects that must be filtered out with their children
  QList<ModuleBase_ViewerPrsPtr> aValidatedValues;

  // Collect compsolids.
  QList<ModuleBase_ViewerPrsPtr>::const_iterator anIt = theValues.begin(), aLast = theValues.end();
  for(; anIt != aLast; anIt++) {
    const ModuleBase_ViewerPrsPtr& aViewerPrs = *anIt;
    ObjectPtr anObject = aViewerPrs->object();
    ResultBodyPtr aResultCompSolid =
      std::dynamic_pointer_cast<ModelAPI_ResultBody>(anObject);
    if (aResultCompSolid.get()) {
      for(int aSubIndex = 0; aSubIndex < aResultCompSolid->numberOfSubs(); aSubIndex++)
        aFilterOut.insert(aResultCompSolid->subResult(aSubIndex));
    } else { // it could be a whole feature selected, so, add all results of this feature
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(anObject);
      if (aFeature.get()) {
        std::list<ResultPtr>::const_iterator aRes = aFeature->results().cbegin();
        for(; aRes != aFeature->results().cend(); aRes++)
          aFilterOut.insert(*aRes);
      }
    }
  }

  // Filter sub-solids of compsolids.
  anIt = theValues.begin();
  for(; anIt != aLast; anIt++) {
    const ModuleBase_ViewerPrsPtr& aViewerPrs = *anIt;
    ObjectPtr anObject = aViewerPrs->object();
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
    while(aResult.get()) {
      if (aFilterOut.find(aResult) != aFilterOut.end()) // skip if parent is filtered out
        break;
      aResult = ModelAPI_Tools::bodyOwner(aResult); // iterate all parents
    }
    if (aResult.get()) {
      continue; // skip
    } else {
      aValidatedValues.append(*anIt);
    }
  }

  if (aValidatedValues.size() != theValues.size()) {
    theValues.clear();
    theValues = aValidatedValues;
  }
}
