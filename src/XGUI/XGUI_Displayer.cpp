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
  std::vector<Handle(AIS_InteractiveObject)> aDispAIS;
  if (myFeature2AISObjectMap.find(theFeature) != myFeature2AISObjectMap.end()) {
    aDispAIS = myFeature2AISObjectMap[theFeature];
  }
  aDispAIS.push_back(anAIS);
  myFeature2AISObjectMap[theFeature] = aDispAIS;

  aContext->Display(anAIS, Standard_False);

  if (isUpdateViewer)
    aContext->UpdateCurrentViewer();
}

void XGUI_Displayer::Erase(boost::shared_ptr<ModelAPI_Feature> theFeature,
                           const bool isUpdateViewer)
{
  if (myFeature2AISObjectMap.find(theFeature) == myFeature2AISObjectMap.end())
    return;

  std::vector<Handle(AIS_InteractiveObject)> aDispAIS = myFeature2AISObjectMap[theFeature];
  std::vector<Handle(AIS_InteractiveObject)>::const_iterator anIt = aDispAIS.begin(),
                                                             aLast = aDispAIS.end();
  Handle(AIS_InteractiveContext) aContext = AISContext();
  for (; anIt != aLast; anIt++) {
    Handle(AIS_InteractiveObject) anAIS = *anIt;
    Handle(AIS_Shape) anAISShape = Handle(AIS_Shape)::DownCast(anAIS);
    if (anAISShape.IsNull())
      continue;
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
  
  if (IsVisible(theFeature)) {
    Erase(theFeature, false);
  }

  Handle(AIS_Shape) anAIS = new AIS_Shape(theShape);
  std::vector<Handle(AIS_InteractiveObject)> aDispAIS;
  if (myFeature2AISObjectMap.find(theFeature) != myFeature2AISObjectMap.end()) {
    aDispAIS = myFeature2AISObjectMap[theFeature];
  }
  aDispAIS.push_back(anAIS);
  myFeature2AISObjectMap[theFeature] = aDispAIS;

  Handle(AIS_InteractiveContext) ic = AISContext();

  // Open local context if there is no one
  if (!ic->HasOpenedContext()) {
    ic->ClearCurrents(false);
    ic->OpenLocalContext(false/*use displayed objects*/, /*true*/false/*use displayed objects*/,
                         true/*allow shape decomposition*/);
  }
  // Activate selection of objects from prs
  if (!anAIS.IsNull()) {
    if (anAIS->IsKind(STANDARD_TYPE(AIS_Shape))) {
      ic->Display(anAIS, false);
      ic->Load(anAIS, -1, true/*allow decomposition*/);
      std::list<int>::const_iterator anIt = theModes.begin(), aLast = theModes.end();
      for (; anIt != aLast; anIt++) 
        ic->Activate(anAIS, AIS_Shape::SelectionMode((TopAbs_ShapeEnum)*anIt));
    }
  }
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

Handle(AIS_InteractiveContext) XGUI_Displayer::AISContext() const 
{ 
  return myWorkshop->viewer()->AISContext(); 
}
