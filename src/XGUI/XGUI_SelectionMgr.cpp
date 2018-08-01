// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

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
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_Tools.h>

#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_Tools.h>

#include <SelectMgr_ListIteratorOfListOfFilter.hxx>

#ifdef TINSPECTOR
#include <inspector/VInspectorAPI_CallBack.hxx>
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
  XGUI_Displayer* aDisplayer = myWorkshop->displayer();
  if (!myWorkshop->operationMgr()->hasOperation()) {

    QList<ModuleBase_ViewerPrsPtr> aTmpList = aSelectedPrs;
    ObjectPtr aObject;
    FeaturePtr aFeature;
    // Select all results of a selected feature in viewer
    foreach(ModuleBase_ViewerPrsPtr aPrs, aSelectedPrs) {
      aObject = aPrs->object();
      if (aObject.get()) {
        aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObject);
        if (aFeature.get()) {
          std::list<ResultPtr> allRes;
          ModelAPI_Tools::allResults(aFeature, allRes);
          for (std::list<ResultPtr>::iterator aRes = allRes.begin(); aRes != allRes.end(); aRes++) {
            aTmpList.append(std::shared_ptr<ModuleBase_ViewerPrs>(
              new ModuleBase_ViewerPrs(*aRes, GeomShapePtr(), NULL)));
          }
        }
      }
    }
    aDisplayer->setSelected(aTmpList);
  } else {
    aDisplayer->setSelected(aSelectedPrs);
  }
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
void XGUI_SelectionMgr::deselectPresentation(const Handle(AIS_InteractiveObject) theObject)
{
  NCollection_List<Handle(SelectBasics_EntityOwner)> aResultOwners;

  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected()) {
    Handle(SelectMgr_EntityOwner) anOwner = aContext->SelectedOwner();
    if (anOwner.IsNull()) // TODO: check why it is possible
      continue;
    if (anOwner->Selectable() == theObject && anOwner->IsSelected())
      aResultOwners.Append(anOwner);
  }
  NCollection_List<Handle(SelectBasics_EntityOwner)>::Iterator anOwnersIt (aResultOwners);
  Handle(SelectMgr_EntityOwner) anOwner;
  for (; anOwnersIt.More(); anOwnersIt.Next()) {
    anOwner = Handle(SelectMgr_EntityOwner)::DownCast(anOwnersIt.Value());
    if (!anOwner.IsNull())
      aContext->AddOrRemoveSelected(anOwner, false);
  }
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

std::list<FeaturePtr> XGUI_SelectionMgr::getSelectedFeatures()
{
  std::list<FeaturePtr> aFeatures;
  QObjectPtrList aObjects = selection()->selectedObjects();
  if (aObjects.isEmpty())
    return aFeatures;

  bool isPart = false;
  foreach(ObjectPtr aObj, aObjects) {
    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObj);
    if (aFeature.get()) {
      ResultPtr aRes = aFeature->firstResult();
      isPart = (aRes.get() && (aRes->groupName() == ModelAPI_ResultPart::group()));
      if (!isPart)
        aFeatures.push_back(aFeature);
    }
  }
  return aFeatures;
}