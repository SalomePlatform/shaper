// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        XGUI_Displayer.cpp
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "XGUI_Displayer.h"
#include "XGUI_Workshop.h"
#include "XGUI_ViewerProxy.h"

#include <AppElements_Viewer.h>

#include <ModelAPI_Document.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Tools.h>

#include <ModuleBase_ResultPrs.h>

#include <GeomAPI_Shape.h>
#include <GeomAPI_IPresentable.h>
#include <GeomAPI_ICustomPrs.h>

#include <AIS_InteractiveContext.hxx>
#include <AIS_LocalContext.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_DimensionSelectionMode.hxx>
#include <AIS_Shape.hxx>
#include <AIS_Dimension.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <SelectMgr_ListOfFilter.hxx>
#include <SelectMgr_ListIteratorOfListOfFilter.hxx>

#include <TColStd_MapOfTransient.hxx>
#include <TColStd_MapIteratorOfMapOfTransient.hxx>

#include <set>

const int MOUSE_SENSITIVITY_IN_PIXEL = 10;  ///< defines the local context mouse selection sensitivity


// Workaround for bug #25637
void displayedObjects(const Handle(AIS_InteractiveContext)& theAIS, AIS_ListOfInteractive& theList)
{
  // Get from null point
  theAIS->DisplayedObjects(theList, true);
  if (theAIS->HasOpenedContext()) {
    // get from local context
    const Handle(AIS_LocalContext)& aLC = theAIS->LocalContext();
    TColStd_MapOfTransient aMap;
    int NbDisp = aLC->DisplayedObjects(aMap);
    TColStd_MapIteratorOfMapOfTransient aIt(aMap);

    Handle(AIS_InteractiveObject) curIO;
    Handle(Standard_Transient) Tr;
    for(; aIt.More(); aIt.Next()){
      Tr = aIt.Key();
      curIO = *((Handle(AIS_InteractiveObject)*) &Tr);
      theList.Append(curIO);
    }
  }
}


XGUI_Displayer::XGUI_Displayer(XGUI_Workshop* theWorkshop)
  : myWorkshop(theWorkshop)
{
}

XGUI_Displayer::~XGUI_Displayer()
{
}

bool XGUI_Displayer::isVisible(ObjectPtr theObject) const
{
  return myResult2AISObjectMap.contains(theObject);
}

void XGUI_Displayer::display(ObjectPtr theObject, bool isUpdateViewer)
{
  if (isVisible(theObject)) {
    redisplay(theObject, isUpdateViewer);
  } else {
    AISObjectPtr anAIS;

    GeomPresentablePtr aPrs = std::dynamic_pointer_cast<GeomAPI_IPresentable>(theObject);
    bool isShading = false;
    if (aPrs.get() != NULL) {
      anAIS = aPrs->getAISObject(AISObjectPtr());
    } else {
      ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
      if (aResult.get() != NULL) {
        std::shared_ptr<GeomAPI_Shape> aShapePtr = ModelAPI_Tools::shape(aResult);
        if (aShapePtr.get() != NULL) {
          anAIS = AISObjectPtr(new GeomAPI_AISObject());
          anAIS->setImpl(new Handle(AIS_InteractiveObject)(new ModuleBase_ResultPrs(aResult)));
          //anAIS->createShape(aShapePtr);
          isShading = true;
        }
      }
    }
    if (anAIS)
      display(theObject, anAIS, isShading, isUpdateViewer);
  }
}

bool canBeShaded(Handle(AIS_InteractiveObject) theAIS)
{
  Handle(AIS_Shape) aShapePrs = Handle(AIS_Shape)::DownCast(theAIS);
  if (!aShapePrs.IsNull()) {
    TopoDS_Shape aShape = aShapePrs->Shape();
    TopAbs_ShapeEnum aType = aShape.ShapeType();
    if ((aType == TopAbs_VERTEX) || (aType == TopAbs_EDGE) || (aType == TopAbs_WIRE))
      return false;
    else
      return true;
  }
  return false;
}

