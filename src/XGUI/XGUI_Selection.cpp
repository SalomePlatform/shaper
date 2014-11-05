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

QList<ModuleBase_ViewerPrs> XGUI_Selection::getSelected(int theShapeTypeToSkip) const
{
  std::set<ObjectPtr> aPrsFeatures;
  QList<ModuleBase_ViewerPrs> aPresentations;
  XGUI_Displayer* aDisplayer = myWorkshop->displayer();

  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected()) {
    ModuleBase_ViewerPrs aPrs;

    Handle(AIS_InteractiveObject) anIO = aContext->SelectedInteractive();
    aPrs.setInteractive(anIO);

    ObjectPtr aFeature = aDisplayer->getObject(anIO);
    if (aPrsFeatures.find(aFeature) == aPrsFeatures.end()) {
      aPrs.setFeature(aFeature);
      aPrsFeatures.insert(aFeature);
    }
    if (aContext->HasOpenedContext()) {
      TopoDS_Shape aShape = aContext->SelectedShape();
      if (!aShape.IsNull() && (aShape.ShapeType() != theShapeTypeToSkip))
        aPrs.setShape(aShape);
    }
    Handle(SelectMgr_EntityOwner) anOwner = aContext->SelectedOwner();
    aPrs.setOwner(anOwner);
    aPresentations.append(aPrs);
  }
  return aPresentations;
}

QList<ModuleBase_ViewerPrs> XGUI_Selection::getHighlighted(int theShapeTypeToSkip) const
{
  //std::set<ObjectPtr> aPrsFeatures;
  QList<ModuleBase_ViewerPrs> aPresentations;
  XGUI_Displayer* aDisplayer = myWorkshop->displayer();

  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  for (aContext->InitDetected(); aContext->MoreDetected(); aContext->NextDetected()) {
    ModuleBase_ViewerPrs aPrs;
    Handle(AIS_InteractiveObject) anIO = aContext->DetectedInteractive();
    aPrs.setInteractive(anIO);

    ObjectPtr aResult = aDisplayer->getObject(anIO);
    // we should not check the appearance of this feature because there can be some selected shapes
    // for one feature
    //if (aPrsFeatures.find(aResult) == aPrsFeatures.end()) {
      aPrs.setFeature(aResult);
      //aPrsFeatures.insert(aResult);
    //}
    if (aContext->HasOpenedContext()) {
      TopoDS_Shape aShape = aContext->DetectedShape();
      if (!aShape.IsNull() && aShape.ShapeType() != theShapeTypeToSkip)
        aPrs.setShape(aShape);
    }
    aPresentations.push_back(aPrs);
  }
  return aPresentations;
}

QList<ObjectPtr> XGUI_Selection::selectedObjects() const
{
  return myWorkshop->objectBrowser()->selectedObjects();
}

QList<ObjectPtr> XGUI_Selection::selectedPresentations() const
{
  QList<ObjectPtr> aSelectedList;

  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected()) {
    Handle(AIS_InteractiveObject) anIO = aContext->SelectedInteractive();
    ObjectPtr aResult = myWorkshop->displayer()->getObject(anIO);
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
void XGUI_Selection::selectedShapes(NCollection_List<TopoDS_Shape>& theList, 
                                    std::list<ObjectPtr>& theOwners) const
{
  theList.Clear();
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected()) {
    TopoDS_Shape aShape = aContext->SelectedShape();
    if (!aShape.IsNull()) {
      theList.Append(aShape);
      Handle(SelectMgr_EntityOwner) aEO = aContext->SelectedOwner();
      Handle(AIS_InteractiveObject) anObj = 
        Handle(AIS_InteractiveObject)::DownCast(aEO->Selectable());
      ObjectPtr anObject = myWorkshop->displayer()->getObject(anObj);
      theOwners.push_back(anObject);
    }
  }
}
