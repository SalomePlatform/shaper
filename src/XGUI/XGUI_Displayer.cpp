// File:        XGUI_Displayer.cpp
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "XGUI_Displayer.h"
#include "XGUI_Viewer.h"

#include <ModelAPI_Document.h>

#include <AIS_InteractiveContext.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>

#include <AIS_Shape.hxx>

XGUI_Displayer::XGUI_Displayer(XGUI_Viewer* theViewer)
: myViewer(theViewer)
{
}

XGUI_Displayer::~XGUI_Displayer()
{
}

void XGUI_Displayer::Display(boost::shared_ptr<ModelAPI_Feature> theFeature,
                             const bool isUpdateViewer)
{
}

void XGUI_Displayer::Display(boost::shared_ptr<ModelAPI_Feature> theFeature,
                             const TopoDS_Shape& theShape, const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = myViewer->AISContext();

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
  Handle(AIS_InteractiveContext) aContext = myViewer->AISContext();
  for (; anIt != aLast; anIt++) {
    Handle(AIS_InteractiveObject) anAIS = *anIt;
    Handle(AIS_Shape) anAISShape = Handle(AIS_Shape)::DownCast(anAIS);
    if (anAISShape.IsNull())
      continue;
      aContext->Erase(anAISShape);
  }

  if (isUpdateViewer)
    aContext->UpdateCurrentViewer();
}

void XGUI_Displayer::LocalSelection(boost::shared_ptr<ModelAPI_Feature> theFeature,
                                    const TopoDS_Shape& theShape,
                                    const int theMode, const bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = myViewer->AISContext();

  Handle(AIS_Shape) anAIS = new AIS_Shape(theShape);
  std::vector<Handle(AIS_InteractiveObject)> aDispAIS;
  if (myFeature2AISObjectMap.find(theFeature) != myFeature2AISObjectMap.end()) {
    aDispAIS = myFeature2AISObjectMap[theFeature];
  }
  aDispAIS.push_back(anAIS);
  myFeature2AISObjectMap[theFeature] = aDispAIS;
  aContext->Display(anAIS, Standard_False);

  AIS_ListOfInteractive anAISList;
  anAISList.Append(anAIS);
  myViewer->setLocalSelection(anAISList, theMode, true);
}

void XGUI_Displayer::GlobalSelection(const bool isUpdateViewer)
{
  myViewer->setGlobalSelection(true);
}

