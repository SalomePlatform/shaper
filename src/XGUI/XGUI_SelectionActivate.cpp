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

#include "XGUI_SelectionActivate.h"

#include "ModelAPI_Object.h"

#include "ModuleBase_IModule.h"
#include "ModuleBase_IViewer.h"
#include "ModuleBase_ModelWidget.h"
#include "ModuleBase_Preferences.h"

#include "XGUI_ActiveControlMgr.h"
#include "XGUI_ActiveControlSelector.h"
#include "XGUI_Displayer.h"
#include "XGUI_FacesPanel.h"
#include "XGUI_FacesPanelSelector.h"
#include "XGUI_SelectionMgr.h"
#include "XGUI_Tools.h"
#include "XGUI_Workshop.h"

#include <SUIT_ResourceMgr.h>

#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <AIS_Trihedron.hxx>

#include <SelectMgr_SelectionManager.hxx>

//#define DEBUG_ACTIVATE_OBJECTS
//#define DEBUG_DEACTIVATE
//#define DEBUG_ACTIVATE_AIS
//#define DEBUG_DEACTIVATE_AIS

#define CLEAR_OUTDATED_SELECTION_BEFORE_REDISPLAY

//**************************************************************
XGUI_SelectionActivate::XGUI_SelectionActivate(ModuleBase_IWorkshop* theWorkshop)
 : ModuleBase_ISelectionActivate(theWorkshop), myIsTrihedronActive(true)
{
}

//**************************************************************
XGUI_SelectionActivate::SelectionPlace XGUI_SelectionActivate::activeSelectionPlace() const
{
  XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(myWorkshop);
  XGUI_ActiveControlSelector* anActiveSelector = aWorkshop->activeControlMgr()->activeSelector();
  if (!anActiveSelector)
    return Workshop;

  if (anActiveSelector->getType() == XGUI_FacesPanelSelector::Type())
    return FacesPanel;
  else
    return PropertyPanel;
}

//**************************************************************
void XGUI_SelectionActivate::updateSelectionModes()
{
  QIntList aModes;
  switch (activeSelectionPlace()) {
  case Workshop:
    myWorkshop->module()->activeSelectionModes(aModes);
    break;
    case PropertyPanel: {
      ModuleBase_ModelWidget* anActiveWidget = myWorkshop->module()->activeWidget();
      if (anActiveWidget)
        getSelectionModes(anActiveWidget, aModes);
      else
        myWorkshop->module()->activeSelectionModes(aModes); //using module modes
    }
    break;
    case FacesPanel: {
      XGUI_Tools::workshop(myWorkshop)->facesPanel()->selectionModes(aModes);
      myWorkshop->module()->customSubShapesSelectionModes(aModes); // avoid wire selection
    }
    break;
    default: break;
  }
  activateObjects(aModes, getDisplayer()->displayedObjects(), true);
}

//**************************************************************
void XGUI_SelectionActivate::updateSelectionFilters()
{
  SelectMgr_ListOfFilter aSelectionFilters;
  switch (activeSelectionPlace()) {
  case Workshop:
    XGUI_Tools::workshop(myWorkshop)->selectionFilters(aSelectionFilters);
    break;
    case PropertyPanel: {
      ModuleBase_ModelWidget* anActiveWidget = myWorkshop->module()->activeWidget();
      getSelectionFilters(anActiveWidget, aSelectionFilters);
    }
    break;
    case FacesPanel: {
      //XGUI_Tools::workshop(myWorkshop)->selectionFilters(aSelectionFilters);
      XGUI_Tools::workshop(myWorkshop)->facesPanel()->selectionFilters(aSelectionFilters);
    }
    break;
    default: break;
  }
  activateSelectionFilters(aSelectionFilters);
}

//**************************************************************
void XGUI_SelectionActivate::activateSelectionFilters
  (const SelectMgr_ListOfFilter& theSelectionFilters)
{
  XGUI_Displayer* aDisplayer = getDisplayer();
  aDisplayer->deactivateSelectionFilters(false);

  SelectMgr_ListIteratorOfListOfFilter aIt(theSelectionFilters);
  for (; aIt.More(); aIt.Next()) {
    Handle(SelectMgr_Filter) aFilter = aIt.Value();
    if (aFilter.IsNull())
      continue;
    aDisplayer->addSelectionFilter(aFilter);
  }
}

