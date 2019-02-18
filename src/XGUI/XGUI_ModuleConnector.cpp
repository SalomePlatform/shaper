// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "XGUI_ModuleConnector.h"
#include "XGUI_Workshop.h"
#include "XGUI_ViewerProxy.h"
#include "XGUI_Selection.h"
#include "XGUI_SelectionActivate.h"
#include "XGUI_SelectionMgr.h"
#include "XGUI_OperationMgr.h"
#include "XGUI_Displayer.h"
#include "XGUI_PropertyPanel.h"
#include "XGUI_ActionsMgr.h"
#include "XGUI_ErrorMgr.h"
#include "XGUI_ObjectsBrowser.h"

#include <ModuleBase_IModule.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_OperationDescription.h>

#include <AIS_Shape.hxx>

#ifndef HAVE_SALOME
#include "AppElements_Command.h"
#else
#include "XGUI_SalomeConnector.h"
#endif

XGUI_ModuleConnector::XGUI_ModuleConnector(XGUI_Workshop* theWorkshop)
    : ModuleBase_IWorkshop(theWorkshop),
      myWorkshop(theWorkshop)
{
  XGUI_SelectionMgr* aSelector = myWorkshop->selector();
  connect(aSelector, SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()));
}

XGUI_ModuleConnector::~XGUI_ModuleConnector()
{
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

ModuleBase_IErrorMgr* XGUI_ModuleConnector::errorMgr() const
{
  return myWorkshop->errorMgr();
}

ModuleBase_ISelectionActivate* XGUI_ModuleConnector::selectionActivate() const
{
  return myWorkshop->selectionActivate();
}

ModuleBase_Operation* XGUI_ModuleConnector::currentOperation() const
{
  return myWorkshop->operationMgr()->currentOperation();
}


QObjectPtrList XGUI_ModuleConnector::activeObjects(const QObjectPtrList& theObjList) const
{
  QObjectPtrList aActiveOPbjects;
  ModuleBase_IModule* aModule = myWorkshop->module();
  // Activate objects only which can be activated
  foreach (ObjectPtr aObj, theObjList) {
    if (aModule->canActivateSelection(aObj))
      aActiveOPbjects.append(aObj);
  }
  return aActiveOPbjects;
}

AISObjectPtr XGUI_ModuleConnector::findPresentation(const ObjectPtr& theObject) const
{
  XGUI_Displayer* aDisp = myWorkshop->displayer();
  return aDisp->getAISObject(theObject);
}

bool XGUI_ModuleConnector::isVisible(const ObjectPtr& theObject) const
{
  XGUI_Displayer* aDisp = myWorkshop->displayer();
  return aDisp->isVisible(theObject);
}


ObjectPtr XGUI_ModuleConnector::findPresentedObject(const AISObjectPtr& theAIS) const
{
  XGUI_Displayer* aDisp = myWorkshop->displayer();
  ObjectPtr anObject = aDisp->getObject(theAIS);
  return anObject;
}

void XGUI_ModuleConnector::setSelected(const QList<ModuleBase_ViewerPrsPtr>& theValues)
{
  XGUI_Displayer* aDisp = myWorkshop->displayer();
  XGUI_ObjectsBrowser* aBrowser = myWorkshop->objectBrowser();
  if (theValues.isEmpty()) {
    myWorkshop->selector()->clearSelection();
    aBrowser->treeView()->clearSelection();
  } else {
    aDisp->setSelected(theValues);
    // Synchronise the selection with Object browser
    QObjectPtrList anObjects;
    foreach(ModuleBase_ViewerPrsPtr aVal, theValues) {
      anObjects.append(aVal->object());
    }
    aBrowser->setObjectsSelected(anObjects);
  }
}

void XGUI_ModuleConnector::setStatusBarMessage(const QString& theMessage)
{
  myWorkshop->setStatusBarMessage(theMessage);
}

bool XGUI_ModuleConnector::canStartOperation(QString theId, bool& isCommitted)
{
  return myWorkshop->operationMgr()->canStartOperation(theId, isCommitted);
}

void XGUI_ModuleConnector::processLaunchOperation(ModuleBase_Operation* theOperation)
{
  XGUI_OperationMgr* anOperationMgr = workshop()->operationMgr();

  if (anOperationMgr->startOperation(theOperation)) {
    ModuleBase_OperationFeature* aFOperation =
      dynamic_cast<ModuleBase_OperationFeature*>(theOperation);
    if (aFOperation) {
      workshop()->propertyPanel()->updateContentWidget(aFOperation->feature());
      workshop()->propertyPanel()->createContentPanel(aFOperation->feature());
    }
    if (!theOperation->getDescription()->hasXmlRepresentation()) {
      if (theOperation->commit())
        workshop()->updateCommandStatus();
    }
  }
}

ModuleBase_Operation* XGUI_ModuleConnector::findStartedOperation(const QString& theId)
{
  return myWorkshop->operationMgr()->findOperation(theId);
}

bool XGUI_ModuleConnector::canStopOperation(ModuleBase_Operation* theOperation)
{
  return myWorkshop->operationMgr()->canStopOperation(theOperation);
}

void XGUI_ModuleConnector::stopOperation(ModuleBase_Operation* theOperation,
                                         bool& isCommitted)
{
  myWorkshop->operationMgr()->stopOperation(theOperation, isCommitted);
}

void XGUI_ModuleConnector::updateCommandStatus()
{
  myWorkshop->updateCommandStatus();
}

QMainWindow* XGUI_ModuleConnector::desktop() const
{
  return myWorkshop->desktop();
}

bool XGUI_ModuleConnector::hasSHIFTPressed() const
{
  return myWorkshop->operationMgr()->hasSHIFTPressed();
}

std::shared_ptr<Config_FeatureMessage> XGUI_ModuleConnector::featureInfo(const QString& theId) const
{
#ifdef HAVE_SALOME
  return myWorkshop->salomeConnector()->featureInfo(theId);
#else
  AppElements_Command* aAction =
    dynamic_cast<AppElements_Command*>(myWorkshop->actionsMgr()->action(theId));
  if (aAction)
    return aAction->featureMessage();
  return std::shared_ptr<Config_FeatureMessage>();
#endif
}