void XGUI_Displayer::display(ObjectPtr theObject, AISObjectPtr theAIS, 
                             bool isShading, bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return;

  Handle(AIS_InteractiveObject) anAISIO = theAIS->impl<Handle(AIS_InteractiveObject)>();
  if (!anAISIO.IsNull()) {
    myResult2AISObjectMap[theObject] = theAIS;
    bool aCanBeShaded = canBeShaded(anAISIO);
    // In order to avoid extra closing/opening context
    if (aCanBeShaded)
      closeLocalContexts(false);
    aContext->Display(anAISIO, false);
    qDebug("### Display %i", (long)anAISIO.Access());

    aContext->SetDisplayMode(anAISIO, isShading? Shading : Wireframe, false);
    // Customization of presentation
    FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
    if (aFeature.get() != NULL) {
      GeomCustomPrsPtr aCustPrs = std::dynamic_pointer_cast<GeomAPI_ICustomPrs>(aFeature);
      if (aCustPrs.get() != NULL)
        aCustPrs->customisePresentation(theAIS);
    }
    if (aCanBeShaded) {
      openLocalContext();
      activateObjects(myActiveSelectionModes);
    }
  }
  if (isUpdateViewer)
    updateViewer();
}

void XGUI_Displayer::erase(ObjectPtr theObject, const bool isUpdateViewer)
{
  if (!isVisible(theObject))
    return;

  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return;
  AISObjectPtr anObject = myResult2AISObjectMap[theObject];
  if (anObject) {
    Handle(AIS_InteractiveObject) anAIS = anObject->impl<Handle(AIS_InteractiveObject)>();
    if (!anAIS.IsNull()) {
      aContext->Remove(anAIS, isUpdateViewer);
    }
  }
  myResult2AISObjectMap.remove(theObject);
}

void XGUI_Displayer::redisplay(ObjectPtr theObject, bool isUpdateViewer)
{
  if (!isVisible(theObject))
    return;

  AISObjectPtr aAISObj = getAISObject(theObject);
  Handle(AIS_InteractiveObject) aAISIO = aAISObj->impl<Handle(AIS_InteractiveObject)>();

  GeomPresentablePtr aPrs = std::dynamic_pointer_cast<GeomAPI_IPresentable>(theObject);
  if (aPrs) {
    AISObjectPtr aAIS_Obj = aPrs->getAISObject(aAISObj);
    if (!aAIS_Obj) {
      erase(theObject, isUpdateViewer);
      return;
    }
    if (aAIS_Obj != aAISObj) {
      myResult2AISObjectMap[theObject] = aAIS_Obj;
    }
    aAISIO = aAIS_Obj->impl<Handle(AIS_InteractiveObject)>();
  }

  if (!aAISIO.IsNull()) {
    Handle(AIS_InteractiveContext) aContext = AISContext();
    if (aContext.IsNull())
      return;
    aContext->Redisplay(aAISIO, isUpdateViewer);
  }
}

void XGUI_Displayer::deactivate(ObjectPtr theObject)
{
  if (isVisible(theObject)) {
    Handle(AIS_InteractiveContext) aContext = AISContext();
    if (aContext.IsNull())
      return;

    AISObjectPtr anObj = myResult2AISObjectMap[theObject];
    Handle(AIS_InteractiveObject) anAIS = anObj->impl<Handle(AIS_InteractiveObject)>();
    aContext->Deactivate(anAIS);
    qDebug("### Deactivate obj %i", (long)anAIS.Access());
  }
}

void XGUI_Displayer::activate(ObjectPtr theFeature)
{
  activate(theFeature, myActiveSelectionModes);
}

void XGUI_Displayer::activate(ObjectPtr theObject, const QIntList& theModes)
{
  if (isVisible(theObject)) {
    Handle(AIS_InteractiveContext) aContext = AISContext();
    if (aContext.IsNull())
      return;

    AISObjectPtr anObj = myResult2AISObjectMap[theObject];
    Handle(AIS_InteractiveObject) anAIS = anObj->impl<Handle(AIS_InteractiveObject)>();
    aContext->Deactivate(anAIS);
    aContext->Load(anAIS, -1, true);
    // In order to clear active modes list
    if (theModes.size() > 0) {
      foreach(int aMode, theModes) {
        //aContext->Load(anAIS, aMode, true);
        aContext->Activate(anAIS, aMode);
        qDebug("### 1. Activate obj %i, %i", (long)anAIS.Access(), aMode);
      }
    } else {
      //aContext->Load(anAIS, 0, true);
      aContext->Activate(anAIS);
      qDebug("### 2. Activate obj %i", (long)anAIS.Access());
    }
  }
}

