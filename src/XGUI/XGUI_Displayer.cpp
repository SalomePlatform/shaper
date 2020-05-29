// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "XGUI_Displayer.h"

#include "XGUI_FacesPanel.h"
#include "XGUI_Selection.h"
#include "XGUI_SelectionActivate.h"
#include "XGUI_SelectionMgr.h"
#include "XGUI_ViewerProxy.h"
#include "XGUI_Workshop.h"

#ifndef HAVE_SALOME
#include <AppElements_Viewer.h>
#endif

#include <ModelAPI_Document.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>

#include <ModuleBase_BRepOwner.h>
#include <ModuleBase_IModule.h>
#include <ModuleBase_Preferences.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_IViewer.h>

#include <GeomAPI_Shape.h>
#include <GeomAPI_IPresentable.h>
#include <GeomAPI_ICustomPrs.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_IScreenParams.h>

#include <SUIT_ResourceMgr.h>

#include <AIS_InteractiveContext.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_DimensionSelectionMode.hxx>
#include <AIS_Shape.hxx>
#include <AIS_Dimension.hxx>
#include <AIS_Trihedron.hxx>
#ifdef BEFORE_TRIHEDRON_PATCH
#include <AIS_Axis.hxx>
#include <AIS_Plane.hxx>
#include <AIS_Point.hxx>
#endif
#include <AIS_Selection.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <SelectMgr_ListOfFilter.hxx>
#include <SelectMgr_ListIteratorOfListOfFilter.hxx>
#include <SelectMgr_SelectionManager.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <BRepMesh_IncrementalMesh.hxx>

#include <StdSelect_ViewerSelector3d.hxx>

#include <TColStd_MapOfTransient.hxx>
#include <TColStd_MapIteratorOfMapOfTransient.hxx>
#include <Standard_Version.hxx>

#ifdef TINSPECTOR
#include <inspector/VInspectorAPI_CallBack.hxx>
#endif

#include <Events_Loop.h>
#include <ModelAPI_Events.h>
#include <Config_PropManager.h>

#include <set>

#ifdef _MSC_VER
#pragma warning(disable: 4702)
#endif

/// defines the local context mouse selection sensitivity
const int MOUSE_SENSITIVITY_IN_PIXEL = 10;

//#define DEBUG_DISPLAY
//#define DEBUG_FEATURE_REDISPLAY
//#define DEBUG_SELECTION_FILTERS

//#define DEBUG_OCCT_SHAPE_SELECTION

#define CLEAR_OUTDATED_SELECTION_BEFORE_REDISPLAY

//#define DEBUG_VIEWER_BLOCKED_COUNT

//**************************************************************
void displayedObjects(const Handle(AIS_InteractiveContext)& theAIS, AIS_ListOfInteractive& theList)
{
  // Get from null point
#if OCC_VERSION_HEX < 0x070400
	theAIS->DisplayedObjects(theList, true);
#else
	theAIS->DisplayedObjects(theList);
#endif
}

QString qIntListInfo(const QIntList& theValues, const QString& theSeparator = QString(", "))
{
  QStringList anInfo;
  QIntList::const_iterator anIt = theValues.begin(), aLast = theValues.end();
  for (; anIt != aLast; anIt++) {
    anInfo.append(QString::number(*anIt));
  }
  return anInfo.join(theSeparator);
}

//**************************************************************
XGUI_Displayer::XGUI_Displayer(XGUI_Workshop* theWorkshop)
: myWorkshop(theWorkshop),
  myViewerBlockedRecursiveCount(0),
  myContextId(0),
  myNeedUpdate(false)
{
  BRepMesh_IncrementalMesh::SetParallelDefault(Standard_True);
}

//**************************************************************
XGUI_Displayer::~XGUI_Displayer()
{
}

//**************************************************************
bool XGUI_Displayer::isVisible(ObjectPtr theObject) const
{
  return myResult2AISObjectMap.contains(theObject);
}

//**************************************************************
bool XGUI_Displayer::display(ObjectPtr theObject, bool theUpdateViewer)
{
  bool aDisplayed = false;
  if (isVisible(theObject)) {
    aDisplayed = redisplay(theObject, theUpdateViewer);
  } else {
    AISObjectPtr anAIS;
    GeomPresentablePtr aPrs = std::dynamic_pointer_cast<GeomAPI_IPresentable>(theObject);
    bool isShading = false;
    if (aPrs.get() != NULL) {
      GeomScreenParamsPtr aScreen = std::dynamic_pointer_cast<GeomAPI_IScreenParams>(theObject);
      if (aScreen.get()) {
        aScreen->setScreenPlane(getScreenPlane());
        aScreen->setViewScale(getViewScale());
      }
      anAIS = aPrs->getAISObject(anAIS);
      if (anAIS.get())
        myWorkshop->module()->customizePresentation(theObject, anAIS);
    } else {
      anAIS = myWorkshop->module()->createPresentation(theObject);
      isShading = true;
    }
    if (anAIS.get())
      aDisplayed = display(theObject, anAIS, isShading, theUpdateViewer);
  }
  return aDisplayed;
}


//**************************************************************
bool canBeShaded(Handle(AIS_InteractiveObject) theAIS, ModuleBase_IModule* theModule)
{
  Handle(AIS_Shape) aShapePrs = Handle(AIS_Shape)::DownCast(theAIS);
  if (!aShapePrs.IsNull()) {
    TopoDS_Shape aShape = aShapePrs->Shape();
    if (aShape.IsNull())
      return false;
    TopAbs_ShapeEnum aType = aShape.ShapeType();
    if ((aType == TopAbs_VERTEX) || (aType == TopAbs_EDGE) || (aType == TopAbs_WIRE))
      return false;
    else {
      // Check that the presentation is not a sketch
      return theModule->canBeShaded(theAIS);
    }
  }
  return false;
}

