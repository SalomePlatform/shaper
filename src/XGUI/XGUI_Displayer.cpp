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

#include <set>

const int MOUSE_SENSITIVITY_IN_PIXEL = 10;  ///< defines the local context mouse selection sensitivity

XGUI_Displayer::XGUI_Displayer(XGUI_Workshop* theWorkshop)
{
  myWorkshop = theWorkshop;
}

XGUI_Displayer::~XGUI_Displayer()
{
}

bool XGUI_Displayer::isVisible(ObjectPtr theObject) const
{
  return myResult2AISObjectMap.find(theObject) != myResult2AISObjectMap.end();
}

void XGUI_Displayer::display(ObjectPtr theObject, bool isUpdateViewer)
{
  if (isVisible(theObject)) {
    redisplay(theObject, isUpdateViewer);
  } else {
    AISObjectPtr anAIS;

    GeomPresentablePtr aPrs = boost::dynamic_pointer_cast<GeomAPI_IPresentable>(theObject);
    bool isShading = false;
    if (aPrs) {
      anAIS = aPrs->getAISObject(AISObjectPtr());
    } else {
      ResultPtr aResult = boost::dynamic_pointer_cast<ModelAPI_Result>(theObject);
      if (aResult) {
        boost::shared_ptr<GeomAPI_Shape> aShapePtr = ModelAPI_Tools::shape(aResult);
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
  myResult2AISObjectMap.erase(theObject);
}

void XGUI_Displayer::redisplay(ObjectPtr theObject, bool isUpdateViewer)
{
  if (!isVisible(theObject))
    return;

  AISObjectPtr aAISObj = getAISObject(theObject);
  Handle(AIS_InteractiveObject) aAISIO = aAISObj->impl<Handle(AIS_InteractiveObject)>();

  GeomPresentablePtr aPrs = boost::dynamic_pointer_cast<GeomAPI_IPresentable>(theObject);
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
    
  AISObjectPtr anObj = myResult2AISObjectMap.at(theObject);
  Handle(AIS_InteractiveObject) anAIS = anObj->impl<Handle(AIS_InteractiveObject)>();

  TColStd_ListOfInteger aModes;
  aContext->ActivatedModes(anAIS, aModes);
  return aModes.Extent() > 0;
}

void XGUI_Displayer::stopSelection(const QList<ObjectPtr>& theResults, const bool isStop,
                                   const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return;

  Handle(AIS_Shape) anAIS;
  QList<ObjectPtr>::const_iterator anIt = theResults.begin(), aLast = theResults.end();
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

void XGUI_Displayer::setSelected(const QList<ObjectPtr>& theResults, const bool isUpdateViewer)
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
    if (myResult2AISObjectMap.find(aResult) == myResult2AISObjectMap.end())
      continue;

    AISObjectPtr anObj = myResult2AISObjectMap[aResult];
    if (anObj) {
      Handle(AIS_InteractiveObject) anAIS = anObj->impl<Handle(AIS_InteractiveObject)>();
      if (!anAIS.IsNull())
        aContext->AddOrRemoveSelected(anAIS, false);
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

   ResultToAISMap::iterator aIt;
   for (aIt = myResult2AISObjectMap.begin(); aIt != myResult2AISObjectMap.end(); aIt++) {
     // erase an object
     AISObjectPtr aAISObj = (*aIt).second;
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

  ResultToAISMap::const_iterator aFIt = myResult2AISObjectMap.begin(), aFLast =
      myResult2AISObjectMap.end();
  std::list<ObjectPtr> aRemoved;
  for (; aFIt != aFLast; aFIt++) {
    ObjectPtr aFeature = (*aFIt).first;
    if (!aFeature || !aFeature->data() || !aFeature->data()->isValid()) {
      AISObjectPtr anObj = (*aFIt).second;
      if (!anObj)
        continue;
      Handle(AIS_InteractiveObject) anAIS = anObj->impl<Handle(AIS_InteractiveObject)>();
      if (!anAIS.IsNull()) {
        aContext->Remove(anAIS, false);
        aRemoved.push_back(aFeature);
      }
    }
  }
  std::list<ObjectPtr>::const_iterator anIt = aRemoved.begin(), aLast = aRemoved.end();
  for (; anIt != aLast; anIt++) {
    myResult2AISObjectMap.erase(myResult2AISObjectMap.find(*anIt));
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
    aContext->ClearCurrents(false);
    //aContext->OpenLocalContext(false/*use displayed objects*/, true/*allow shape decomposition*/);
    aContext->OpenLocalContext();
    aContext->NotUseDisplayedObjects();
  }
}

void XGUI_Displayer::closeLocalContexts(const bool isUpdateViewer)
{
  AISContext()->ClearSelected(false);
  closeAllContexts(true);
}

AISObjectPtr XGUI_Displayer::getAISObject(ObjectPtr theObject) const
{
  AISObjectPtr anIO;
  if (myResult2AISObjectMap.find(theObject) != myResult2AISObjectMap.end())
    anIO = (myResult2AISObjectMap.find(theObject))->second;
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
  ResultToAISMap::const_iterator aFIt = myResult2AISObjectMap.begin(), aFLast =
      myResult2AISObjectMap.end();
  for (; aFIt != aFLast && !aFeature; aFIt++) {
    AISObjectPtr anObj = (*aFIt).second;
    if (!anObj)
      continue;
    Handle(AIS_InteractiveObject) anAIS = anObj->impl<Handle(AIS_InteractiveObject)>();
    if (anAIS != theIO)
      continue;
    aFeature = (*aFIt).first;
  }
  return aFeature;
}

void XGUI_Displayer::closeAllContexts(const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) ic = AISContext();
  if (!ic.IsNull()) {
    ic->CloseAllContexts(false);
    if (isUpdateViewer)
      updateViewer();
  }
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

void XGUI_Displayer::display(AISObjectPtr theAIS, bool isUpdate)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  Handle(AIS_InteractiveObject) anAISIO = theAIS->impl<Handle(AIS_InteractiveObject)>();
  if (!anAISIO.IsNull())
    aContext->Display(anAISIO, isUpdate);
}

void XGUI_Displayer::erase(AISObjectPtr theAIS, const bool isUpdate)
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
  ResultToAISMap::iterator aIt;
  Handle(AIS_InteractiveObject) anAISIO;
  for (aIt = myResult2AISObjectMap.begin(); aIt != myResult2AISObjectMap.end(); aIt++) {
    anAISIO = (*aIt).second->impl<Handle(AIS_InteractiveObject)>();
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
  aContext->AddFilter(theFilter);
}

void XGUI_Displayer::removeSelectionFilter(const Handle(SelectMgr_Filter)& theFilter)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return;
  aContext->RemoveFilter(theFilter);
}