void XGUI_Displayer::activateObjects(const QIntList& theModes)
{
  // In order to avoid doblications of selection modes
  QIntList aNewModes;
  foreach (int aMode, theModes) {
    if (!aNewModes.contains(aMode))
      aNewModes.append(aMode);
  }
  myActiveSelectionModes = aNewModes;
  Handle(AIS_InteractiveContext) aContext = AISContext();
  // Open local context if there is no one
  if (!aContext->HasOpenedContext()) 
    return;

  //aContext->UseDisplayedObjects();
  //myUseExternalObjects = true;

  AIS_ListOfInteractive aPrsList;
  displayedObjects(aContext, aPrsList);

  Handle(AIS_Trihedron) aTrihedron;
  AIS_ListIteratorOfListOfInteractive aLIt(aPrsList);
  Handle(AIS_InteractiveObject) anAISIO;
  for(aLIt.Initialize(aPrsList); aLIt.More(); aLIt.Next()){
    anAISIO = aLIt.Value();
    aContext->Load(anAISIO, -1, true);
    aContext->Deactivate(anAISIO);
    aTrihedron = Handle(AIS_Trihedron)::DownCast(anAISIO);
    //Deactivate trihedron which can be activated in local selector
    if (aTrihedron.IsNull()) {
      //aContext->Load(anAISIO, -1, true);
      // In order to clear active modes list
      if (myActiveSelectionModes.size() == 0) {
        //aContext->Load(anAISIO, 0, true);
        aContext->Activate(anAISIO);
        qDebug("### 2. Activate all %i", (long)anAISIO.Access());
      } else {
        foreach(int aMode, myActiveSelectionModes) {
          //aContext->Load(anAISIO, aMode, true);
          aContext->Activate(anAISIO, aMode);
          qDebug("### 1. Activate all %i, %i", (long)anAISIO.Access(), aMode);
        }
      }
    }
  }
}


void XGUI_Displayer::deactivateObjects()
{
  myActiveSelectionModes.clear();
  Handle(AIS_InteractiveContext) aContext = AISContext();
  // Open local context if there is no one
  if (!aContext->HasOpenedContext()) 
    return;

  //aContext->NotUseDisplayedObjects();
  AIS_ListOfInteractive aPrsList;
  displayedObjects(aContext, aPrsList);

  AIS_ListIteratorOfListOfInteractive aLIt;
  //Handle(AIS_Trihedron) aTrihedron;
  Handle(AIS_InteractiveObject) anAISIO;
  for(aLIt.Initialize(aPrsList); aLIt.More(); aLIt.Next()){
    anAISIO = aLIt.Value();
    aContext->Deactivate(anAISIO);
    //aTrihedron = Handle(AIS_Trihedron)::DownCast(anAISIO);
    //if (aTrihedron.IsNull()) {
    //  qDebug("### Deactivate all %i", (long)anAISIO.Access());
    //  //aContext->Activate(anAISIO);
    //}
  }
}

bool XGUI_Displayer::isActive(ObjectPtr theObject) const
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return false;
  if (!isVisible(theObject))
    return false;
    
  AISObjectPtr anObj = myResult2AISObjectMap[theObject];
  Handle(AIS_InteractiveObject) anAIS = anObj->impl<Handle(AIS_InteractiveObject)>();

  TColStd_ListOfInteger aModes;
  aContext->ActivatedModes(anAIS, aModes);
  return aModes.Extent() > 0;
}

void XGUI_Displayer::setSelected(const QObjectPtrList& theResults, const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return;
  if (aContext->HasOpenedContext()) {
    aContext->UnhilightSelected();
    aContext->ClearSelected();
    foreach(ObjectPtr aResult, theResults) {
      if (isVisible(aResult)) {
        AISObjectPtr anObj = myResult2AISObjectMap[aResult];
        Handle(AIS_InteractiveObject) anAIS = anObj->impl<Handle(AIS_InteractiveObject)>();
        if (!anAIS.IsNull())
          aContext->SetSelected(anAIS, false);
      }
    }
  } else {
    aContext->UnhilightCurrents();
    aContext->ClearCurrents();
    foreach(ObjectPtr aResult, theResults) {
      if (isVisible(aResult)) {
        AISObjectPtr anObj = myResult2AISObjectMap[aResult];
        Handle(AIS_InteractiveObject) anAIS = anObj->impl<Handle(AIS_InteractiveObject)>();
        if (!anAIS.IsNull())
          aContext->SetCurrentObject(anAIS, false);
      }
    }
  }
  if (isUpdateViewer)
    updateViewer();
}


