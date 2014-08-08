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

#include <GeomAPI_Shape.h>
#include <GeomAPI_IPresentable.h>

#include <AIS_InteractiveContext.hxx>
#include <AIS_LocalContext.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_DimensionSelectionMode.hxx>

#include <AIS_Shape.hxx>

#include <set>

const int MOUSE_SENSITIVITY_IN_PIXEL = 10; ///< defines the local context mouse selection sensitivity

XGUI_Displayer::XGUI_Displayer(XGUI_Workshop* theWorkshop)
{
  myWorkshop = theWorkshop;
}

XGUI_Displayer::~XGUI_Displayer()
{
}

bool XGUI_Displayer::isVisible(ObjectPtr theObject)
{
  return myResult2AISObjectMap.find(theObject) != myResult2AISObjectMap.end();
}

void XGUI_Displayer::display(ObjectPtr theObject, bool isUpdateViewer)
{
  if (isVisible(theObject)) {
    redisplay(theObject, isUpdateViewer);
  } else {
    boost::shared_ptr<GeomAPI_AISObject> anAIS;

    GeomPresentablePtr aPrs = boost::dynamic_pointer_cast<GeomAPI_IPresentable>(theObject);
    if (aPrs) {
      anAIS = aPrs->getAISObject(boost::shared_ptr<GeomAPI_AISObject>());
    } else {
      ResultPtr aResult = boost::dynamic_pointer_cast<ModelAPI_Result>(theObject);
      if (aResult) {
        boost::shared_ptr<GeomAPI_Shape> aShapePtr = ModelAPI_Tools::shape(aResult);
        if (aShapePtr) {
          anAIS = boost::shared_ptr<GeomAPI_AISObject>(new GeomAPI_AISObject());
          anAIS->createShape(aShapePtr);
        }
      }
    }
    if (anAIS)
      display(theObject, anAIS, isUpdateViewer);
  }
}

void XGUI_Displayer::display(ObjectPtr theObject,
                             boost::shared_ptr<GeomAPI_AISObject> theAIS, bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();

  Handle(AIS_InteractiveObject) anAISIO = theAIS->impl<Handle(AIS_InteractiveObject)>();
  if (!anAISIO.IsNull()) {
    myResult2AISObjectMap[theObject] = theAIS;
    aContext->Display(anAISIO, isUpdateViewer);
  }
}



void XGUI_Displayer::erase(ObjectPtr theObject, const bool isUpdateViewer)
{
  if (!isVisible(theObject)) return;

  Handle(AIS_InteractiveContext) aContext = AISContext();
  boost::shared_ptr<GeomAPI_AISObject> anObject = myResult2AISObjectMap[theObject];
  if (anObject)  {
    Handle(AIS_InteractiveObject) anAIS = anObject->impl<Handle(AIS_InteractiveObject)>();
    if (!anAIS.IsNull()) {
      aContext->Remove(anAIS, isUpdateViewer);
    }
  }
  myResult2AISObjectMap.erase(theObject);
}


/*bool XGUI_Displayer::redisplay(ObjectPtr theObject,
                               boost::shared_ptr<GeomAPI_AISObject> theAIS,
                               const bool isUpdateViewer)
{
  bool isCreated = false;
  Handle(AIS_InteractiveObject) anAIS = 
    theAIS ? theAIS->impl<Handle(AIS_InteractiveObject)>() : Handle(AIS_InteractiveObject)();
  Handle(AIS_InteractiveContext) aContext = AISContext();
  // Open local context if there is no one
  if (!aContext->HasOpenedContext()) {
    aContext->ClearCurrents(false);
    aContext->OpenLocalContext(false /use displayed objects/, true /allow shape decomposition/);
    // set mouse sensitivity
    //aContext->SetSensitivityMode(StdSelect_SM_WINDOW);
    //aContext->SetPixelTolerance(MOUSE_SENSITIVITY_IN_PIXEL);
  }
  // display or redisplay presentation
  boost::shared_ptr<GeomAPI_AISObject> anObj = myResult2AISObjectMap[theObject];
  if (isVisible(theObject) && anObj && !anObj->empty()) {
    aContext->Redisplay(anAIS, isUpdateViewer);
    //aContext->RecomputeSelectionOnly(anAIS);
  }
  else {
    myResult2AISObjectMap[theObject] = theAIS;
    aContext->Display(anAIS, isUpdateViewer);
    isCreated = true;
  }
  return isCreated;
}*/

