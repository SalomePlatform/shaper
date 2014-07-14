// File:        XGUI_Selection.cpp
// Created:     8 July 2014
// Author:      Vitaly SMETANNIKOV

#include "XGUI_Selection.h"
#include "XGUI_Workshop.h"
#include "XGUI_Displayer.h"
#include "XGUI_ViewerProxy.h"
#include "XGUI_ObjectsBrowser.h"

#include <ModelAPI_Feature.h>

#include <AIS_InteractiveContext.hxx>

#include <set>


XGUI_Selection::XGUI_Selection(XGUI_Workshop* theWorkshop)
: myWorkshop(theWorkshop)
{
}

std::list<ModuleBase_ViewerPrs> XGUI_Selection::getSelected(int theShapeTypeToSkip) const
{
  std::set<ResultPtr> aPrsFeatures;
  std::list<ModuleBase_ViewerPrs> aPresentations;

  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected()) {
    Handle(AIS_InteractiveObject) anIO = aContext->SelectedInteractive();
    TopoDS_Shape aShape = aContext->SelectedShape();

    if (theShapeTypeToSkip >= 0 && !aShape.IsNull() && aShape.ShapeType() == theShapeTypeToSkip)
      continue;

    ResultPtr aFeature = myWorkshop->displayer()->getResult(anIO);
    if (aPrsFeatures.find(aFeature) != aPrsFeatures.end())
      continue;
    Handle(SelectMgr_EntityOwner) anOwner = aContext->SelectedOwner();
    aPresentations.push_back(ModuleBase_ViewerPrs(aFeature, aShape, anOwner));
    aPrsFeatures.insert(aFeature);
  }
  return aPresentations;
}

std::list<ModuleBase_ViewerPrs> XGUI_Selection::getHighlighted(int theShapeTypeToSkip) const
{
  std::set<ResultPtr> aPrsFeatures;
  std::list<ModuleBase_ViewerPrs> aPresentations;

  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  for (aContext->InitDetected(); aContext->MoreDetected(); aContext->NextDetected()) {
    Handle(AIS_InteractiveObject) anIO = aContext->DetectedInteractive();
    TopoDS_Shape aShape = aContext->DetectedShape();
    if (theShapeTypeToSkip >= 0 && !aShape.IsNull() && aShape.ShapeType() == theShapeTypeToSkip)
      continue;

    ResultPtr aResult = myWorkshop->displayer()->getResult(anIO);
    if (aPrsFeatures.find(aResult) != aPrsFeatures.end())
      continue;
    aPresentations.push_back(ModuleBase_ViewerPrs(aResult, aShape, NULL));
    aPrsFeatures.insert(aResult);
  }

  return aPresentations;
}

QList<ObjectPtr> XGUI_Selection::selectedObjects() const
{
  return myWorkshop->objectBrowser()->selectedObjects();
}
  
QResultList XGUI_Selection::selectedResults() const
{
  QResultList aSelectedList;

  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected()) {
    Handle(AIS_InteractiveObject) anIO = aContext->SelectedInteractive();
    ResultPtr aResult = myWorkshop->displayer()->getResult(anIO);
    if (aResult)
      aSelectedList.append(aResult);
  }
  return aSelectedList;
}


//**************************************************************
QModelIndexList XGUI_Selection::selectedIndexes() const 
{ 
  return myWorkshop->objectBrowser()->selectedIndexes();
}

//**************************************************************
void XGUI_Selection::selectedAISObjects(AIS_ListOfInteractive& theList) const
{
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  theList.Clear();
  for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected())
    theList.Append(aContext->SelectedInteractive());
}

//**************************************************************
void XGUI_Selection::selectedShapes(NCollection_List<TopoDS_Shape>& theList) const
{
  theList.Clear();
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected()) {
    TopoDS_Shape aShape = aContext->SelectedShape();
    if (!aShape.IsNull())
      theList.Append(aShape);
  }
}