//**************************************************************
bool XGUI_Displayer::display(ObjectPtr theObject, AISObjectPtr theAIS,
                             bool isShading, bool theUpdateViewer)
{
  bool aDisplayed = false;

  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return aDisplayed;

  Handle(AIS_InteractiveObject) anAISIO = theAIS->impl<Handle(AIS_InteractiveObject)>();
  if (!anAISIO.IsNull()) {
    appendResultObject(theObject, theAIS);

    //bool isCustomized = customizeObject(theObject);

    int aDispMode = isShading? Shading : Wireframe;
    anAISIO->SetDisplayMode(aDispMode);
    aContext->Display(anAISIO, aDispMode, 0, false, AIS_DS_Displayed);
    #ifdef TINSPECTOR
    if (getCallBack()) getCallBack()->Display(anAISIO);
    #endif
    aDisplayed = true;

    emit objectDisplayed(theObject, theAIS);
    selectionActivate()->activate(anAISIO, theUpdateViewer);
  }
  if (theUpdateViewer)
    updateViewer();

  return aDisplayed;
}

//**************************************************************
bool XGUI_Displayer::erase(ObjectPtr theObject, const bool theUpdateViewer)
{
  bool aErased = false;
  if (!isVisible(theObject))
    return aErased;

  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return aErased;

  AISObjectPtr anObject = myResult2AISObjectMap.value(theObject);
  if (anObject) {
    Handle(AIS_InteractiveObject) anAIS = anObject->impl<Handle(AIS_InteractiveObject)>();
    if (!anAIS.IsNull()) {
      emit beforeObjectErase(theObject, anObject);
      aContext->Remove(anAIS, false/*update viewer*/);
      #ifdef TINSPECTOR
      if (getCallBack()) getCallBack()->Remove(anAIS);
      #endif
      aErased = true;
    }
  }
  myResult2AISObjectMap.remove(theObject);

#ifdef DEBUG_DISPLAY
  std::ostringstream aPtrStr;
  aPtrStr << theObject.get();
  qDebug(QString("erase object: %1").arg(aPtrStr.str().c_str()).toStdString().c_str());
  qDebug(getResult2AISObjectMapInfo().c_str());
#endif

  if (theUpdateViewer)
    updateViewer();

  return aErased;
}

//**************************************************************
bool XGUI_Displayer::redisplay(ObjectPtr theObject, bool theUpdateViewer)
{
  bool aRedisplayed = false;
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return aRedisplayed;

  if (!isVisible(theObject))
    return aRedisplayed;

  AISObjectPtr aAISObj = getAISObject(theObject);
  Handle(AIS_InteractiveObject) aAISIO;

  GeomPresentablePtr aPrs = std::dynamic_pointer_cast<GeomAPI_IPresentable>(theObject);
  if (aPrs) {
    GeomScreenParamsPtr aScreen = std::dynamic_pointer_cast<GeomAPI_IScreenParams>(theObject);
    if (aScreen.get()) {
      aScreen->setScreenPlane(getScreenPlane());
      aScreen->setViewScale(getViewScale());
    }
    AISObjectPtr aAIS_Obj = aPrs->getAISObject(aAISObj);
    if (!aAIS_Obj) {
      aRedisplayed = erase(theObject, theUpdateViewer);
      return aRedisplayed;
    }
    if (aAIS_Obj != aAISObj) {
      erase(theObject, theUpdateViewer);
      appendResultObject(theObject, aAIS_Obj);
    }
    aAISIO = aAIS_Obj->impl<Handle(AIS_InteractiveObject)>();
  }
  else {
    aAISIO = aAISObj->impl<Handle(AIS_InteractiveObject)>();
  }

  if (!aAISIO.IsNull()) {
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
    if (aResult.get()) {
      // Set color
      std::vector<int> aColor;
      ModelAPI_Tools::getColor(aResult, aColor);
      if (aColor.size() > 0) {
        Quantity_Color
          aCol(aColor[0] / 255., aColor[1] / 255., aColor[2] / 255., Quantity_TOC_RGB);
        aAISIO->SetColor(aCol);
      }
      // Set deflection
      double aDeflection = ModelAPI_Tools::getDeflection(aResult);
      if ((aDeflection >= 0) && (aDeflection != aAISObj->getDeflection()))
        aAISObj->setDeflection(aDeflection);

      // Set transparency
      double aTransparency = ModelAPI_Tools::getTransparency(aResult);
      if ((aTransparency >= 0) && (aTransparency != aAISObj->getTransparency()))
        aAISObj->setTransparency(aTransparency);

      // Set Iso-Lines
      Handle(ModuleBase_ResultPrs) aResPrs = Handle(ModuleBase_ResultPrs)::DownCast(aAISIO);
      if (!aResPrs.IsNull())
        aResPrs->updateIsoLines();
    }
    //myWorkshop->module()->storeSelection();

//#ifdef CLEAR_OUTDATED_SELECTION_BEFORE_REDISPLAY
    //myWorkshop->selector()->deselectPresentation(aAISIO);
//#endif

    if (aContext->IsDisplayed(aAISIO))
      aContext->Redisplay(aAISIO, false);
    else {
      aContext->Display(aAISIO, false);
    }
      #ifdef TINSPECTOR
      if (getCallBack()) getCallBack()->Redisplay(aAISIO);
      #endif

      //if (aNeedToRestoreSelection)
    //myWorkshop->module()->restoreSelection();

    aRedisplayed = true;
    #ifdef DEBUG_FEATURE_REDISPLAY
      qDebug("  Redisplay happens");
    #endif
    if (theUpdateViewer)
      updateViewer();
  }
  return aRedisplayed;
}