void XGUI_Displayer::clearSelected()
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext) {
    aContext->UnhilightCurrents(false);
    aContext->ClearSelected();
  }
}

void XGUI_Displayer::eraseAll(const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return;

   foreach (AISObjectPtr aAISObj, myResult2AISObjectMap) {
     // erase an object
     Handle(AIS_InteractiveObject) anIO = aAISObj->impl<Handle(AIS_InteractiveObject)>();
     if (!anIO.IsNull())
       aContext->Remove(anIO, false);
   }
   myResult2AISObjectMap.clear();
   if (isUpdateViewer)
     updateViewer();
 }

void XGUI_Displayer::openLocalContext()
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return;
  // Open local context if there is no one
  if (!aContext->HasOpenedContext()) {
    // Preserve selected objects
    //AIS_ListOfInteractive aAisList;
    //for (aContext->InitCurrent(); aContext->MoreCurrent(); aContext->NextCurrent())
    //  aAisList.Append(aContext->Current());

    // get the filters from the global context and append them to the local context
    // a list of filters in the global context is not cleared and should be cleared here
    SelectMgr_ListOfFilter aFilters;
    aFilters.Assign(aContext->Filters());
    // it is important to remove the filters in the global context, because there is a code
    // in the closeLocalContex, which restore the global context filters
    aContext->RemoveFilters();

    //aContext->ClearCurrents();
    aContext->OpenLocalContext();
    qDebug("### Open context");
    //aContext->NotUseDisplayedObjects();

    //myUseExternalObjects = false;

    SelectMgr_ListIteratorOfListOfFilter aIt(aFilters);
    for (;aIt.More(); aIt.Next()) {
      aContext->AddFilter(aIt.Value());
    }
    // Restore selection
    //AIS_ListIteratorOfListOfInteractive aIt2(aAisList);
    //for(; aIt2.More(); aIt2.Next()) {
    //  aContext->SetSelected(aIt2.Value(), false);
    //}
  }
}

void XGUI_Displayer::closeLocalContexts(const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if ( (!aContext.IsNull()) && (aContext->HasOpenedContext()) ) {
    // Preserve selected objects
    //AIS_ListOfInteractive aAisList;
    //for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected())
    //  aAisList.Append(aContext->SelectedInteractive());

    // get the filters from the local context and append them to the global context
    // a list of filters in the local context is cleared
    SelectMgr_ListOfFilter aFilters;
    aFilters.Assign(aContext->Filters());

    //aContext->ClearSelected();
    aContext->CloseAllContexts(false);
    qDebug("### Close context");

    // Redisplay all object if they were displayed in localContext
    Handle(AIS_InteractiveObject) aAISIO;
    foreach (AISObjectPtr aAIS, myResult2AISObjectMap) {
      aAISIO = aAIS->impl<Handle(AIS_InteractiveObject)>();
      if (aContext->DisplayStatus(aAISIO) != AIS_DS_Displayed) {
        aContext->Display(aAISIO, false);
        aContext->SetDisplayMode(aAISIO, Shading, false);
      }
    }

    // Append the filters from the local selection in the global selection context
    SelectMgr_ListIteratorOfListOfFilter aIt(aFilters);
    for (;aIt.More(); aIt.Next()) {
      Handle(SelectMgr_Filter) aFilter = aIt.Value();
      aContext->AddFilter(aFilter);
    }

    if (isUpdateViewer)
      updateViewer();
    //myUseExternalObjects = false;

    // Restore selection
    //AIS_ListIteratorOfListOfInteractive aIt2(aAisList);
    //for(; aIt2.More(); aIt2.Next()) {
    //  if (aContext->IsDisplayed(aIt2.Value()))
    //    aContext->SetCurrentObject(aIt2.Value(), false);
    //}
  }
}

AISObjectPtr XGUI_Displayer::getAISObject(ObjectPtr theObject) const
{
  AISObjectPtr anIO;
  if (myResult2AISObjectMap.contains(theObject))
    anIO = myResult2AISObjectMap[theObject];
  return anIO;
}

ObjectPtr XGUI_Displayer::getObject(const AISObjectPtr& theIO) const
{
  Handle(AIS_InteractiveObject) aRefAIS = theIO->impl<Handle(AIS_InteractiveObject)>();
  return getObject(aRefAIS);
}