//**************************************************************
void XGUI_SelectionActivate::activateSelectionAndFilters(ModuleBase_ModelWidget* theWidget)
{
  // activate selection modes
  QIntList aModes;
  getSelectionModes(theWidget, aModes);
  activateObjects(aModes, getDisplayer()->displayedObjects(), true);

  // activate selection filters
  SelectMgr_ListOfFilter aSelectionFilters;
  getSelectionFilters(theWidget, aSelectionFilters);
  activateSelectionFilters(aSelectionFilters);
}

//**************************************************************
void XGUI_SelectionActivate::getSelectionModes(ModuleBase_ModelWidget* theWidget,
                                               QIntList& theModes)
{
  if (!theWidget)
    return;

  bool isAdditional = false;
  theWidget->selectionModes(theModes, isAdditional);
  if (isAdditional) {
    myWorkshop->module()->customSubShapesSelectionModes(theModes);
    //theModes.append(XGUI_Tools::workshop(myWorkshop)->viewerSelectionModes());
    //myWorkshop->module()->activeSelectionModes(theModes);
  }
}

//**************************************************************
void XGUI_SelectionActivate::getSelectionFilters(ModuleBase_ModelWidget* theWidget,
                                                 SelectMgr_ListOfFilter& theSelectionFilters)
{
  XGUI_Tools::workshop(myWorkshop)->selectionFilters(theSelectionFilters);

  if (theWidget)
    theWidget->selectionFilters(theSelectionFilters);
}

//**************************************************************
QIntList XGUI_SelectionActivate::activeSelectionModes() const
{
  QIntList aModes;
  foreach (int aMode, myActiveSelectionModes) {
    // aMode < 9 is a Shape Enum values
    aModes << ((aMode < 9)? AIS_Shape::SelectionType(aMode) : aMode);
  }
  return aModes;
}

//**************************************************************
bool XGUI_SelectionActivate::isActive(ObjectPtr theObject) const
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull() || !getDisplayer()->isVisible(theObject))
    return false;

  AISObjectPtr anObj = getDisplayedAISObject(theObject);
  Handle(AIS_InteractiveObject) anAIS = anObj->impl<Handle(AIS_InteractiveObject)>();

  TColStd_ListOfInteger aModes;
  aContext->ActivatedModes(anAIS, aModes);

  return aModes.Extent() > 0;
}

//**************************************************************
void XGUI_SelectionActivate::activateObjects(const QIntList& theModes,
  const QObjectPtrList& theObjList, const bool theUpdateViewer)
{
  setSelectionModes(theModes);

  Handle(AIS_InteractiveContext) aContext = AISContext();
  // Open local context if there is no one
  if (aContext.IsNull())
    return;

  //aContext->UseDisplayedObjects();
  //myUseExternalObjects = true;

  Handle(AIS_InteractiveObject) anAISIO;
  AIS_ListOfInteractive aPrsList;
  AIS_ListOfInteractive aPrsListToBeDeactivated;
  //if (aObjList.isEmpty())
  //  return;
  //else {
  foreach(ObjectPtr anObject, theObjList) {
    AISObjectPtr anAISObject = getDisplayedAISObject(anObject);
    if (!anAISObject.get())
      continue;

    Handle(AIS_InteractiveObject) aPrs = anAISObject->impl<Handle(AIS_InteractiveObject)>();
    if (myWorkshop->module()->canActivateSelection(anObject))
      aPrsList.Append(aPrs);
    else
      aPrsListToBeDeactivated.Append(aPrs);
  }
  //}

  // Add trihedron because it has to partisipate in selection
  Handle(AIS_InteractiveObject) aTrihedron;
  if (isTrihedronActive()) {
    aTrihedron = getTrihedron();
    if (!aTrihedron.IsNull() && aContext->IsDisplayed(aTrihedron))
      aPrsList.Append(aTrihedron);
  }
  if (aPrsList.Extent() == 0 && aPrsListToBeDeactivated.Extent() == 0)
    return;

  AIS_ListIteratorOfListOfInteractive aLIt;
  bool isActivationChanged = false;
  for(aLIt.Initialize(aPrsList); aLIt.More(); aLIt.Next()) {
    anAISIO = aLIt.Value();
    if (activate(anAISIO, false))
      isActivationChanged = true;
  }

  for(aLIt.Initialize(aPrsListToBeDeactivated); aLIt.More(); aLIt.Next()) {
    anAISIO = aLIt.Value();
    deactivateAIS(anAISIO);
    isActivationChanged = true;
  }
}