void XGUI_Displayer::redisplay(ObjectPtr theObject, bool isUpdateViewer)
{
  if (!isVisible(theObject))
    return;

  Handle(AIS_InteractiveObject) aAISIO;
  boost::shared_ptr<GeomAPI_AISObject> aAISObj = getAISObject(theObject);
  GeomPresentablePtr aPrs = boost::dynamic_pointer_cast<GeomAPI_IPresentable>(theObject);
  if (aPrs) {
    boost::shared_ptr<GeomAPI_AISObject> aAIS_Obj = aPrs->getAISObject(aAISObj);
    if (aAISObj && !aAIS_Obj) {
      erase(theObject, isUpdateViewer);
      return;
    }
    aAISIO = aAIS_Obj->impl<Handle(AIS_InteractiveObject)>();
  } else {
    ResultPtr aResult = boost::dynamic_pointer_cast<ModelAPI_Result>(theObject);
    if (aResult) {
      boost::shared_ptr<GeomAPI_Shape> aShapePtr = ModelAPI_Tools::shape(aResult);
      if (aShapePtr) {
        Handle(AIS_Shape) aAISShape = Handle(AIS_Shape)::DownCast(aAISObj->impl<Handle(AIS_InteractiveObject)>());
        if (!aAISShape.IsNull()) {
          aAISShape->Set(aShapePtr->impl<TopoDS_Shape>());
          aAISIO = aAISShape;
        }
      }
    }
  }
  if (!aAISIO.IsNull()) {
    Handle(AIS_InteractiveContext) aContext = AISContext();
    aContext->Redisplay(aAISIO, isUpdateViewer);
    //if (aContext->HasOpenedContext()) {
    //  aContext->Load(aAISIO, -1, true/*allow decomposition*/);
    //}
  }
}

void XGUI_Displayer::activateInLocalContext(ObjectPtr theResult,
                                         const std::list<int>& theModes, const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  // Open local context if there is no one
  if (!aContext->HasOpenedContext()) {
    aContext->ClearCurrents(false);
    aContext->OpenLocalContext(false/*use displayed objects*/, true/*allow shape decomposition*/);
  }
  // display or redisplay presentation
  Handle(AIS_InteractiveObject) anAIS;
  if (isVisible(theResult)) {
    boost::shared_ptr<GeomAPI_AISObject> anObj = myResult2AISObjectMap[theResult];
    if (anObj)
      anAIS = anObj->impl<Handle(AIS_InteractiveObject)>();
  }

  // Activate selection of objects from prs
  if (!anAIS.IsNull()) {
    aContext->ClearSelected(false); // ToCheck
    //aContext->upClearSelected(false); // ToCheck
    aContext->Load(anAIS, -1, true/*allow decomposition*/);
    aContext->Deactivate(anAIS);

    std::list<int>::const_iterator anIt = theModes.begin(), aLast = theModes.end();
    for (; anIt != aLast; anIt++) {
      aContext->Activate(anAIS, (*anIt));
    }
  }

  if (isUpdateViewer)
    updateViewer();
}

void XGUI_Displayer::deactivate(ObjectPtr theObject)
{
  if (isVisible(theObject)) {
    Handle(AIS_InteractiveContext) aContext = AISContext();

     boost::shared_ptr<GeomAPI_AISObject> anObj = myResult2AISObjectMap[theObject];
     Handle(AIS_InteractiveObject) anAIS = anObj->impl<Handle(AIS_InteractiveObject)>();
     aContext->Deactivate(anAIS);
  }
}

void XGUI_Displayer::activate(ObjectPtr theObject)
{
  if (isVisible(theObject)) {
    Handle(AIS_InteractiveContext) aContext = AISContext();

     boost::shared_ptr<GeomAPI_AISObject> anObj = myResult2AISObjectMap[theObject];
     Handle(AIS_InteractiveObject) anAIS = anObj->impl<Handle(AIS_InteractiveObject)>();
     aContext->Activate(anAIS);
  }
}