ObjectPtr XGUI_Displayer::getObject(const Handle(AIS_InteractiveObject)& theIO) const
{
  ObjectPtr aFeature;
  foreach (ObjectPtr anObj, myResult2AISObjectMap.keys()) {
    AISObjectPtr aAIS = myResult2AISObjectMap[anObj];
    Handle(AIS_InteractiveObject) anAIS = aAIS->impl<Handle(AIS_InteractiveObject)>();
    if (anAIS == theIO)
      return anObj;
  }
  return aFeature;
}

void XGUI_Displayer::updateViewer()
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull())
    aContext->UpdateCurrentViewer();
}

Handle(AIS_InteractiveContext) XGUI_Displayer::AISContext() const
{
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if ((!aContext.IsNull()) && (!aContext->HasOpenedContext())) {
    aContext->OpenLocalContext();
    qDebug("### Open context");
  }
  return aContext;
}

Handle(SelectMgr_AndFilter) XGUI_Displayer::GetFilter()
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (myAndFilter.IsNull() && !aContext.IsNull()) {
    myAndFilter = new SelectMgr_AndFilter();
    aContext->AddFilter(myAndFilter);
  }
  return myAndFilter;
}

void XGUI_Displayer::displayAIS(AISObjectPtr theAIS, bool isUpdate)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  Handle(AIS_InteractiveObject) anAISIO = theAIS->impl<Handle(AIS_InteractiveObject)>();
  if (!anAISIO.IsNull()) {
    aContext->Display(anAISIO, isUpdate);
    if (aContext->HasOpenedContext()) {
      //if (myUseExternalObjects) {
        if (myActiveSelectionModes.size() == 0)
          aContext->Activate(anAISIO);
        else {
          foreach(int aMode, myActiveSelectionModes) {
            aContext->Activate(anAISIO, aMode);
          }
        }
      //}
    }
  }
}

void XGUI_Displayer::eraseAIS(AISObjectPtr theAIS, const bool isUpdate)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  Handle(AIS_InteractiveObject) anAISIO = theAIS->impl<Handle(AIS_InteractiveObject)>();
  if (!anAISIO.IsNull()) {
    aContext->Remove(anAISIO, isUpdate);
  }
}


void XGUI_Displayer::setDisplayMode(ObjectPtr theObject, DisplayMode theMode, bool toUpdate)
{
  if (theMode == NoMode)
    return;

  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return;

  AISObjectPtr aAISObj = getAISObject(theObject);
  if (!aAISObj)
    return;

  Handle(AIS_InteractiveObject) aAISIO = aAISObj->impl<Handle(AIS_InteractiveObject)>();
  aContext->SetDisplayMode(aAISIO, theMode, toUpdate);
}

XGUI_Displayer::DisplayMode XGUI_Displayer::displayMode(ObjectPtr theObject) const
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return NoMode;

  AISObjectPtr aAISObj = getAISObject(theObject);
  if (!aAISObj)
    return NoMode;

  Handle(AIS_InteractiveObject) aAISIO = aAISObj->impl<Handle(AIS_InteractiveObject)>();
  return (XGUI_Displayer::DisplayMode) aAISIO->DisplayMode();
}

void XGUI_Displayer::addSelectionFilter(const Handle(SelectMgr_Filter)& theFilter)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return;
  const SelectMgr_ListOfFilter& aFilters = aContext->Filters();
  SelectMgr_ListIteratorOfListOfFilter aIt(aFilters);
  for (; aIt.More(); aIt.Next()) {
    if (theFilter.Access() == aIt.Value().Access())
      return;
  }
  GetFilter()->Add(theFilter);
}

void XGUI_Displayer::removeSelectionFilter(const Handle(SelectMgr_Filter)& theFilter)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return;
  GetFilter()->Remove(theFilter);
}

void XGUI_Displayer::removeFilters()
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return;
  GetFilter()->Clear();
}

void XGUI_Displayer::showOnly(const QObjectPtrList& theList)
{
  QObjectPtrList aDispList = myResult2AISObjectMap.keys();
  foreach(ObjectPtr aObj, aDispList) {
    if (!theList.contains(aObj))
      erase(aObj, false);
  }
  foreach(ObjectPtr aObj, theList) {
    if (!isVisible(aObj))
      display(aObj, false);
  }
  updateViewer();
}
