// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#include "XGUI_SelectionMgr.h"

#include "XGUI_Workshop.h"
#include "XGUI_ObjectsBrowser.h"
#include "XGUI_SalomeConnector.h"
#include "XGUI_ViewerProxy.h"
#include "XGUI_Displayer.h"
#include "XGUI_Selection.h"

#include <AppElements_MainWindow.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_Object.h>

#include <SelectMgr_ListIteratorOfListOfFilter.hxx>

XGUI_SelectionMgr::XGUI_SelectionMgr(XGUI_Workshop* theParent)
    : QObject(theParent),
      myWorkshop(theParent)
{
  mySelection = new XGUI_Selection(myWorkshop);
}

XGUI_SelectionMgr::~XGUI_SelectionMgr()
{
  delete mySelection;
}

//**************************************************************
void XGUI_SelectionMgr::connectViewers()
{
  connect(myWorkshop->objectBrowser(), SIGNAL(selectionChanged()), this,
          SLOT(onObjectBrowserSelection()));

  //Connect to other viewers
  connect(myWorkshop->viewer(), SIGNAL(selectionChanged()), this, SLOT(onViewerSelection()));
}

//**************************************************************
void XGUI_SelectionMgr::setSelectedOwners(const SelectMgr_IndexedMapOfOwner& theSelectedOwners,
                                          bool isUpdateViewer)
{
  SelectMgr_IndexedMapOfOwner aSelectedOwners;
  selection()->selectedOwners(aSelectedOwners);

  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  for  (Standard_Integer i = 1, n = theSelectedOwners.Extent(); i <= n; i++)  {
    Handle(SelectMgr_EntityOwner) anOwner = theSelectedOwners(i);
    if (aSelectedOwners.FindIndex(anOwner) > 0)
      continue;

    aContext->AddOrRemoveSelected(anOwner, isUpdateViewer);
  }
}

//**************************************************************
void XGUI_SelectionMgr::updateSelectedOwners(bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  const SelectMgr_ListOfFilter& aFilters = aContext->Filters();

  SelectMgr_IndexedMapOfOwner anOwnersToDeselect;

  SelectMgr_ListIteratorOfListOfFilter anIt(aFilters);
  for (; anIt.More(); anIt.Next()) {
    Handle(SelectMgr_Filter) aFilter = anIt.Value();
    for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected()) {
      Handle(SelectMgr_EntityOwner) anOwner = aContext->SelectedOwner();
      if (!aFilter->IsOk(anOwner))
        anOwnersToDeselect.Add(aContext->SelectedOwner());
    }
  }

  setSelectedOwners(anOwnersToDeselect, false);

  if (isUpdateViewer)
    aContext->UpdateCurrentViewer();
}

//**************************************************************
void XGUI_SelectionMgr::onObjectBrowserSelection()
{
  QObjectPtrList aObjects = myWorkshop->objectBrowser()->selectedObjects();
  XGUI_Displayer* aDisplayer = myWorkshop->displayer();
  aDisplayer->setSelected(aObjects);
  emit selectionChanged();
}

//**************************************************************
void XGUI_SelectionMgr::onViewerSelection()
{
  QObjectPtrList aFeatures;
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected()) {
    Handle(AIS_InteractiveObject) anIO = aContext->SelectedInteractive();
    ObjectPtr aResult = myWorkshop->displayer()->getObject(anIO);
    if (aResult)
      aFeatures.append(aResult);
  }
  bool aBlocked = myWorkshop->objectBrowser()->blockSignals(true);
  myWorkshop->objectBrowser()->setObjectsSelected(aFeatures);
  myWorkshop->objectBrowser()->blockSignals(aBlocked);

  emit selectionChanged();
}

//**************************************************************
/*QFeatureList XGUI_SelectionMgr::selectedFeatures() const 
 { 
 return myWorkshop->objectBrowser()->selectedFeatures(); 
 }

 //**************************************************************
 QModelIndexList XGUI_SelectionMgr::selectedIndexes() const 
 { 
 return myWorkshop->objectBrowser()->selectedIndexes();
 }

 //**************************************************************
 void XGUI_SelectionMgr::selectedAISObjects(AIS_ListOfInteractive& theList) const
 {
 Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
 theList.Clear();
 for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected())
 theList.Append(aContext->SelectedInteractive());
 }

 //**************************************************************
 void XGUI_SelectionMgr::selectedShapes(NCollection_List<TopoDS_Shape>& theList) const
 {
 theList.Clear();
 Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
 for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected()) {
 TopoDS_Shape aShape = aContext->SelectedShape();
 if (!aShape.IsNull())
 theList.Append(aShape);
 }
 }*/
