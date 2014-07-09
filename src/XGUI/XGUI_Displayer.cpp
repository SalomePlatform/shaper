// File:        XGUI_Displayer.cpp
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "XGUI_Displayer.h"
#include "XGUI_Viewer.h"
#include "XGUI_Workshop.h"
#include "XGUI_ViewerProxy.h"
#include "XGUI_Tools.h"

#include <ModelAPI_Document.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>

#include <GeomAPI_Shape.h>

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

bool XGUI_Displayer::isVisible(FeaturePtr theFeature)
{
  FeaturePtr aFeature = XGUI_Tools::realFeature(theFeature);
  return myFeature2AISObjectMap.find(aFeature) != myFeature2AISObjectMap.end();
}

bool XGUI_Displayer::display(FeaturePtr theFeature, bool isUpdateViewer)
{
  FeaturePtr aFeature = XGUI_Tools::realFeature(theFeature);
  boost::shared_ptr<GeomAPI_Shape> aShapePtr = aFeature->data()->shape();

  if (aShapePtr) {
    boost::shared_ptr<GeomAPI_AISObject> anAIS(new GeomAPI_AISObject());
    anAIS->createShape(aShapePtr);
    return display(aFeature, anAIS, isUpdateViewer);
  }
  return false;
}

bool XGUI_Displayer::display(FeaturePtr theFeature,
                             boost::shared_ptr<GeomAPI_AISObject> theAIS, bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();

  Handle(AIS_InteractiveObject) anAISIO = theAIS->impl<Handle(AIS_InteractiveObject)>();
  if (!anAISIO.IsNull()) {
    myFeature2AISObjectMap[theFeature] = theAIS;
    aContext->Display(anAISIO, isUpdateViewer);
    return true;
  }
  return false;
}



void XGUI_Displayer::erase(FeaturePtr theFeature,
                           const bool isUpdateViewer)
{
  FeaturePtr aFeature = XGUI_Tools::realFeature(theFeature);

  if (myFeature2AISObjectMap.find(aFeature) == myFeature2AISObjectMap.end())
    return;

  Handle(AIS_InteractiveContext) aContext = AISContext();
  boost::shared_ptr<GeomAPI_AISObject> anObject = myFeature2AISObjectMap[aFeature];
  if (anObject)  {
    Handle(AIS_InteractiveObject) anAIS = anObject->impl<Handle(AIS_InteractiveObject)>();
    if (!anAIS.IsNull()) {
      aContext->Erase(anAIS, isUpdateViewer);
      
    }
  }
  myFeature2AISObjectMap.erase(aFeature);
}


bool XGUI_Displayer::redisplay(FeaturePtr theFeature,
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
    aContext->OpenLocalContext(false/*use displayed objects*/, true/*allow shape decomposition*/);
    // set mouse sensitivity
    //aContext->SetSensitivityMode(StdSelect_SM_WINDOW);
    //aContext->SetPixelTolerance(MOUSE_SENSITIVITY_IN_PIXEL);
  }
  // display or redisplay presentation
  boost::shared_ptr<GeomAPI_AISObject> anObj = myFeature2AISObjectMap[theFeature];
  if (isVisible(theFeature) && anObj && !anObj->empty()) {
      aContext->RecomputeSelectionOnly(anAIS);
  }
  else {
    myFeature2AISObjectMap[theFeature] = theAIS;
    aContext->Display(anAIS, false);
    isCreated = true;
  }
  if (isUpdateViewer)
    updateViewer();

  return isCreated;
}

bool XGUI_Displayer::redisplay(FeaturePtr theFeature, bool isUpdateViewer)
{
  FeaturePtr aFeature = XGUI_Tools::realFeature(theFeature);
  if (!isVisible(aFeature))
    return false;

  boost::shared_ptr<GeomAPI_Shape> aShapePtr = aFeature->data()->shape();
  if (aShapePtr) {
    boost::shared_ptr<GeomAPI_AISObject> aAISObj = getAISObject(aFeature);
    Handle(AIS_Shape) aAISShape = Handle(AIS_Shape)::DownCast(aAISObj->impl<Handle(AIS_InteractiveObject)>());
    if (!aAISShape.IsNull()) {
      aAISShape->Set(aShapePtr->impl<TopoDS_Shape>());
      AISContext()->Redisplay(aAISShape);
      return true;
    }
  }
  return false;
}

