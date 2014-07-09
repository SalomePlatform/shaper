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
  std::set<FeaturePtr> aPrsFeatures;
  std::list<ModuleBase_ViewerPrs> aPresentations;

  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected()) {
    Handle(AIS_InteractiveObject) anIO = aContext->SelectedInteractive();
    TopoDS_Shape aShape = aContext->SelectedShape();

    if (theShapeTypeToSkip >= 0 && !aShape.IsNull() && aShape.ShapeType() == theShapeTypeToSkip)
      continue;

    FeaturePtr aFeature = myWorkshop->displayer()->getFeature(anIO);
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
  std::set<FeaturePtr > aPrsFeatures;
  std::list<ModuleBase_ViewerPrs> aPresentations;

  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  for (aContext->InitDetected(); aContext->MoreDetected(); aContext->NextDetected()) {
    Handle(AIS_InteractiveObject) anIO = aContext->DetectedInteractive();
    TopoDS_Shape aShape = aContext->DetectedShape();
    if (theShapeTypeToSkip >= 0 && !aShape.IsNull() && aShape.ShapeType() == theShapeTypeToSkip)
      continue;

    FeaturePtr aFeature = myWorkshop->displayer()->getFeature(anIO);
    if (aPrsFeatures.find(aFeature) != aPrsFeatures.end())
      continue;
    aPresentations.push_back(ModuleBase_ViewerPrs(aFeature, aShape, NULL));
    aPrsFeatures.insert(aFeature);
  }

  return aPresentations;
}

QFeatureList XGUI_Selection::selectedFeatures() const
{
  return myWorkshop->objectBrowser()->selectedFeatures();
  //QFeatureList aSelectedList;

  //Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  //for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected()) {
  //  Handle(AIS_InteractiveObject) anIO = aContext->SelectedInteractive();
  //  FeaturePtr aFeature = myWorkshop->displayer()->getFeature(anIO);
  //  if (aFeature)
  //    aSelectedList.append(aFeature);
  //}
  //return aSelectedList;
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
