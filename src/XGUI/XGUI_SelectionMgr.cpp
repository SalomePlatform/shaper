// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#include "XGUI_SelectionMgr.h"

#include "XGUI_Workshop.h"
#include "XGUI_ObjectsBrowser.h"
#include "XGUI_SalomeConnector.h"
#include "XGUI_ViewerProxy.h"
#include "XGUI_Displayer.h"
#include "XGUI_Selection.h"
#include "XGUI_OperationMgr.h"

#ifndef HAVE_SALOME
#include <AppElements_MainWindow.h>
#endif

#include <ModelAPI_Feature.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_ResultCompSolid.h>

#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_Tools.h>

#include <SelectMgr_ListIteratorOfListOfFilter.hxx>

#ifdef TINSPECTOR
#include <VInspectorAPI_CallBack.hxx>
#endif

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
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (!aContext.IsNull()) {
    /// previous selection should be cleared, else there will be decomposition of selections:
    /// as AddOrRemoveSelected inverts current selection
    aContext->ClearSelected(false);

    for  (Standard_Integer i = 1, n = theSelectedOwners.Extent(); i <= n; i++)  {
      Handle(SelectMgr_EntityOwner) anOwner = theSelectedOwners(i);

      aContext->AddOrRemoveSelected(anOwner, isUpdateViewer);
      #ifdef TINSPECTOR
      if (myWorkshop->displayer()->getCallBack())
        myWorkshop->displayer()->getCallBack()->AddOrRemoveSelected(anOwner);
      #endif
    }
  }
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
        ResultPtr aResult;
        ResultCompSolidPtr aCompSolid;
        std::list<ResultPtr>::const_iterator aIt;
        for (aIt = aResList.cbegin(); aIt != aResList.cend(); ++aIt) {
          aResult = (*aIt);
          aSelectedPrs.append(std::shared_ptr<ModuleBase_ViewerPrs>(
            new ModuleBase_ViewerPrs(aResult, GeomShapePtr(), NULL)));
          aCompSolid = std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(aResult);
          if (aCompSolid.get()) {
            for (int i = 0; i < aCompSolid->numberOfSubs(); i++) {
              ResultBodyPtr aResult = aCompSolid->subResult(i);
              aSelectedPrs.append(std::shared_ptr<ModuleBase_ViewerPrs>(
                new ModuleBase_ViewerPrs(aResult, aResult->shape(), NULL)));
            }
          }
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
  QList<ModuleBase_ViewerPrsPtr> aValues;
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (!aContext.IsNull())
    aValues = selection()->getSelected(ModuleBase_ISelection::Viewer);

  QObjectPtrList anObjects;
  convertToObjectBrowserSelection(aValues, anObjects);

  bool aBlocked = myWorkshop->objectBrowser()->blockSignals(true);
  myWorkshop->objectBrowser()->setObjectsSelected(anObjects);
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
//**************************************************************
void XGUI_SelectionMgr::setSelected(const QList<ModuleBase_ViewerPrsPtr>& theValues)
{
  // update selection in Viewer
  XGUI_Displayer* aDisplayer = myWorkshop->displayer();
  aDisplayer->setSelected(theValues);

  // update selection in Object Browser
  bool aBlocked = myWorkshop->objectBrowser()->blockSignals(true);
  QObjectPtrList anObjects;
  convertToObjectBrowserSelection(theValues, anObjects);

  myWorkshop->objectBrowser()->setObjectsSelected(anObjects);
  myWorkshop->objectBrowser()->blockSignals(aBlocked);
}
//**************************************************************
void XGUI_SelectionMgr::convertToObjectBrowserSelection(
                                   const QList<ModuleBase_ViewerPrsPtr>& theValues,
                                   QObjectPtrList& theObjects)
{
  theObjects.clear();

  ResultPtr aResult;
  FeaturePtr aFeature;
  bool aHasOperation = (myWorkshop->operationMgr()->currentOperation() != 0);
  SessionPtr aMgr = ModelAPI_Session::get();
  DocumentPtr anActiveDocument = aMgr->activeDocument();

  foreach(ModuleBase_ViewerPrsPtr aPrs, theValues) {
    if (aPrs->object().get()) {
      if (!theObjects.contains(aPrs->object()))
        theObjects.append(aPrs->object());
      if (aPrs->shape().get() && (!aHasOperation)) {
        aResult = std::dynamic_pointer_cast<ModelAPI_Result>(aPrs->object());
        if (aResult.get()) {
          aFeature = anActiveDocument->producedByFeature(aResult, aPrs->shape());
          if (aFeature.get() && (!theObjects.contains(aFeature)))
            theObjects.append(aFeature);
        }
      }
    }
  }
}