#ifdef DEBUG_ACTIVATE_OBJECTS
//**************************************************************
QString getModeInfo(const int theMode)
{
  QString anInfo = "Undefined";
  switch(theMode) {
    case 0: anInfo = "SHAPE(0)"; break;
    case 1: anInfo = "VERTEX(1)"; break;
    case 2: anInfo = "EDGE(2)"; break;
    case 3: anInfo = "WIRE(3)"; break;
    case 4: anInfo = "FACE(4)"; break;
    case 5: anInfo = "SHELL(5)"; break;
    case 6: anInfo = "SOLID(6)"; break;
    case 7: anInfo = "COMPSOLID(7)"; break;
    case 8: anInfo = "COMPOUND(8)"; break;
    case 100: anInfo = "Sel_Mode_First(100)"; break; //SketcherPrs_Tools
    case 101: anInfo = "Sel_Constraint(101)"; break;
    case 102: anInfo = "Sel_Dimension_All(102)"; break;
    case 103: anInfo = "Sel_Dimension_Line(103)"; break;
    case 104: anInfo = "Sel_Dimension_Text(104)"; break;
    default: break;
  }
  return anInfo;
}

//**************************************************************
QString getModesInfo(const QIntList& theModes)
{
  QStringList aModesInfo;
  for (int i = 0, aSize = theModes.size(); i < aSize; i++)
    aModesInfo.append(getModeInfo(theModes[i]));
  return QString("[%1] = %2").arg(aModesInfo.size()).arg(aModesInfo.join(", "));
}
#endif

//**************************************************************
void XGUI_SelectionActivate::setSelectionModes(const QIntList& theModes)
{
  // Convert shape types to selection types
  QIntList aModes;
  foreach(int aType, theModes) {
    aModes.append(getSelectionMode(aType));
  }

#ifdef DEBUG_ACTIVATE_OBJECTS
  QStringList anInfo;
  QObjectPtrList::const_iterator anIt = theObjList.begin(), aLast = theObjList.end();
  for (; anIt != aLast; ++anIt) {
    anInfo.append(ModuleBase_Tools::objectInfo((*anIt)));
  }
  QString anInfoStr = anInfo.join(", ");

  qDebug(QString("activateObjects: new modes%1, active modes%2, objects[%3] = %4").
    arg(getModesInfo(aModes)).
    arg(getModesInfo(myActiveSelectionModes)).
    arg(theObjList.size()).
    arg(anInfoStr).
    toStdString().c_str());
#endif
  // In order to avoid doblications of selection modes
  QIntList aNewModes;
  foreach (int aMode, aModes) {
    if (!aNewModes.contains(aMode))
      aNewModes.append(aMode);
  }
  myActiveSelectionModes = aNewModes;
}

//**************************************************************
void XGUI_SelectionActivate::activateOnDisplay(const Handle(AIS_InteractiveObject)& theIO,
                                               const bool theUpdateViewer)
{
  if (myActiveSelectionModes.size() == 0)
    activateAIS(theIO, 0, theUpdateViewer);
  else {
    foreach(int aMode, myActiveSelectionModes) {
      activateAIS(theIO, aMode, theUpdateViewer);
    }
  }
}

//**************************************************************
void XGUI_SelectionActivate::activateAIS(const Handle(AIS_InteractiveObject)& theIO,
                                         const int theMode, const bool theUpdateViewer) const
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!theIO.IsNull() && theIO == getTrihedron()) {
    if (theMode != AIS_Shape::SelectionType(TopAbs_EDGE) &&
        theMode != AIS_Shape::SelectionType(TopAbs_VERTEX))
      return;
  }
  if (!aContext.IsNull()) {
    if (myWorkshop->module()) {
      int aMode = (theMode > 8)? theMode : AIS_Shape::SelectionType(theMode);
      aContext->Activate(theIO, theMode, false);
    } else
      aContext->Activate(theIO, theMode, false);

    // the fix from VPA for more suitable selection of sketcher lines
    if (theIO->Width() > 1) {
      double aPrecision = theIO->Width() + 2;
      if (theMode == getSelectionMode(TopAbs_VERTEX))
        aPrecision = ModuleBase_Preferences::resourceMgr()->doubleValue("Viewer",
        "point-selection-sensitivity", 12);
      else if ((theMode == getSelectionMode(TopAbs_EDGE)) ||
               (theMode == getSelectionMode(TopAbs_WIRE)))
        aPrecision = theIO->Width() + ModuleBase_Preferences::resourceMgr()->doubleValue("Viewer",
           "edge-selection-sensitivity", 2);
      aContext->SetSelectionSensitivity(theIO, theMode, aPrecision);
    }

#ifdef DEBUG_ACTIVATE_AIS
    ObjectPtr anObject = getObject(theIO);
    anInfo.append(ModuleBase_Tools::objectInfo((*anIt)));
    qDebug(QString("activateAIS: theMode = %1, object = %2").arg(theMode)
      .arg(anInfo).toStdString().c_str());
#endif
    if (theUpdateViewer)
      getDisplayer()->updateViewer();
  }
}