void XGUI_Displayer::stopSelection(const QList<ObjectPtr>& theResults, const bool isStop,
                                   const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();

  Handle(AIS_Shape) anAIS;
  QList<ObjectPtr>::const_iterator anIt = theResults.begin(), aLast = theResults.end();
  ObjectPtr aFeature;
  for (; anIt != aLast; anIt++) {
    aFeature = *anIt;
    if (isVisible(aFeature))
      anAIS = Handle(AIS_Shape)::DownCast(myResult2AISObjectMap[aFeature]->impl<Handle(AIS_InteractiveObject)>());
    if (anAIS.IsNull())
      continue;

    if (isStop) {
      QColor aColor(Qt::white);
      anAIS->SetColor(Quantity_Color(aColor.red()/255., aColor.green()/255., aColor.blue()/255., Quantity_TOC_RGB));
      anAIS->Redisplay();
    }
    else {
      QColor aColor(Qt::red);
      anAIS->SetColor(Quantity_Color(aColor.red()/255., aColor.green()/255., aColor.blue()/255., Quantity_TOC_RGB));
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
  foreach(ObjectPtr aResult, theResults) {
    if (myResult2AISObjectMap.find(aResult) == myResult2AISObjectMap.end()) 
      continue;

    boost::shared_ptr<GeomAPI_AISObject> anObj = myResult2AISObjectMap[aResult];
    if (anObj) {
      Handle(AIS_InteractiveObject) anAIS = anObj->impl<Handle(AIS_InteractiveObject)>();
      if (!anAIS.IsNull())
        aContext->AddOrRemoveSelected(anAIS, false);
    }
  }
  if (isUpdateViewer)
    updateViewer();
}


/*void XGUI_Displayer::EraseAll(const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) ic = AISContext();

  AIS_ListOfInteractive aList;
  ic->DisplayedObjects(aList);
  AIS_ListIteratorOfListOfInteractive anIter(aList);
  for (; anIter.More(); anIter.Next()) {
    if ((anIter.Value()->DynamicType() == STANDARD_TYPE(AIS_Trihedron)))
      continue;

    // erase an object
    Handle(AIS_InteractiveObject) anIO = anIter.Value();
    ic->Erase(anIO, false);
  }
  myResult2AISObjectMap.clear();
  if (isUpdateViewer)
    updateViewer();
}*/

void XGUI_Displayer::eraseDeletedResults(const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();

  ResultToAISMap::const_iterator aFIt = myResult2AISObjectMap.begin(),
                                 aFLast = myResult2AISObjectMap.end();
  std::list<ObjectPtr> aRemoved;
  for (; aFIt != aFLast; aFIt++)
  {
    ObjectPtr aFeature = (*aFIt).first;
    if (!aFeature || !aFeature->data() || !aFeature->data()->isValid()) {
      boost::shared_ptr<GeomAPI_AISObject> anObj = (*aFIt).second;
      if (!anObj) continue;
      Handle(AIS_InteractiveObject) anAIS = anObj->impl<Handle(AIS_InteractiveObject)>();
      if (!anAIS.IsNull()) {
        aContext->Remove(anAIS, false);
        aRemoved.push_back(aFeature);
      }
    }
  }
  std::list<ObjectPtr>::const_iterator anIt = aRemoved.begin(),
                                                                 aLast = aRemoved.end();
  for (; anIt != aLast; anIt++) {
    myResult2AISObjectMap.erase(myResult2AISObjectMap.find(*anIt));
  }

  if (isUpdateViewer)
    updateViewer();
}

void XGUI_Displayer::closeLocalContexts(const bool isUpdateViewer)
{
  closeAllContexts(true);
}

boost::shared_ptr<GeomAPI_AISObject> XGUI_Displayer::getAISObject(ObjectPtr theObject) const
{
  boost::shared_ptr<GeomAPI_AISObject> anIO;
  if (myResult2AISObjectMap.find(theObject) != myResult2AISObjectMap.end())
    anIO = (myResult2AISObjectMap.find(theObject))->second;
  return anIO;
}

ObjectPtr XGUI_Displayer::getObject(Handle(AIS_InteractiveObject) theIO) const
{
  ObjectPtr aFeature;
  ResultToAISMap::const_iterator aFIt = myResult2AISObjectMap.begin(),
                                 aFLast = myResult2AISObjectMap.end();
  for (; aFIt != aFLast && !aFeature; aFIt++) {
    boost::shared_ptr<GeomAPI_AISObject> anObj = (*aFIt).second;
    if (!anObj) continue;
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


void XGUI_Displayer::display(boost::shared_ptr<GeomAPI_AISObject> theAIS, bool isUpdate)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  Handle(AIS_InteractiveObject) anAISIO = theAIS->impl<Handle(AIS_InteractiveObject)>();
  if (!anAISIO.IsNull())
    aContext->Display(anAISIO, isUpdate);
}

void XGUI_Displayer::erase(boost::shared_ptr<GeomAPI_AISObject> theAIS, const bool isUpdate)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  Handle(AIS_InteractiveObject) anAISIO = theAIS->impl<Handle(AIS_InteractiveObject)>();
  if (!anAISIO.IsNull()) {
    aContext->Remove(anAISIO, isUpdate);
  }
}