//**************************************************************
void XGUI_Displayer::redisplayObjects()
{
  // redisplay objects visualized in the viewer
  static Events_ID EVENT_DISP = Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY);
  static const ModelAPI_EventCreator* aECreator = ModelAPI_EventCreator::get();
  QObjectPtrList aDisplayed = displayedObjects();
  QObjectPtrList::const_iterator anIt = aDisplayed.begin(), aLast = aDisplayed.end();
  for (; anIt != aLast; anIt++) {
    aECreator->sendUpdated(*anIt, EVENT_DISP);
  }
  XGUI_ViewerProxy* aViewer = myWorkshop->viewer();
  if (aViewer->isColorScaleVisible()) {
    aViewer->setupColorScale();
    aViewer->setColorScaleShown(true);
  }
  Events_Loop::loop()->flush(EVENT_DISP);
}

//**************************************************************
void XGUI_Displayer::deactivateObjects(const QObjectPtrList& theObjList,
                                       const bool theUpdateViewer)
{
  //Handle(AIS_InteractiveObject) aTrihedron = getTrihedron();
  //if (!aTrihedron.IsNull())
  //  deactivateAIS(aTrihedron);

  QObjectPtrList::const_iterator anIt = theObjList.begin(), aLast = theObjList.end();
  for (; anIt != aLast; anIt++) {
    selectionActivate()->deactivate(*anIt, false);
  }
  //VSV It seems that there is no necessity to update viewer on deactivation
  //if (theUpdateViewer)
  //  updateViewer();
}

//**************************************************************
bool XGUI_Displayer::isVisible(XGUI_Displayer* theDisplayer, const ObjectPtr& theObject)
{
  bool aVisible = false;
  GeomPresentablePtr aPrs = std::dynamic_pointer_cast<GeomAPI_IPresentable>(theObject);
  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
  if (aPrs.get() || aResult.get()) {
    aVisible = theDisplayer->isVisible(theObject);
    // compsolid is not visualized in the viewer,
    // but should have presentation when all sub solids are
    // visible. It is useful for highlight presentation where compsolid shape is selectable
    if (!aVisible && aResult.get() && aResult->groupName() == ModelAPI_ResultBody::group()) {
      ResultBodyPtr aCompsolidResult = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aResult);
      if (aCompsolidResult.get() != NULL) { // change colors for all sub-solids
        int aNumberOfSubs = aCompsolidResult->numberOfSubs();
        bool anAllSubsVisible = aNumberOfSubs > 0;
        for(int i = 0; i < aNumberOfSubs && anAllSubsVisible; i++) {
          anAllSubsVisible = theDisplayer->isVisible(aCompsolidResult->subResult(i));
        }
        aVisible = anAllSubsVisible;
      }
    }
  }
  // it is possible that feature is presentable and has results, so we should check visibility
  // of results if presentation is not shown (e.g. Sketch Circle/Arc features)
  if (!aVisible) {
    // check if all results of the feature are visible
    FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
    std::list<ResultPtr> aResults;
    ModelAPI_Tools::allResults(aFeature, aResults);
    std::list<ResultPtr>::const_iterator aIt;
    aVisible = !aResults.empty();
    for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
      aVisible = aVisible && theDisplayer->isVisible(*aIt);
    }
  }
  return aVisible;
}

//**************************************************************
void XGUI_Displayer::setSelected(const  QList<ModuleBase_ViewerPrsPtr>& theValues,
                                 bool theUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return;
  aContext->UnhilightSelected(false);
  aContext->ClearSelected(false);
  #ifdef TINSPECTOR
  if (getCallBack()) getCallBack()->ClearSelected();
  #endif
  NCollection_DataMap<TopoDS_Shape, NCollection_Map<Handle(AIS_InteractiveObject)>>
    aShapesToBeSelected;

  foreach (ModuleBase_ViewerPrsPtr aPrs, theValues) {
    const GeomShapePtr& aGeomShape = aPrs->shape();
    if (aGeomShape.get() && !aGeomShape->isNull()) {
      const TopoDS_Shape& aShape = aGeomShape->impl<TopoDS_Shape>();
#ifdef DEBUG_OCCT_SHAPE_SELECTION
      // problem 1: performance
      // problem 2: IO is not specified, so the first found owner is selected, as a result
      // it might belong to another result
      aContext->AddOrRemoveSelected(aShape, false);
      #ifdef TINSPECTOR
      if (getCallBack()) getCallBack()->AddOrRemoveSelected(aShape);
      #endif
#else
      NCollection_Map<Handle(AIS_InteractiveObject)> aPresentations;
      if (aShapesToBeSelected.IsBound(aShape))
        aPresentations = aShapesToBeSelected.Find(aShape);
      ObjectPtr anObject = aPrs->object();
      getPresentations(anObject, aPresentations);

      aShapesToBeSelected.Bind(aShape, aPresentations);
#endif
    } else {
      ObjectPtr anObject = aPrs->object();
      ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
      if (aResult.get() && isVisible(aResult)) {
        AISObjectPtr anObj = myResult2AISObjectMap.value(aResult);
        Handle(AIS_InteractiveObject) anAIS = anObj->impl<Handle(AIS_InteractiveObject)>();
        if (!anAIS.IsNull()) {
          // The methods are replaced in order to provide multi-selection, e.g. restore selection
          // by activating multi selector widget. It also gives an advantage that the multi
          // selection in OB gives multi-selection in the viewer
          //aContext->SetSelected(anAIS, false);
          // The selection in the context was cleared, so the method sets the objects are selected
          aContext->AddOrRemoveSelected(anAIS, false);
          #ifdef TINSPECTOR
          if (getCallBack()) getCallBack()->AddOrRemoveSelected(anAIS);
          #endif
        }
      }
    }
  }
  if (!aShapesToBeSelected.IsEmpty())
    XGUI_Displayer::AddOrRemoveSelectedShapes(aContext, aShapesToBeSelected);

  if (theUpdateViewer)
    updateViewer();
}