void XGUI_Displayer::activateInLocalContext(FeaturePtr theFeature,
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
  if (isVisible(theFeature))
  {
    boost::shared_ptr<GeomAPI_AISObject> anObj = myFeature2AISObjectMap[theFeature];
    if (anObj)
      anAIS = anObj->impl<Handle(AIS_InteractiveObject)>();
  }

  // Activate selection of objects from prs
  if (!anAIS.IsNull()) {
    aContext->Load(anAIS, -1, true/*allow decomposition*/);
    aContext->Deactivate(anAIS);

    std::list<int>::const_iterator anIt = theModes.begin(), aLast = theModes.end();
    for (; anIt != aLast; anIt++)
    {
      aContext->Activate(anAIS, (*anIt));
    }
  }

  if (isUpdateViewer)
    updateViewer();
}

void XGUI_Displayer::stopSelection(const QFeatureList& theFeatures, const bool isStop,
                                   const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();

  Handle(AIS_Shape) anAIS;
  QFeatureList::const_iterator anIt = theFeatures.begin(), aLast = theFeatures.end();
  FeaturePtr aFeature;
  for (; anIt != aLast; anIt++) {
    aFeature = *anIt;
    if (isVisible(aFeature))
      anAIS = Handle(AIS_Shape)::DownCast(myFeature2AISObjectMap[aFeature]->impl<Handle(AIS_InteractiveObject)>());
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

void XGUI_Displayer::setSelected(const QFeatureList& theFeatures, const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  // we need to unhighligth objects manually in the current local context
  // in couple with the selection clear (TODO)
  Handle(AIS_LocalContext) aLocalContext = aContext->LocalContext();
  if (!aLocalContext.IsNull())
    aLocalContext->UnhilightLastDetected(myWorkshop->viewer()->activeView());

  aContext->ClearSelected();
  foreach(FeaturePtr aFeature, theFeatures) {
    FeaturePtr aRFeature = XGUI_Tools::realFeature(aFeature);
    if (myFeature2AISObjectMap.find(aRFeature) == myFeature2AISObjectMap.end()) 
      return;

    boost::shared_ptr<GeomAPI_AISObject> anObj = myFeature2AISObjectMap[aRFeature];
    if (anObj)
    {
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
  myFeature2AISObjectMap.clear();
  if (isUpdateViewer)
    updateViewer();
}*/

void XGUI_Displayer::eraseDeletedFeatures(const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();

  FeatureToAISMap::const_iterator aFIt = myFeature2AISObjectMap.begin(),
                                  aFLast = myFeature2AISObjectMap.end();
  std::list<FeaturePtr> aRemoved;
  for (; aFIt != aFLast; aFIt++)
  {
    FeaturePtr aFeature = (*aFIt).first;
    if (!aFeature || !aFeature->data() || !aFeature->data()->isValid()) {
      boost::shared_ptr<GeomAPI_AISObject> anObj = (*aFIt).second;
      if (!anObj) continue;
      Handle(AIS_InteractiveObject) anAIS = anObj->impl<Handle(AIS_InteractiveObject)>();
      if (!anAIS.IsNull()) {
        aContext->Erase(anAIS, false);
        aRemoved.push_back(aFeature);
      }
    }
  }
  std::list<FeaturePtr>::const_iterator anIt = aRemoved.begin(),
                                                                 aLast = aRemoved.end();
  for (; anIt != aLast; anIt++) {
    myFeature2AISObjectMap.erase(myFeature2AISObjectMap.find(*anIt));
  }

  if (isUpdateViewer)
    updateViewer();
}

void XGUI_Displayer::closeLocalContexts(const bool isUpdateViewer)
{
  closeAllContexts(true);
}

boost::shared_ptr<GeomAPI_AISObject> XGUI_Displayer::getAISObject(
                                              FeaturePtr theFeature) const
{
  boost::shared_ptr<GeomAPI_AISObject> anIO;
  if (myFeature2AISObjectMap.find(theFeature) != myFeature2AISObjectMap.end())
    anIO = (myFeature2AISObjectMap.find(theFeature))->second;
  return anIO;
}

FeaturePtr XGUI_Displayer::getFeature(Handle(AIS_InteractiveObject) theIO) const
{
  FeaturePtr aFeature;
  FeatureToAISMap::const_iterator aFIt = myFeature2AISObjectMap.begin(),
                                  aFLast = myFeature2AISObjectMap.end();
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
