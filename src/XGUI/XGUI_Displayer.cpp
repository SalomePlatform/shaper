// File:        XGUI_Displayer.cpp
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "XGUI_Displayer.h"
#include "XGUI_Viewer.h"
#include "XGUI_Workshop.h"
#include "XGUI_ViewerProxy.h"

#include <ModelAPI_Document.h>

#include <AIS_InteractiveContext.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>

#include <AIS_Shape.hxx>

XGUI_Displayer::XGUI_Displayer(XGUI_Workshop* theWorkshop)
{
  myWorkshop = theWorkshop;
}

XGUI_Displayer::~XGUI_Displayer()
{
}

bool XGUI_Displayer::IsVisible(boost::shared_ptr<ModelAPI_Feature> theFeature)
{
  return myFeature2AISObjectMap.find(theFeature) != myFeature2AISObjectMap.end();
}

void XGUI_Displayer::Display(boost::shared_ptr<ModelAPI_Feature> theFeature,
                             const bool isUpdateViewer)
{
}

void XGUI_Displayer::Display(boost::shared_ptr<ModelAPI_Feature> theFeature,
                             const TopoDS_Shape& theShape, const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();

  Handle(AIS_Shape) anAIS = new AIS_Shape(theShape);
  myFeature2AISObjectMap[theFeature] = anAIS;

  aContext->Display(anAIS, Standard_False);
  if (isUpdateViewer)
    aContext->UpdateCurrentViewer();
}

boost::shared_ptr<ModelAPI_Feature> XGUI_Displayer::GetFeature(const TopoDS_Shape& theShape)
{
  boost::shared_ptr<ModelAPI_Feature> aFeature;

  FeatureToAISMap::const_iterator aFIt = myFeature2AISObjectMap.begin(),
                                  aFLast = myFeature2AISObjectMap.end();
  for (; aFIt != aFLast && !aFeature; aFIt++)
  {
    Handle(AIS_InteractiveObject) anAIS = (*aFIt).second;
    Handle(AIS_Shape) anAISShape = Handle(AIS_Shape)::DownCast(anAIS);
    if (!anAISShape.IsNull() && anAISShape->Shape() == theShape) {
      aFeature = (*aFIt).first;
    }
  }
  return aFeature;
}

std::list<XGUI_ViewerPrs> XGUI_Displayer::GetViewerPrs
                                                (const NCollection_List<TopoDS_Shape>& theShapes)
{
  std::list<XGUI_ViewerPrs> aPresentations;
  if (theShapes.IsEmpty())
    return aPresentations;

  NCollection_List<TopoDS_Shape>::Iterator anIt(theShapes);
  for (; anIt.More(); anIt.Next()) {
    const TopoDS_Shape& aShape = anIt.Value();
    if (aShape.IsNull())
      continue;
    boost::shared_ptr<ModelAPI_Feature> aFeature = GetFeature(aShape);
    aPresentations.push_back(XGUI_ViewerPrs(aFeature, aShape));
  }

  return aPresentations;
}

void XGUI_Displayer::Erase(boost::shared_ptr<ModelAPI_Feature> theFeature,
                           const bool isUpdateViewer)
{
  if (myFeature2AISObjectMap.find(theFeature) == myFeature2AISObjectMap.end())
    return;

  Handle(AIS_InteractiveContext) aContext = AISContext();
  Handle(AIS_InteractiveObject) anAIS = myFeature2AISObjectMap[theFeature];
  Handle(AIS_Shape) anAISShape = Handle(AIS_Shape)::DownCast(anAIS);
  if (!anAISShape.IsNull())
  {
    aContext->Erase(anAISShape);
  }
  myFeature2AISObjectMap.erase(theFeature);

  if (isUpdateViewer)
    aContext->UpdateCurrentViewer();
}

void XGUI_Displayer::RedisplayInLocalContext(boost::shared_ptr<ModelAPI_Feature> theFeature,
                                             const TopoDS_Shape& theShape,
                                             const std::list<int>& theModes, const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  // Open local context if there is no one
  if (!aContext->HasOpenedContext()) {
    aContext->ClearCurrents(false);
    aContext->OpenLocalContext(false/*use displayed objects*/, /*true*/false/*use displayed objects*/,
                         true/*allow shape decomposition*/);
  }
  // display or redisplay presentation
  Handle(AIS_Shape) anAIS;
  if (IsVisible(theFeature)) {
    anAIS = Handle(AIS_Shape)::DownCast(myFeature2AISObjectMap[theFeature]);
    if (!anAIS.IsNull()) {
      anAIS->Set(theShape);
      anAIS->Redisplay();
    }
  }
  else {
    anAIS = new AIS_Shape(theShape);
    myFeature2AISObjectMap[theFeature] = anAIS;
    aContext->Display(anAIS, false);
  }
  // Activate selection of objects from prs
  if (!anAIS.IsNull()) {
    aContext->Load(anAIS, -1, true/*allow decomposition*/);
    std::list<int>::const_iterator anIt = theModes.begin(), aLast = theModes.end();
    for (; anIt != aLast; anIt++)
    {
      aContext->Activate(anAIS, AIS_Shape::SelectionMode((TopAbs_ShapeEnum)*anIt));
    }
  }

  if (isUpdateViewer)
    aContext->UpdateCurrentViewer();
}

void XGUI_Displayer::EraseAll(const bool isUpdateViewer)
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
    ic->UpdateCurrentViewer();
}

void XGUI_Displayer::CloseLocalContexts(const bool isUpdateViewer)
{
  closeAllContexts(true);
}

void XGUI_Displayer::closeAllContexts(const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) ic = AISContext();
  if (!ic.IsNull()) {
    ic->CloseAllContexts(false);
    if (isUpdateViewer)
      ic->UpdateCurrentViewer();
  }
}

void XGUI_Displayer::UpdateViewer()
{
  Handle(AIS_InteractiveContext) ic = AISContext();
  if (!ic.IsNull())
    ic->UpdateCurrentViewer();
}

Handle(AIS_InteractiveContext) XGUI_Displayer::AISContext() const 
{ 
  return myWorkshop->viewer()->AISContext(); 
}