//**************************************************************
void XGUI_Displayer::clearSelected(const bool theUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull()) {
    aContext->UnhilightSelected(false);//UnhilightCurrents(false);
    aContext->ClearSelected(theUpdateViewer);
    #ifdef TINSPECTOR
    if (getCallBack()) getCallBack()->ClearSelected();
    #endif
  }
}

//**************************************************************
bool XGUI_Displayer::eraseAll(const bool theUpdateViewer)
{
  bool aErased = false;
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull()) {
#ifdef OPTIMIZE_PRS
    foreach(ObjectPtr aObj, myResult2AISObjectMap.objects()) {
      AISObjectPtr aAISObj = myResult2AISObjectMap.value(aObj);
#else
    foreach(ObjectPtr aObj, myResult2AISObjectMap.keys()) {
      AISObjectPtr aAISObj = myResult2AISObjectMap[aObj];
#endif
      // erase an object
      Handle(AIS_InteractiveObject) anIO = aAISObj->impl<Handle(AIS_InteractiveObject)>();
      if (!anIO.IsNull()) {
        emit beforeObjectErase(aObj, aAISObj);
        aContext->Remove(anIO, false/*update viewer*/);
        #ifdef TINSPECTOR
        if (getCallBack()) getCallBack()->Remove(anIO);
        #endif
        aErased = true;
      }
    }
    if (theUpdateViewer)
      updateViewer();
  }
  myResult2AISObjectMap.clear();
#ifdef DEBUG_DISPLAY
  qDebug("eraseAll");
  qDebug(getResult2AISObjectMapInfo().c_str());
#endif
  return aErased;
}


//**************************************************************
AISObjectPtr XGUI_Displayer::getAISObject(ObjectPtr theObject) const
{
#ifdef OPTIMIZE_PRS
  return myResult2AISObjectMap.value(theObject);
#else
  AISObjectPtr anIO;
  if (myResult2AISObjectMap.contains(theObject))
    anIO = myResult2AISObjectMap[theObject];
  return anIO;
#endif
}

//**************************************************************
ObjectPtr XGUI_Displayer::getObject(const AISObjectPtr& theIO) const
{
  Handle(AIS_InteractiveObject) aRefAIS = theIO->impl<Handle(AIS_InteractiveObject)>();
  return getObject(aRefAIS);
}

//**************************************************************
ObjectPtr XGUI_Displayer::getObject(const Handle(AIS_InteractiveObject)& theIO) const
{
#ifdef OPTIMIZE_PRS
  ObjectPtr anObject = myResult2AISObjectMap.value(theIO);
#else
  ObjectPtr anObject;
  ResultToAISMap::const_iterator aMapIter = myResult2AISObjectMap.cbegin();
  for (; aMapIter != myResult2AISObjectMap.cend(); aMapIter++) {
    const AISObjectPtr& aAIS = aMapIter.value();
    Handle(AIS_InteractiveObject) anAIS = aAIS->impl<Handle(AIS_InteractiveObject)>();
    if (anAIS == theIO)
      anObject = aMapIter.key();
    if (anObject.get())
      break;
  }
#endif
  if (!anObject.get()) {
    std::shared_ptr<GeomAPI_AISObject> anAISObj = AISObjectPtr(new GeomAPI_AISObject());
    if (!theIO.IsNull()) {
      anAISObj->setImpl(new Handle(AIS_InteractiveObject)(theIO));
    }
    if (myWorkshop->module())
      anObject = myWorkshop->module()->findPresentedObject(anAISObj);
  }
  return anObject;
}

//**************************************************************
bool XGUI_Displayer::enableUpdateViewer(const bool isEnabled)
{
  bool aWasEnabled = isUpdateEnabled();
  if (isEnabled)
    myViewerBlockedRecursiveCount--;
  else
    myViewerBlockedRecursiveCount++;

#ifdef DEBUG_VIEWER_BLOCKED_COUNT
  std::cout << "myViewerBlockedRecursiveCount = " << myViewerBlockedRecursiveCount << std::endl;
#endif

  if (myNeedUpdate && isUpdateEnabled()) {
    updateViewer();
    myNeedUpdate = false;
  }
  return aWasEnabled;
}

//**************************************************************
void XGUI_Displayer::updateViewer() const
{
  Handle(AIS_InteractiveContext) aContext = AISContext();

#ifdef DEBUG_VIEWER_BLOCKED_COUNT
  std::cout << "updateViewer: " << (myViewerBlockedRecursiveCount == 0 ? " done" : " later")
            << std::endl;
#endif

  if (!aContext.IsNull() && isUpdateEnabled()) {
    //myWorkshop->viewer()->Zfitall();
    aContext->UpdateCurrentViewer();
  } else {
    myNeedUpdate = true;
  }
}

//**************************************************************
Handle(AIS_InteractiveContext) XGUI_Displayer::AISContext() const
{
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (!aContext.IsNull() && (myContextId != aContext.get())) {
    myContextId = aContext.get();
    if (!myWorkshop->selectionActivate()->isTrihedronActive())
      selectionActivate()->deactivateTrihedron(true);
    // Do not modify default drawer. The same is done in ModuleBase_ResultPrs
    //aContext->DefaultDrawer()->VIsoAspect()->SetNumber(0);
    //aContext->DefaultDrawer()->UIsoAspect()->SetNumber(0);

    // Commented out according to discussion in bug #2825
    ModuleBase_IViewer::DefaultHighlightDrawer = aContext->HighlightStyle();
    //Handle(Prs3d_Drawer) aSelStyle = aContext->SelectionStyle();
    //double aDeflection =
    //  QString(ModelAPI_ResultConstruction::DEFAULT_DEFLECTION().c_str()).toDouble();
    //try {
    //  aDeflection = Config_PropManager::real("Visualization", "construction_deflection");
    //} catch (...) {}

    //ModuleBase_IViewer::DefaultHighlightDrawer->SetDeviationCoefficient(aDeflection);
    //aSelStyle->SetDeviationCoefficient(aDeflection);

    Handle(AIS_Trihedron) aTrihedron = myWorkshop->viewer()->trihedron();
    if (!aTrihedron.IsNull())
      ModuleBase_Tools::setPointBallHighlighting(aTrihedron.get());
  }
  return aContext;
}

