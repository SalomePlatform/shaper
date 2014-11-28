// File:        XGUI_Displayer.cpp
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "XGUI_Displayer.h"
#include "XGUI_Viewer.h"
#include "XGUI_Workshop.h"
#include "XGUI_ViewerProxy.h"

#include <ModelAPI_Document.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Tools.h>

#include <ModuleBase_ResultPrs.h>

#include <GeomAPI_Shape.h>
#include <GeomAPI_IPresentable.h>

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

#include <set>

const int MOUSE_SENSITIVITY_IN_PIXEL = 10;  ///< defines the local context mouse selection sensitivity

XGUI_Displayer::XGUI_Displayer(XGUI_Workshop* theWorkshop)
  : myUseExternalObjects(false), myWorkshop(theWorkshop)
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
    if (aPrs) {
      anAIS = aPrs->getAISObject(AISObjectPtr());
    } else {
      ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
      if (aResult) {
        std::shared_ptr<GeomAPI_Shape> aShapePtr = ModelAPI_Tools::shape(aResult);
        if (aShapePtr) {
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

void XGUI_Displayer::display(ObjectPtr theObject, AISObjectPtr theAIS, 
                             bool isShading, bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return;

  Handle(AIS_InteractiveObject) anAISIO = theAIS->impl<Handle(AIS_InteractiveObject)>();
  if (!anAISIO.IsNull()) {
    myResult2AISObjectMap[theObject] = theAIS;
    aContext->Display(anAISIO, false);
    aContext->SetDisplayMode(anAISIO, isShading? Shading : Wireframe, isUpdateViewer);
    if (aContext->HasOpenedContext()) {
      if (myUseExternalObjects) {
        if (myActiveSelectionModes.size() == 0)
          aContext->Activate(anAISIO);
        else {
          foreach(int aMode, myActiveSelectionModes) {
            aContext->Activate(anAISIO, aMode);
          }
        }
      }
    }
  }
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
  }
}

void XGUI_Displayer::activate(ObjectPtr theFeature)
{
  QIntList aModes;
  activate(theFeature, aModes);
}

void XGUI_Displayer::activate(ObjectPtr theObject, const QIntList& theModes)
{
  if (isVisible(theObject)) {
    Handle(AIS_InteractiveContext) aContext = AISContext();
    if (aContext.IsNull())
      return;

    AISObjectPtr anObj = myResult2AISObjectMap[theObject];
    Handle(AIS_InteractiveObject) anAIS = anObj->impl<Handle(AIS_InteractiveObject)>();
    if (aContext->HasOpenedContext()) {
      aContext->Load(anAIS, -1, true);
    }
    if (theModes.size() > 0) {
      foreach(int aMode, theModes) {
        aContext->Activate(anAIS, aMode);
      }
    } else 
      aContext->Activate(anAIS);
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

void XGUI_Displayer::stopSelection(const QObjectPtrList& theResults, const bool isStop,
                                   const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return;

  Handle(AIS_Shape) anAIS;
  QObjectPtrList::const_iterator anIt = theResults.begin(), aLast = theResults.end();
  ObjectPtr aFeature;
  for (; anIt != aLast; anIt++) {
    aFeature = *anIt;
    if (isVisible(aFeature))
      anAIS = Handle(AIS_Shape)::DownCast(
          myResult2AISObjectMap[aFeature]->impl<Handle(AIS_InteractiveObject)>());
    if (anAIS.IsNull())
      continue;

    if (isStop) {
      QColor aColor(Qt::white);
      anAIS->SetColor(
          Quantity_Color(aColor.red() / 255., aColor.green() / 255., aColor.blue() / 255.,
                         Quantity_TOC_RGB));
      anAIS->Redisplay();
    } else {
      QColor aColor(Qt::red);
      anAIS->SetColor(
          Quantity_Color(aColor.red() / 255., aColor.green() / 255., aColor.blue() / 255.,
                         Quantity_TOC_RGB));
      anAIS->Redisplay();
    }
  }
  if (isUpdateViewer)
    updateViewer();
}

void XGUI_Displayer::setSelected(const QObjectPtrList& theResults, const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  // we need to unhighligth objects manually in the current local context
  // in couple with the selection clear (TODO)
  Handle(AIS_LocalContext) aLocalContext = aContext->LocalContext();
  if (!aLocalContext.IsNull())
    aLocalContext->UnhilightLastDetected(myWorkshop->viewer()->activeView());

  aContext->ClearSelected();
  foreach(ObjectPtr aResult, theResults)
  {
    if (isVisible(aResult)) {
      AISObjectPtr anObj = myResult2AISObjectMap[aResult];
      Handle(AIS_InteractiveObject) anAIS = anObj->impl<Handle(AIS_InteractiveObject)>();
      if (!anAIS.IsNull())
        aContext->SetSelected(anAIS, false);
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
  Handle(AIS_InteractiveContext) ic = AISContext();
  if (ic.IsNull())
    return;

   foreach (AISObjectPtr aAISObj, myResult2AISObjectMap) {
     // erase an object
     Handle(AIS_InteractiveObject) anIO = aAISObj->impl<Handle(AIS_InteractiveObject)>();
     if (!anIO.IsNull())
       ic->Remove(anIO, false);
   }
   myResult2AISObjectMap.clear();
   if (isUpdateViewer)
     updateViewer();
 }

void XGUI_Displayer::eraseDeletedResults(const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return;

  QObjectPtrList aRemoved;
  foreach (ObjectPtr aFeature, myResult2AISObjectMap.keys()) {
    if (!aFeature || !aFeature->data() || !aFeature->data()->isValid()) {
      AISObjectPtr anObj = myResult2AISObjectMap[aFeature];
      if (!anObj)
        continue;
      Handle(AIS_InteractiveObject) anAIS = anObj->impl<Handle(AIS_InteractiveObject)>();
      if (!anAIS.IsNull()) {
        aContext->Remove(anAIS, false);
        aRemoved.append(aFeature);
      }
    }
  }
  foreach(ObjectPtr aObj, aRemoved) {
    myResult2AISObjectMap.remove(aObj);
  }

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

    SelectMgr_ListOfFilter aFilters;
    aFilters.Assign(aContext->Filters());

    aContext->ClearCurrents();
    aContext->OpenLocalContext();
    aContext->NotUseDisplayedObjects();

    myUseExternalObjects = false;
    myActiveSelectionModes.clear();

    SelectMgr_ListIteratorOfListOfFilter aIt(aFilters);
    for (;aIt.More(); aIt.Next()) {
      aContext->AddFilter(aIt.Value());
    }
    // Restore selection
    //AIS_ListIteratorOfListOfInteractive aIt(aAisList);
    //for(; aIt.More(); aIt.Next()) {
    //  if (aContext->IsDisplayed(aIt.Value()))
    //    aContext->SetSelected(aIt.Value(), false);
    //}
  }
}

void XGUI_Displayer::closeLocalContexts(const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) ic = AISContext();
  if ( (!ic.IsNull()) && (ic->HasOpenedContext()) ) {
    // Preserve selected objects
    //AIS_ListOfInteractive aAisList;
    //for (ic->InitSelected(); ic->MoreSelected(); ic->NextSelected())
    //  aAisList.Append(ic->SelectedInteractive());

    ic->ClearSelected();
    ic->CloseAllContexts(false);

    // Redisplay all object if they were displayed in localContext
    Handle(AIS_InteractiveObject) aAISIO;
    foreach (AISObjectPtr aAIS, myResult2AISObjectMap) {
      aAISIO = aAIS->impl<Handle(AIS_InteractiveObject)>();
      if (ic->DisplayStatus(aAISIO) != AIS_DS_Displayed) {
        ic->Display(aAISIO, false);
        ic->SetDisplayMode(aAISIO, Shading, false);
      }
    }
    if (isUpdateViewer)
      updateViewer();
    myUseExternalObjects = false;
    myActiveSelectionModes.clear();

    // Restore selection
    //AIS_ListIteratorOfListOfInteractive aIt(aAisList);
    //for(; aIt.More(); aIt.Next()) {
    //  if (ic->IsDisplayed(aIt.Value()))
    //    ic->SetCurrentObject(aIt.Value(), false);
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
  Handle(AIS_InteractiveContext) ic = AISContext();
  if (!ic.IsNull())
    ic->UpdateCurrentViewer();
}

Handle(AIS_InteractiveContext) XGUI_Displayer::AISContext() const
{
  return myWorkshop->viewer()->AISContext();
}

void XGUI_Displayer::displayAIS(AISObjectPtr theAIS, bool isUpdate)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  Handle(AIS_InteractiveObject) anAISIO = theAIS->impl<Handle(AIS_InteractiveObject)>();
  if (!anAISIO.IsNull()) {
    aContext->Display(anAISIO, isUpdate);
    if (aContext->HasOpenedContext()) {
      if (myUseExternalObjects) {
        if (myActiveSelectionModes.size() == 0)
          aContext->Activate(anAISIO);
        else {
          foreach(int aMode, myActiveSelectionModes) {
            aContext->Activate(anAISIO, aMode);
          }
        }
      }
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

void XGUI_Displayer::activateObjectsOutOfContext(const QIntList& theModes)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  // Open local context if there is no one
  if (!aContext->HasOpenedContext()) 
    return;

  aContext->UseDisplayedObjects();
  myUseExternalObjects = true;
  myActiveSelectionModes = theModes;

  //Deactivate trihedron which can be activated in local selector
  AIS_ListOfInteractive aPrsList;
  aContext->DisplayedObjects(aPrsList, true);

  Handle(AIS_Trihedron) aTrihedron;
  AIS_ListIteratorOfListOfInteractive aLIt(aPrsList);
  for(; aLIt.More(); aLIt.Next()){
    aTrihedron = Handle(AIS_Trihedron)::DownCast(aLIt.Value());
    if (!aTrihedron.IsNull()) {
      aContext->Deactivate(aTrihedron);
      break;
    }
  }

  Handle(AIS_InteractiveObject) anAISIO;
  foreach (AISObjectPtr aAIS, myResult2AISObjectMap) {
  anAISIO = aAIS->impl<Handle(AIS_InteractiveObject)>();
    aContext->Load(anAISIO, -1, true);
    if (theModes.size() == 0)
      aContext->Activate(anAISIO);
    else {
      foreach(int aMode, theModes) {
        aContext->Activate(anAISIO, aMode);
      }
    }
  }
}


void XGUI_Displayer::deactivateObjectsOutOfContext()
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  // Open local context if there is no one
  if (!aContext->HasOpenedContext()) 
    return;

  aContext->NotUseDisplayedObjects();
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

void XGUI_Displayer::setSelectionModes(const QIntList& theModes)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return;
  if (!aContext->HasOpenedContext())
    return;
  // Clear previous mode
  const TColStd_ListOfInteger& aModes = aContext->ActivatedStandardModes();
  if (!aModes.IsEmpty()) {
    TColStd_ListOfInteger aMModes;
    aMModes.Assign(aModes);
    TColStd_ListIteratorOfListOfInteger it(aMModes);
    for(; it.More(); it.Next()) {
      aContext->DeactivateStandardMode((TopAbs_ShapeEnum)it.Value());
    }
  }
  foreach(int aMode, theModes) {
    aContext->ActivateStandardMode((TopAbs_ShapeEnum)aMode);
  }
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
  aContext->AddFilter(theFilter);
}

void XGUI_Displayer::removeSelectionFilter(const Handle(SelectMgr_Filter)& theFilter)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return;
  aContext->RemoveFilter(theFilter);
}
