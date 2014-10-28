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

#include <AIS_Shape.hxx>


XGUI_ModuleConnector::XGUI_ModuleConnector(XGUI_Workshop* theWorkshop)
    : ModuleBase_IWorkshop(theWorkshop),
      myWorkshop(theWorkshop)
{
  XGUI_SelectionMgr* aSelector = myWorkshop->selector();
  connect(aSelector, SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()));
  
  XGUI_OperationMgr* anOperationMgr = myWorkshop->operationMgr();

  connect(anOperationMgr, SIGNAL(operationStarted(ModuleBase_Operation*)), 
    this, SIGNAL(operationStarted(ModuleBase_Operation*)));
  connect(anOperationMgr, SIGNAL(operationStopped(ModuleBase_Operation*)), 
    this, SIGNAL(operationStopped(ModuleBase_Operation*)));

  myDocumentShapeFilter = new ModuleBase_ShapeDocumentFilter(this);
}

XGUI_ModuleConnector::~XGUI_ModuleConnector()
{
  myDocumentShapeFilter.Nullify();
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

ModuleBase_Operation* XGUI_ModuleConnector::currentOperation() const
{
  return myWorkshop->operationMgr()->currentOperation();
}


void XGUI_ModuleConnector::activateSubShapesSelection(const QIntList& theTypes)
{
  XGUI_Displayer* aDisp = myWorkshop->displayer();
  aDisp->openLocalContext();
  // Convert shape types to selection types
  QIntList aModes;
  foreach(int aType, theTypes) {
    aModes.append(AIS_Shape::SelectionMode((TopAbs_ShapeEnum)aType));
  }
  aDisp->activateObjectsOutOfContext(aModes);
  //TODO: We have to open Local context because at neutral point filters don't work (bug 25340)
  aDisp->addSelectionFilter(myDocumentShapeFilter);
}

void XGUI_ModuleConnector::deactivateSubShapesSelection()
{
  XGUI_Displayer* aDisp = myWorkshop->displayer();
  // The document limitation selection has to be only during operation
  aDisp->removeSelectionFilter(myDocumentShapeFilter);
  aDisp->closeLocalContexts(false);
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

void XGUI_ModuleConnector::setSelected(const QList<ObjectPtr>& theFeatures)
{
  XGUI_Displayer* aDisp = myWorkshop->displayer();
  aDisp->setSelected(theFeatures);
}