//**************************************************************
void XGUI_Displayer::setSelectionColor(const std::vector<int>& theColor)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull()) {
    Quantity_Color aQColor(theColor[0] / 255.,
                           theColor[1] / 255.,
                           theColor[2] / 255., Quantity_TOC_RGB);
    aContext->SelectionStyle()->SetColor(aQColor);
    aContext->SelectionStyle()->PointAspect()->SetColor(aQColor);
    aContext->SelectionStyle()->LineAspect()->SetColor(aQColor);
    aContext->HighlightStyle(Prs3d_TypeOfHighlight_LocalSelected)->SetColor(aQColor);
  }
}


//**************************************************************
std::vector<int> XGUI_Displayer::selectionColor() const
{
  std::vector<int> aColor;
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull()) {
    Quantity_Color aQColor = aContext->SelectionStyle()->Color();
    aColor.push_back((int)(aQColor.Red() * 255));
    aColor.push_back((int)(aQColor.Green() * 255));
    aColor.push_back((int)(aQColor.Blue() * 255));
  }
  return aColor;
}


//**************************************************************
Handle(SelectMgr_AndFilter) XGUI_Displayer::GetFilter()
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull() && myAndFilter.IsNull()) {
    myAndFilter = new SelectMgr_AndFilter();
    aContext->AddFilter(myAndFilter);
  }
  return myAndFilter;
}

//**************************************************************
bool XGUI_Displayer::displayAIS(AISObjectPtr theAIS, const bool toActivateInSelectionModes,
                                const Standard_Integer theDisplayMode, bool theUpdateViewer)
{
  bool aDisplayed = false;
  Handle(AIS_InteractiveContext) aContext = AISContext();
  Handle(AIS_InteractiveObject) anAISIO = theAIS->impl<Handle(AIS_InteractiveObject)>();
  if (!aContext.IsNull() && !anAISIO.IsNull()) {
    aContext->Display(anAISIO, theDisplayMode, 0, false/*update viewer*/, AIS_DS_Displayed);
    #ifdef TINSPECTOR
    if (getCallBack()) getCallBack()->Display(anAISIO);
    #endif
    aDisplayed = true;
    aContext->Deactivate(anAISIO);
    #ifdef TINSPECTOR
    if (getCallBack()) getCallBack()->Deactivate(anAISIO);
    #endif
    aContext->Load(anAISIO);
    #ifdef TINSPECTOR
    if (getCallBack()) getCallBack()->Load(anAISIO);
    #endif
    if (toActivateInSelectionModes)
      myWorkshop->selectionActivate()->activateOnDisplay(anAISIO, theUpdateViewer);

    if (theUpdateViewer)
      updateViewer();
  }
  return aDisplayed;
}

//**************************************************************
bool XGUI_Displayer::eraseAIS(AISObjectPtr theAIS, const bool theUpdateViewer)
{
  bool aErased = false;
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull()) {
    Handle(AIS_InteractiveObject) anAISIO = theAIS->impl<Handle(AIS_InteractiveObject)>();
    if (!anAISIO.IsNull() && aContext->IsDisplayed(anAISIO)) {
      aContext->Remove(anAISIO, false/*update viewer*/);
      #ifdef TINSPECTOR
      if (getCallBack()) getCallBack()->Remove(anAISIO);
      #endif
      aErased = true;
    }
  }
  if (aErased && theUpdateViewer)
    updateViewer();
  return aErased;
}

//**************************************************************
void XGUI_Displayer::setDisplayMode(ObjectPtr theObject, DisplayMode theMode, bool theUpdateViewer)
{
  if (theMode == NoMode)
    return;

  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return;

  AISObjectPtr aAISObj = getAISObject(theObject);
  if (!aAISObj)
    return;

  Handle(AIS_InteractiveObject) aAISIO = aAISObj->impl<Handle(AIS_InteractiveObject)>();
  aContext->SetDisplayMode(aAISIO, theMode, false);
  // Redisplay in order to update new mode because it could be not computed before
  if (theUpdateViewer)
    updateViewer();
}

//**************************************************************
XGUI_Displayer::DisplayMode XGUI_Displayer::displayMode(ObjectPtr theObject) const
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return NoMode;

  AISObjectPtr aAISObj = getAISObject(theObject);
  if (!aAISObj)
    return NoMode;

  Handle(AIS_InteractiveObject) aAISIO = aAISObj->impl<Handle(AIS_InteractiveObject)>();
  return (XGUI_Displayer::DisplayMode) aAISIO->DisplayMode();
}

//**************************************************************
void XGUI_Displayer::deactivateSelectionFilters(const bool theAddFilterOnly)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!myAndFilter.IsNull()) {
    bool aFound = false;
    if (!aContext.IsNull()) {
      const SelectMgr_ListOfFilter& aFilters = aContext->Filters();
      SelectMgr_ListIteratorOfListOfFilter anIt(aFilters);
      for (; anIt.More() && !aFound; anIt.Next()) {
        Handle(SelectMgr_Filter) aFilter = anIt.Value();
        aFound = aFilter == myAndFilter;
      }
      if (aFound)
        aContext->RemoveFilter(myAndFilter);
    }
    myAndFilter.Nullify();
  }
}