//**************************************************************
void XGUI_SelectionActivate::deactivateAIS(const Handle(AIS_InteractiveObject)& theIO,
                                           const int theMode) const
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull()) {
    if (theMode == -1)
      aContext->Deactivate(theIO);
    else
      aContext->Deactivate(theIO, theMode);

#ifdef DEBUG_DEACTIVATE_AIS
    ObjectPtr anObject = getObject(theIO);
    anInfo.append(ModuleBase_Tools::objectInfo((*anIt)));
    qDebug(QString("deactivateAIS: theMode = %1, object = %2").arg(theMode)
      .arg(anInfo).toStdString().c_str());
#endif
  }
}

//**************************************************************
bool XGUI_SelectionActivate::activate(const Handle(AIS_InteractiveObject)& theIO,
                                      const bool theUpdateViewer) const
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull() || theIO.IsNull())
    return false;

  bool isActivationChanged = false;
  // deactivate object in all modes, which are not in the list of activation
  // It seems that after the IO deactivation the selected state of the IO's owners
  // is modified in OCC(version: 6.8.0) and the selection of the object later is lost.
  // By this reason, the number of the IO deactivate is decreased and the object is deactivated
  // only if there is a difference in the current modes and the parameters modes.
  // If the selection problem happens again, it is possible to write a test scenario and create
  // a bug. The bug steps are the following:
  // Create two IO, activate them in 5 modes, select the first IO, deactivate 3 modes for both,
  // with clicked SHIFT select the second object.
  // The result is the selection of the first IO is lost.
  TColStd_ListOfInteger aTColModes;
  aContext->ActivatedModes(theIO, aTColModes);
  TColStd_ListIteratorOfListOfInteger itr( aTColModes );
  QIntList aModesActivatedForIO;
  bool isDeactivated = false;
  bool aHasValidMode = false;
  for (; itr.More(); itr.Next() ) {
    Standard_Integer aMode = itr.Value();
    aHasValidMode = aHasValidMode || aMode != -1;
    int aShapeMode = (aMode > 8)? aMode : AIS_Shape::SelectionType(aMode);
    if (!myActiveSelectionModes.contains(aMode)) {
      deactivateAIS(theIO, aMode);
      isDeactivated = true;
    }
    else {
      aModesActivatedForIO.append(aMode);
    }
  }
  if (isDeactivated) {
    // the selection from the previous activation modes should be cleared manually (#26172)
    //theIO->ClearSelected();
#ifndef CLEAR_OUTDATED_SELECTION_BEFORE_REDISPLAY
    XGUI_Tools::workshop(myWorkshop)->selector()->deselectPresentation(theIO);
#endif
    // For performance issues
    //if (theUpdateViewer)
    //  getDisplayer()->updateViewer();
    isActivationChanged = true;
  }

  // loading the interactive object allowing the decomposition
  if (aTColModes.IsEmpty() || !aHasValidMode) {
    aContext->Load(theIO, -1, true);
    Handle(AIS_Trihedron) aTrihedron = Handle(AIS_Trihedron)::DownCast(theIO);
    if (!aTrihedron.IsNull()) {
      // Workaround for Trihedron. It should be loaded using the next Load method to
      // add this object to myGlobal map of selection manager
      // it is important to activate trihedron in two selection modes: edges and vertices
      aContext->SelectionManager()->Load(theIO);
    }
  }

  // trihedron AIS check should be after the AIS loading.
  // If it is not loaded, it is steel selectable in the viewer.
  Handle(AIS_Trihedron) aTrihedron;
  if (!isTrihedronActive())
    aTrihedron = Handle(AIS_Trihedron)::DownCast(theIO);
  if (aTrihedron.IsNull()) {
      // In order to clear active modes list
    if (myActiveSelectionModes.size() == 0) {
      activateAIS(theIO, 0, theUpdateViewer);
    } else {
      foreach(int aMode, myActiveSelectionModes) {
        if (!aModesActivatedForIO.contains(aMode)) {
          activateAIS(theIO, aMode, theUpdateViewer);
          isActivationChanged = true;
        }
      }
    }
  }
  return isActivationChanged;
}

