// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#include "XGUI_SelectionMgr.h"

#include "XGUI_Workshop.h"
#include "XGUI_ObjectsBrowser.h"
#include "XGUI_SalomeConnector.h"
#include "XGUI_ViewerProxy.h"
#include "XGUI_Displayer.h"
#include "XGUI_Selection.h"

#ifndef HAVE_SALOME
#include <AppElements_MainWindow.h>
#endif

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
  if (!aContext.IsNull()) {
    for  (Standard_Integer i = 1, n = theSelectedOwners.Extent(); i <= n; i++)  {
      Handle(SelectMgr_EntityOwner) anOwner = theSelectedOwners(i);
      if (aSelectedOwners.FindIndex(anOwner) > 0)
        continue;

      aContext->AddOrRemoveSelected(anOwner, isUpdateViewer);
    }
  }
}

//**************************************************************
void XGUI_SelectionMgr::updateSelectedOwners(bool isUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (aContext.IsNull())
    return;

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
  QList<ModuleBase_ViewerPrsPtr> aSelectedPrs =
             myWorkshop->selector()->selection()->getSelected(ModuleBase_ISelection::Browser);

  QList<ModuleBase_ViewerPrsPtr> aTmpList = aSelectedPrs;
  ObjectPtr aObject;
  FeaturePtr aFeature;
  foreach(ModuleBase_ViewerPrsPtr aPrs, aTmpList) {
    aObject = aPrs->object();
    if (aObject.get()) {
      aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObject);
      if (aFeature.get()) {
        const std::list<std::shared_ptr<ModelAPI_Result>> aResList = aFeature->results();
        std::list<ResultPtr>::const_iterator aIt;
        for (aIt = aResList.cbegin(); aIt != aResList.cend(); ++aIt) {
          aSelectedPrs.append(std::shared_ptr<ModuleBase_ViewerPrs>(
               new ModuleBase_ViewerPrs((*aIt), GeomShapePtr(), NULL)));
        }
      }
    }
  }
  XGUI_Displayer* aDisplayer = myWorkshop->displayer();
  aDisplayer->setSelected(aSelectedPrs);
  emit selectionChanged();
}

//**************************************************************
void XGUI_SelectionMgr::onViewerSelection()
{
  SessionPtr aMgr = ModelAPI_Session::get();
  DocumentPtr anActiveDocument = aMgr->activeDocument();
  QObjectPtrList aFeatures;
  ResultPtr aResult;
  FeaturePtr aFeature;
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (!aContext.IsNull()) {
    QList<ModuleBase_ViewerPrsPtr> aPresentations = selection()->getSelected(ModuleBase_ISelection::Viewer);
    foreach(ModuleBase_ViewerPrsPtr aPrs, aPresentations) {
      if (aPrs->object().get()) {
        aFeatures.append(aPrs->object());
        if (aPrs->shape().get()) {
          aResult = std::dynamic_pointer_cast<ModelAPI_Result>(aPrs->object());
          if (aResult.get()) {
            aFeature = anActiveDocument->producedByFeature(aResult, aPrs->shape());
            if (aFeature.get() && (!aFeatures.contains(aFeature)))
              aFeatures.append(aFeature);
          }
        }
      }
    }
  }
  bool aBlocked = myWorkshop->objectBrowser()->blockSignals(true);
  myWorkshop->objectBrowser()->setObjectsSelected(aFeatures);
  myWorkshop->objectBrowser()->blockSignals(aBlocked);

  emit selectionChanged();
}

//**************************************************************
void XGUI_SelectionMgr::updateSelectionBy(const ModuleBase_ISelection::SelectionPlace& thePlace)
{
  QList<ModuleBase_ViewerPrsPtr> aSelectedPrs =
               myWorkshop->selector()->selection()->getSelected(thePlace);
  if (thePlace == ModuleBase_ISelection::Browser) {
    XGUI_Displayer* aDisplayer = myWorkshop->displayer();
    aDisplayer->setSelected(aSelectedPrs);
  }

}
//**************************************************************
void XGUI_SelectionMgr::clearSelection()
{
  QObjectPtrList aFeatures;
  bool aBlocked = myWorkshop->objectBrowser()->blockSignals(true);
  myWorkshop->objectBrowser()->setObjectsSelected(aFeatures);
  myWorkshop->objectBrowser()->blockSignals(aBlocked);
  
  QList<ModuleBase_ViewerPrsPtr> aSelectedPrs =
             myWorkshop->selector()->selection()->getSelected(ModuleBase_ISelection::Browser);

  XGUI_Displayer* aDisplayer = myWorkshop->displayer();
  aDisplayer->setSelected(aSelectedPrs);

  emit selectionChanged();
}