//**************************************************************
void XGUI_Displayer::addSelectionFilter(const Handle(SelectMgr_Filter)& theFilter)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull() || hasSelectionFilter(theFilter))
    return;

  Handle(SelectMgr_CompositionFilter) aCompositeFilter = GetFilter();
  if (!aCompositeFilter.IsNull()) {
    aCompositeFilter->Add(theFilter);
#ifdef DEBUG_SELECTION_FILTERS
    int aCount = aCompositeFilter->StoredFilters().Extent();
    qDebug(QString("addSelectionFilter: filters.count() = %1").arg(aCount).toStdString().c_str());
#endif
  }
}

//**************************************************************
void XGUI_Displayer::removeSelectionFilter(const Handle(SelectMgr_Filter)& theFilter)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return;

  Handle(SelectMgr_AndFilter) aCompositeFilter = GetFilter();
  if (!aCompositeFilter.IsNull() && aCompositeFilter->IsIn(theFilter)) {
    aCompositeFilter->Remove(theFilter);
#ifdef DEBUG_SELECTION_FILTERS
    int aCount = aCompositeFilter->StoredFilters().Extent();
    qDebug(QString("removeSelectionFilter: filters.count() = %1")
      .arg(aCount).toStdString().c_str());
#endif
  }
}

//**************************************************************
bool XGUI_Displayer::hasSelectionFilter(const Handle(SelectMgr_Filter)& theFilter)
{
  bool aFilterFound = false;

  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return aFilterFound;
  const SelectMgr_ListOfFilter& aFilters = aContext->Filters();
  SelectMgr_ListIteratorOfListOfFilter aIt(aFilters);
  for (; aIt.More() && !aFilterFound; aIt.Next()) {
    if (theFilter.get() == aIt.Value().get())
      aFilterFound = true;
  }
  Handle(SelectMgr_CompositionFilter) aCompositeFilter = GetFilter();
  if (!aCompositeFilter.IsNull()) {
    const SelectMgr_ListOfFilter& aStoredFilters = aCompositeFilter->StoredFilters();
    for (aIt.Initialize(aStoredFilters); aIt.More() && !aFilterFound; aIt.Next()) {
      if (theFilter.get() == aIt.Value().get())
        aFilterFound = true;
    }
  }
  return aFilterFound;
}

//**************************************************************
void XGUI_Displayer::removeFilters()
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return;

  Handle(SelectMgr_CompositionFilter) aCompositeFilter = GetFilter();
  if (!aCompositeFilter.IsNull())
    aCompositeFilter->Clear();
}

//**************************************************************
void XGUI_Displayer::showOnly(const QObjectPtrList& theList)
{
#ifdef OPTIMIZE_PRS
  QObjectPtrList aDispList = myResult2AISObjectMap.objects();
#else
  QObjectPtrList aDispList = myResult2AISObjectMap.keys();
#endif
  foreach(ObjectPtr aObj, aDispList) {
    if (!theList.contains(aObj))
      erase(aObj, false);
  }
  foreach(ObjectPtr aObj, theList) {
    if (!isVisible(aObj))
      display(aObj, false);
  }
  updateViewer();
}

//**************************************************************
bool XGUI_Displayer::canBeShaded(ObjectPtr theObject) const
{
  if (!isVisible(theObject))
    return false;

  AISObjectPtr aAISObj = getAISObject(theObject);
  if (aAISObj.get() == NULL)
    return false;

  Handle(AIS_InteractiveObject) anAIS = aAISObj->impl<Handle(AIS_InteractiveObject)>();
  return ::canBeShaded(anAIS, myWorkshop->module());
}

//**************************************************************
//bool XGUI_Displayer::customizeObject(ObjectPtr theObject)
//{
//  AISObjectPtr anAISObj = getAISObject(theObject);
//  // correct the result's color it it has the attribute
//  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
//
//  // Customization of presentation
//  GeomCustomPrsPtr aCustomPrs;
//  FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
//  if (aFeature.get() != NULL) {
//    GeomCustomPrsPtr aCustPrs = std::dynamic_pointer_cast<GeomAPI_ICustomPrs>(aFeature);
//    if (aCustPrs.get() != NULL)
//      aCustomPrs = aCustPrs;
//  }
//  if (aCustomPrs.get() == NULL) {
//    GeomPresentablePtr aPrs = std::dynamic_pointer_cast<GeomAPI_IPresentable>(theObject);
//    // we ignore presentable not customized objects
//    if (aPrs.get() == NULL)
//      aCustomPrs = myCustomPrs;
//  }
//  bool isCustomized = aCustomPrs.get() &&
//                      aCustomPrs->customisePresentation(aResult, anAISObj, myCustomPrs);
//  isCustomized = myWorkshop->module()->afterCustomisePresentation(aResult, anAISObj, myCustomPrs)
//                 || isCustomized;
//
//  // update presentation state if faces panel is active
//  if (anAISObj.get() && myWorkshop->facesPanel())
//    isCustomized = myWorkshop->facesPanel()->customizeObject(theObject, anAISObj) || isCustomized;
//
//  return isCustomized;
//}

//**************************************************************
QColor XGUI_Displayer::setObjectColor(ObjectPtr theObject,
                                      const QColor& theColor,
                                      bool theUpdateViewer)
{
  if (!isVisible(theObject))
    return Qt::black;

  AISObjectPtr anAISObj = getAISObject(theObject);
  int aR, aG, aB;
  anAISObj->getColor(aR, aG, aB);
  anAISObj->setColor(theColor.red(), theColor.green(), theColor.blue());
  if (theUpdateViewer)
    updateViewer();
  return QColor(aR, aG, aB);
}

