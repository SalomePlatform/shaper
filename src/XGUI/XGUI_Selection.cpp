// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

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

#include <SelectMgr_Selection.hxx>
#include <SelectBasics_SensitiveEntity.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

#include <set>

XGUI_Selection::XGUI_Selection(XGUI_Workshop* theWorkshop)
    : myWorkshop(theWorkshop)
{
}

QList<ModuleBase_ViewerPrs> XGUI_Selection::getSelected(int theShapeTypeToSkip) const
{
  QList<long> aSelectedIds; // Remember of selected address in order to avoid duplicates

  QList<ModuleBase_ViewerPrs> aPresentations;
  XGUI_Displayer* aDisplayer = myWorkshop->displayer();

  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (aContext->HasOpenedContext()) {
    for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected()) {
      ModuleBase_ViewerPrs aPrs;
      Handle(AIS_InteractiveObject) anIO = aContext->SelectedInteractive();
      if (aSelectedIds.contains((long)anIO.Access()))
        continue;
    
      aSelectedIds.append((long)anIO.Access());
      aPrs.setInteractive(anIO);

      ObjectPtr aFeature = aDisplayer->getObject(anIO);
      // we should not check the appearance of this feature because there can be some selected shapes
      // for one feature
      TopoDS_Shape aShape = aContext->SelectedShape();
      if (!aShape.IsNull() && (aShape.ShapeType() != theShapeTypeToSkip))
        aPrs.setShape(aShape);
      Handle(SelectMgr_EntityOwner) anOwner = aContext->SelectedOwner();
      aPrs.setOwner(anOwner);
      aPrs.setFeature(aFeature);
      aPresentations.append(aPrs);
    }
  } else {
    for (aContext->InitCurrent(); aContext->MoreCurrent(); aContext->NextCurrent()) {
      ModuleBase_ViewerPrs aPrs;
      Handle(AIS_InteractiveObject) anIO = aContext->Current();
      if (aSelectedIds.contains((long)anIO.Access()))
        continue;
    
      aSelectedIds.append((long)anIO.Access());
      aPrs.setInteractive(anIO);

      ObjectPtr aFeature = aDisplayer->getObject(anIO);
      aPrs.setFeature(aFeature);
      aPresentations.append(aPrs);
    }
  }
  return aPresentations;
}

QList<ModuleBase_ViewerPrs> XGUI_Selection::getHighlighted(int theShapeTypeToSkip) const
{
  QList<long> aSelectedIds; // Remember of selected address in order to avoid duplicates
  QList<ModuleBase_ViewerPrs> aPresentations;
  XGUI_Displayer* aDisplayer = myWorkshop->displayer();

  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  for (aContext->InitDetected(); aContext->MoreDetected(); aContext->NextDetected()) {
    ModuleBase_ViewerPrs aPrs;
    Handle(AIS_InteractiveObject) anIO = aContext->DetectedInteractive();
    if (aSelectedIds.contains((long)anIO.Access()))
      continue;
    
    aSelectedIds.append((long)anIO.Access());
    aPrs.setInteractive(anIO);

    ObjectPtr aResult = aDisplayer->getObject(anIO);
    // we should not check the appearance of this feature because there can be some selected shapes
    // for one feature
    aPrs.setFeature(aResult);
    if (aContext->HasOpenedContext()) {
      TopoDS_Shape aShape = aContext->DetectedShape();
      if (!aShape.IsNull() && aShape.ShapeType() != theShapeTypeToSkip)
        aPrs.setShape(aShape);
    }
    aPresentations.push_back(aPrs);
  }
  return aPresentations;
}

QObjectPtrList XGUI_Selection::selectedObjects() const
{
  return myWorkshop->objectBrowser()->selectedObjects();
}

QObjectPtrList XGUI_Selection::selectedPresentations() const
{
  QObjectPtrList aSelectedList;

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
      if (!aEO.IsNull()) {
        Handle(AIS_InteractiveObject) anObj = 
          Handle(AIS_InteractiveObject)::DownCast(aEO->Selectable());
        ObjectPtr anObject = myWorkshop->displayer()->getObject(anObj);
        theOwners.push_back(anObject);
      }
    }
  }
}

//**************************************************************
void XGUI_Selection::selectedOwners(SelectMgr_IndexedMapOfOwner& theSelectedOwners) const
{
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();

  for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected()) {
    theSelectedOwners.Add(aContext->SelectedOwner());
  }
}

//**************************************************************
void XGUI_Selection::entityOwners(const Handle(AIS_InteractiveObject)& theObject,
                                  SelectMgr_IndexedMapOfOwner& theOwners) const
{
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();

  if (theObject.IsNull() || aContext.IsNull())
    return;

  TColStd_ListOfInteger aModes;
  aContext->ActivatedModes(theObject, aModes);

  TColStd_ListIteratorOfListOfInteger anIt(aModes);
  for (; anIt.More(); anIt.Next()) {
    int aMode = anIt.Value();
    if (!theObject->HasSelection(aMode))
      continue;

    Handle(SelectMgr_Selection) aSelection = theObject->Selection(aMode);
    for (aSelection->Init(); aSelection->More(); aSelection->Next()) {
      Handle(SelectBasics_SensitiveEntity) anEntity = aSelection->Sensitive();
      if (anEntity.IsNull())
        continue;
      Handle(SelectMgr_EntityOwner) anOwner =
        Handle(SelectMgr_EntityOwner)::DownCast(anEntity->OwnerId());
      if (!anOwner.IsNull())
        theOwners.Add(anOwner);
    }
  }
}