//**************************************************************
void XGUI_SelectionActivate::deactivate(const ObjectPtr& theObject, const bool theUpdateViewer)
{
#ifdef DEBUG_DEACTIVATE
  QString anInfoStr = ModuleBase_Tools::objectInfo(theObject);
  qDebug(QString("deactivate: myActiveSelectionModes[%1]: %2, objects = ").
    arg(myActiveSelectionModes.size()).arg(qIntListInfo(myActiveSelectionModes)).
    arg(anInfoStr).
    toStdString().c_str());
#endif
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull() && getDisplayer()->isVisible(theObject)) {
    AISObjectPtr anObj = getDisplayedAISObject(theObject);
    Handle(AIS_InteractiveObject) anAIS = anObj->impl<Handle(AIS_InteractiveObject)>();

    deactivateAIS(anAIS);
    // the selection from the previous activation modes should be cleared manually (#26172)
#ifndef CLEAR_OUTDATED_SELECTION_BEFORE_REDISPLAY
    XGUI_Tools::workshop(myWorkshop)->selector()->deselectPresentation(anAIS);
#endif
    if (theUpdateViewer)
      getDisplayer()->updateViewer();
  }
}

/// #1136 hidden axis are selected in sketch
#ifdef BEFORE_TRIHEDRON_PATCH
//**************************************************************
void deactivateObject(Handle(AIS_InteractiveContext) theContext,
                      Handle(AIS_InteractiveObject) theObject)
{
  if (!theObject.IsNull())
    theContext->Deactivate(theObject);
}
#endif

//**************************************************************
void XGUI_SelectionActivate::activateTrihedron(bool theIsActive)
{
  myIsTrihedronActive = theIsActive;
  if (!myIsTrihedronActive)
    deactivateTrihedron(true);
}

//**************************************************************
void XGUI_SelectionActivate::deactivateTrihedron(const bool theUpdateViewer) const
{
  Handle(AIS_InteractiveObject) aTrihedron = getTrihedron();
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aTrihedron.IsNull() && aContext->IsDisplayed(aTrihedron)) {
    Handle(AIS_Trihedron) aTrie = Handle(AIS_Trihedron)::DownCast(aTrihedron);
    if (!aTrie.IsNull())
      aContext->Deactivate(aTrie);

    /// #1136 hidden axis are selected in sketch
#ifdef BEFORE_TRIHEDRON_PATCH
    deactivateObject(aContext, aTrie->XAxis());
    deactivateObject(aContext, aTrie->YAxis());
    deactivateObject(aContext, aTrie->Axis());
    deactivateObject(aContext, aTrie->Position());

    deactivateObject(aContext, aTrie->XYPlane());
    deactivateObject(aContext, aTrie->XZPlane());
    deactivateObject(aContext, aTrie->YZPlane());
#endif
    if (theUpdateViewer)
      getDisplayer()->updateViewer();
  }
}

//**************************************************************
void XGUI_SelectionActivate::deactivateTrihedronInSelectionModes()
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  Handle(AIS_Trihedron) aTrihedron = Handle(AIS_Trihedron)::DownCast(getTrihedron());
  /// deactivate trihedron in selection modes
  TColStd_ListOfInteger aTColModes;
  aContext->ActivatedModes(aTrihedron, aTColModes);
  TColStd_ListIteratorOfListOfInteger itr( aTColModes );
  for (; itr.More(); itr.Next() ) {
    Standard_Integer aMode = itr.Value();
    aContext->Deactivate(aTrihedron, aMode);
  }
}

//**************************************************************
Handle(AIS_InteractiveContext) XGUI_SelectionActivate::AISContext() const
{
  return myWorkshop->viewer()->AISContext();
}

//**************************************************************
XGUI_Displayer* XGUI_SelectionActivate::getDisplayer() const
{
  return XGUI_Tools::workshop(myWorkshop)->displayer();
}

//**************************************************************
Handle(AIS_InteractiveObject) XGUI_SelectionActivate::getTrihedron() const
{
  return myWorkshop->viewer()->trihedron();
}

//**************************************************************
AISObjectPtr XGUI_SelectionActivate::getDisplayedAISObject(ObjectPtr theObject) const
{
  return getDisplayer()->getAISObject(theObject);
}

//**************************************************************
int XGUI_SelectionActivate::getSelectionMode(int theShapeType)
{
  return (theShapeType > TopAbs_SHAPE) ? theShapeType :
                                         AIS_Shape::SelectionMode((TopAbs_ShapeEnum)theShapeType);
}