//**************************************************************
void XGUI_Displayer::appendResultObject(ObjectPtr theObject, AISObjectPtr theAIS)
{
#ifdef OPTIMIZE_PRS
  myResult2AISObjectMap.add(theObject, theAIS);
#else
  myResult2AISObjectMap[theObject] = theAIS;
#endif

#ifdef DEBUG_DISPLAY
  std::ostringstream aPtrStr;
  aPtrStr << theObject.get();
  qDebug(QString("display object: %1").arg(aPtrStr.str().c_str()).toStdString().c_str());
  qDebug(getResult2AISObjectMapInfo().c_str());
#endif
}

#ifdef _DEBUG
//**************************************************************
std::string XGUI_Displayer::getResult2AISObjectMapInfo() const
{
  QStringList aContent;
#ifdef OPTIMIZE_PRS
  foreach(ObjectPtr aObj, myResult2AISObjectMap.objects()) {
    AISObjectPtr aAISObj = myResult2AISObjectMap.value(aObj);
#else
  foreach(ObjectPtr aObj, myResult2AISObjectMap.keys()) {
    AISObjectPtr aAISObj = myResult2AISObjectMap[aObj];
#endif
    std::ostringstream aPtrStr;
    aPtrStr << "aObj = " << aObj.get() << ":";
    aPtrStr << "anAIS = " << aAISObj.get() << ":";
    aPtrStr << "[" << ModuleBase_Tools::objectInfo(aObj).toStdString().c_str() << "]";

    aContent.append(aPtrStr.str().c_str());
  }
  return QString("myResult2AISObjectMap: size = %1\n%2\n").arg(myResult2AISObjectMap.size()).
                                            arg(aContent.join("\n")).toStdString().c_str();
}
#endif

//**************************************************************
void XGUI_Displayer::getPresentations(const ObjectPtr& theObject,
                                  NCollection_Map<Handle(AIS_InteractiveObject)>& thePresentations)
{
  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
  if (aResult.get()) {
    AISObjectPtr aAISObj = getAISObject(aResult);
    if (aAISObj.get() == NULL) {
      // if result is a result of a composite feature, it is visualized by visualization of
      // composite children, so we should get one of this presentations
      ResultBodyPtr aCompSolid = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aResult);
      if (aCompSolid.get() && aCompSolid->numberOfSubs() > 0) {
        aAISObj = getAISObject(aCompSolid->subResult(0));
      }
    }
    if (aAISObj.get() != NULL) {
      Handle(AIS_InteractiveObject) anAIS = aAISObj->impl<Handle(AIS_InteractiveObject)>();
      if (!anAIS.IsNull() && !thePresentations.Contains(anAIS))
        thePresentations.Add(anAIS);
    }
  }
  else {
    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObject);
    // find presentation of the feature
    AISObjectPtr aAISObj = getAISObject(aFeature);
    if (aAISObj.get() != NULL) {
      Handle(AIS_InteractiveObject) anAIS = aAISObj->impl<Handle(AIS_InteractiveObject)>();
      if (!anAIS.IsNull() && !thePresentations.Contains(anAIS))
        thePresentations.Add(anAIS);
    }
    // find presentations of the feature results
    std::list<ResultPtr> aResults;
    ModelAPI_Tools::allResults(aFeature, aResults);
    std::list<ResultPtr>::const_iterator anIt = aResults.begin(), aLast = aResults.end();
    for (; anIt != aLast; ++anIt) {
      AISObjectPtr aCurAISObj = getAISObject(*anIt);
      if (aCurAISObj.get() != NULL) {
        Handle(AIS_InteractiveObject) anAIS = aCurAISObj->impl<Handle(AIS_InteractiveObject)>();
        if (!anAIS.IsNull() && !thePresentations.Contains(anAIS))
          thePresentations.Add(anAIS);
      }
    }
  }
}

//**************************************************************
void XGUI_Displayer::displayTrihedron(bool theToDisplay) const
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return;

  Handle(AIS_Trihedron) aTrihedron = myWorkshop->viewer()->trihedron();

  XGUI_SelectionActivate* aSelectionActive = selectionActivate();
  if (theToDisplay) {
    if (!aContext->IsDisplayed(aTrihedron))
      aContext->Display(aTrihedron,
                        0 /*wireframe*/,
                        -1 /* selection mode */,
                        Standard_True /* update viewer*/,
                        AIS_DS_Displayed /* xdisplay status */);
    #ifdef TINSPECTOR
    if (getCallBack()) getCallBack()->Display(aTrihedron);
    #endif

    if (!aSelectionActive->isTrihedronActive())
      aSelectionActive->deactivateTrihedron(false);
    else
      aSelectionActive->activate(aTrihedron, false);
  } else {
    aSelectionActive->deactivateTrihedron(false);

    aContext->Erase(aTrihedron, Standard_True);
    #ifdef TINSPECTOR
    if (getCallBack()) getCallBack()->Remove(aTrihedron);
    #endif
  }
}

