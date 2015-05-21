// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        XGUI_ModuleConnector.cpp
// Created:     3 June 2014
// Author:      Vitaly Smetannikov

#include "XGUI_ModuleConnector.h"
#include "XGUI_Workshop.h"
#include "XGUI_ViewerProxy.h"
#include "XGUI_SelectionMgr.h"
#include "XGUI_Selection.h"
#include "XGUI_OperationMgr.h"
#include "XGUI_Displayer.h"
#include "XGUI_PropertyPanel.h"

#include <ModuleBase_IModule.h>

#include <AIS_Shape.hxx>


XGUI_ModuleConnector::XGUI_ModuleConnector(XGUI_Workshop* theWorkshop)
    : ModuleBase_IWorkshop(theWorkshop),
      myWorkshop(theWorkshop)
{
  XGUI_SelectionMgr* aSelector = myWorkshop->selector();
  connect(aSelector, SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()));
  
  XGUI_OperationMgr* anOperationMgr = myWorkshop->operationMgr();

  //myDocumentShapeFilter = new ModuleBase_ShapeDocumentFilter(this);
}

XGUI_ModuleConnector::~XGUI_ModuleConnector()
{
  //myDocumentShapeFilter.Nullify();
}

ModuleBase_ISelection* XGUI_ModuleConnector::selection() const
{
  return myWorkshop->selector()->selection();
}

ModuleBase_IModule* XGUI_ModuleConnector::module() const
{
  return myWorkshop->module();
}

ModuleBase_IViewer* XGUI_ModuleConnector::viewer() const
{
  return myWorkshop->viewer();
}

ModuleBase_IPropertyPanel* XGUI_ModuleConnector::propertyPanel() const
{
  return myWorkshop->propertyPanel();
}

ModuleBase_Operation* XGUI_ModuleConnector::currentOperation() const
{
  return myWorkshop->operationMgr()->currentOperation();
}


void XGUI_ModuleConnector::activateSubShapesSelection(const QIntList& theTypes)
{
  XGUI_Displayer* aDisp = myWorkshop->displayer();
  // Close context if it was opened in order to clear stsndard selection modes
  //aDisp->closeLocalContexts(false);
  //aDisp->openLocalContext();
  // Convert shape types to selection types
  QIntList aModes;
  foreach(int aType, theTypes) {
    if (aType > TopAbs_SHAPE) 
      aModes.append(aType);
    else
      aModes.append(AIS_Shape::SelectionMode((TopAbs_ShapeEnum)aType));
  }
  aDisp->activateObjects(aModes);
  //TODO: We have to open Local context because at neutral point filters don't work (bug 25340)
  //aDisp->addSelectionFilter(myDocumentShapeFilter);
}

void XGUI_ModuleConnector::deactivateSubShapesSelection()
{
  XGUI_Displayer* aDisp = myWorkshop->displayer();
  // Clear selection modes
  QIntList aModes;
  // TODO: check on OCC6.9.0
  // the module current active modes should not be deactivated in order to save the objects selected
  // the deactivate object in the mode of selection leads to the object is deselected in the viewer.
  // But, in OCC6.8.0 this deselection does not happened automatically. It is necessary to call
  // ClearOutdatedSelection, but this method has an error in the realization, which should be fixed in
  // the OCC6.9.0 release. Moreother, it is possible that ClearOutdatedSelection will be called inside
  // Deactivate method of AIS_InteractiveContext. In this case, we need not call it.
  module()->activeSelectionModes(aModes);
  aDisp->activateObjects(aModes);
  // The document limitation selection has to be only during operation
  //aDisp->removeSelectionFilter(myDocumentShapeFilter);
  //aDisp->closeLocalContexts(false);
}

AISObjectPtr XGUI_ModuleConnector::findPresentation(const ObjectPtr& theObject) const
{
  XGUI_Displayer* aDisp = myWorkshop->displayer();
  return aDisp->getAISObject(theObject);
}

ObjectPtr XGUI_ModuleConnector::findPresentedObject(const AISObjectPtr& theAIS) const
{
  XGUI_Displayer* aDisp = myWorkshop->displayer();
  return aDisp->getObject(theAIS);
}

void XGUI_ModuleConnector::setSelected(const QList<ModuleBase_ViewerPrs>& theValues)
{
  XGUI_Displayer* aDisp = myWorkshop->displayer();
  if (theValues.isEmpty()) {
    myWorkshop->selector()->clearSelection();
  } else {
    aDisp->setSelected(theValues);
  }    
}

bool XGUI_ModuleConnector::canStartOperation(QString theId)
{
  return myWorkshop->operationMgr()->canStartOperation(theId);
}