//**************************************************************
void XGUI_Displayer::AddOrRemoveSelectedShapes(Handle(AIS_InteractiveContext) theContext,
                           const NCollection_DataMap<TopoDS_Shape,
                           NCollection_Map<Handle(AIS_InteractiveObject)>>& theShapesToBeSelected)
{
  NCollection_Map<Handle(AIS_InteractiveObject)> aCompsolidPresentations;
  NCollection_Map<Handle(AIS_InteractiveObject)> aSelectedPresentations;

  NCollection_List<Handle(SelectBasics_EntityOwner)> anActiveOwners;
  theContext->MainSelector()->ActiveOwners(anActiveOwners);
  NCollection_List<Handle(SelectBasics_EntityOwner)>::Iterator anOwnersIt (anActiveOwners);
  Handle(SelectMgr_EntityOwner) anOwner;

  /// It is very important to check that the owner is processed only once and has a map of
  /// processed owners because SetSelected works as a switch.
  /// If count of calls setSelectec is even, the object stays in the previous state
  /// (selected, deselected)
  /// OCCT: to write about the problem that active owners method returns one owner several times
  QList<size_t> aSelectedIds; // Remember of selected address in order to avoid duplicates
  for (; anOwnersIt.More(); anOwnersIt.Next()) {
    anOwner = anOwnersIt.Value();
    if (aSelectedIds.contains((size_t)anOwner.get()))
      continue;
    aSelectedIds.append((size_t)anOwner.get());

    Handle(StdSelect_BRepOwner) BROwnr = Handle(StdSelect_BRepOwner)::DownCast(anOwner);
    if (!BROwnr.IsNull() && BROwnr->HasShape()) {
      const TopoDS_Shape& aShape = BROwnr->Shape();

      Handle(ModuleBase_ResultPrs) aResPrs =
          Handle(ModuleBase_ResultPrs)::DownCast(BROwnr->Selectable());
      TopoDS_Shape aRealShape;
      if (!aResPrs.IsNull()) {
        aRealShape = aResPrs->originalShape();
      }

      if (aShape.IsNull())
        continue;

      Handle(ModuleBase_BRepOwner) aCustomOwner = Handle(ModuleBase_BRepOwner)::DownCast(anOwner);

      NCollection_DataMap<TopoDS_Shape, NCollection_Map<Handle(AIS_InteractiveObject)> >
                                             ::Iterator aShapeIt(theShapesToBeSelected);
      for (; aShapeIt.More(); aShapeIt.Next()) {
        const TopoDS_Shape& aParameterShape = aShapeIt.Key();
        // In case of compound we cannot rely on simple comparison method.
        // If the compound is generated by Group feature then this compound is alwais new.
        // So, we have to compare content of these compounds

          // isSame should be used here as it does not check orientation of shapes
          // despite on isEqual of shapes or IsBound for shape in QMap. Orientation is
          // different for Edges shapes in model shape and owner even if this is the same shape
        bool isSame = ModuleBase_Tools::isSameShape(aParameterShape, aShape);
        if (!isSame) {
          // In case of using HideFaces panel we can have instead of an original shape
          // a compaund of faces which represent original shape with hidden faces.
          // So, we have to compare the parameter with original shape
          if (!aRealShape.IsNull())
            isSame = ModuleBase_Tools::isSameShape(aParameterShape, aRealShape);
        }
        if (isSame) {
          Handle(AIS_InteractiveObject) anOwnerPresentation =
            Handle(AIS_InteractiveObject)::DownCast(anOwner->Selectable());
          NCollection_Map<Handle(AIS_InteractiveObject)> aPresentations =
            theShapesToBeSelected.Find(aParameterShape);
          if (aPresentations.Contains(anOwnerPresentation)) {
            theContext->AddOrRemoveSelected(anOwner, Standard_False);
            anOwner->SetSelected(Standard_True);
            // collect selected presentations to do not select them if compsolid is selected
            if (!aSelectedPresentations.Contains(anOwnerPresentation))
              aSelectedPresentations.Add(anOwnerPresentation);
          }
        }
        else if (!aCustomOwner.IsNull()) { // CompSolid processing #2219
          // shape of owner is compound, but shape to be selected is compsolid, so
          // we need to compare shape to AIS presentation of owner(rule of the owner creation)
          Handle(AIS_Shape) anOwnerPresentation =
                            Handle(AIS_Shape)::DownCast(anOwner->Selectable());
          const TopoDS_Shape& aPresentationShape = anOwnerPresentation->Shape();
          if (aParameterShape.IsSame(aPresentationShape) &&
              !aCompsolidPresentations.Contains(anOwnerPresentation))
            aCompsolidPresentations.Add(anOwnerPresentation);
        }
      }
    }
  }
  // select CompSolid presentations if their owners was not selected yet
  NCollection_Map<Handle(AIS_InteractiveObject)>::Iterator anIt (aCompsolidPresentations);
  for (; anIt.More(); anIt.Next()) {
    if (aSelectedPresentations.Contains(anIt.Value()))
      continue;
    theContext->AddOrRemoveSelected(anIt.Value(), Standard_False);
  }
}

//**************************************************************
XGUI_SelectionActivate* XGUI_Displayer::selectionActivate() const
{
  return myWorkshop->selectionActivate();
}

//**************************************************************
GeomPlanePtr XGUI_Displayer::getScreenPlane() const
{
  GeomPlanePtr aResult;
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull()) {
    Handle(V3d_Viewer) aViewer = aContext->CurrentViewer();
    Handle(V3d_View) aView;
    for (aViewer->InitActiveViews(); aViewer->MoreActiveViews(); aViewer->NextActiveViews()) {
      aView = aViewer->ActiveView();
      break;
    }
    if (!aView.IsNull()) {
      double aEyeX, aEyeY, aEyeZ;
      aView->Eye(aEyeX, aEyeY, aEyeZ);

      double aProjX, aProjY, aProjZ;
      aView->Proj(aProjX, aProjY, aProjZ);

      GeomPointPtr aPnt = GeomPointPtr(new GeomAPI_Pnt(aEyeX, aEyeY, aEyeZ));
      GeomDirPtr aDir = GeomDirPtr(new GeomAPI_Dir(aProjX, aProjY, aProjZ));

      aResult = GeomPlanePtr(new GeomAPI_Pln(aPnt, aDir));
    }
  }
  return aResult;
}

double XGUI_Displayer::getViewScale() const
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull()) {
    Handle(V3d_Viewer) aViewer = aContext->CurrentViewer();
    Handle(V3d_View) aView;
    for (aViewer->InitActiveViews(); aViewer->MoreActiveViews(); aViewer->NextActiveViews()) {
      aView = aViewer->ActiveView();
      break;
    }
    return aView->Camera()->Scale();
  }
  return 1;
}
