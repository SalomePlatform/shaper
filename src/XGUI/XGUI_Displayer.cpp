// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        XGUI_Displayer.cpp
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "XGUI_Displayer.h"
#include "XGUI_Workshop.h"
#include "XGUI_ViewerProxy.h"
#include "XGUI_SelectionMgr.h"
#include "XGUI_Selection.h"
#include "XGUI_CustomPrs.h"

#include <AppElements_Viewer.h>

#include <ModelAPI_Document.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_ResultCompSolid.h>

#include <ModuleBase_ResultPrs.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_IModule.h>

#include <GeomAPI_Shape.h>
#include <GeomAPI_IPresentable.h>
#include <GeomAPI_ICustomPrs.h>

#include <AIS_InteractiveContext.hxx>
#include <AIS_LocalContext.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_DimensionSelectionMode.hxx>
#include <AIS_Shape.hxx>
#include <AIS_Dimension.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <SelectMgr_ListOfFilter.hxx>
#include <SelectMgr_ListIteratorOfListOfFilter.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_IsoAspect.hxx>

#include <StdSelect_ViewerSelector3d.hxx>

#include <TColStd_MapOfTransient.hxx>
#include <TColStd_MapIteratorOfMapOfTransient.hxx>

#include <Events_Loop.h>
#include <ModelAPI_Events.h>

#include <set>

const int MOUSE_SENSITIVITY_IN_PIXEL = 10;  ///< defines the local context mouse selection sensitivity

//#define DEBUG_ACTIVATE_OBJECTS
//#define DEBUG_DEACTIVATE
//#define DEBUG_ACTIVATE_AIS
//#define DEBUG_DEACTIVATE_AIS

//#define DEBUG_DISPLAY
//#define DEBUG_FEATURE_REDISPLAY
//#define DEBUG_SELECTION_FILTERS

//#define DEBUG_COMPOSILID_DISPLAY
// Workaround for bug #25637
void displayedObjects(const Handle(AIS_InteractiveContext)& theAIS, AIS_ListOfInteractive& theList)
{
  // Get from null point
  theAIS->DisplayedObjects(theList, true);
  if (theAIS->HasOpenedContext()) {
    // get from local context
    const Handle(AIS_LocalContext)& aLC = theAIS->LocalContext();
    TColStd_MapOfTransient aMap;
    int NbDisp = aLC->DisplayedObjects(aMap);
    TColStd_MapIteratorOfMapOfTransient aIt(aMap);

    Handle(AIS_InteractiveObject) curIO;
    Handle(Standard_Transient) Tr;
    for(; aIt.More(); aIt.Next()){
      Tr = aIt.Key();
      curIO = *((Handle(AIS_InteractiveObject)*) &Tr);
      theList.Append(curIO);
    }
  }
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

XGUI_Displayer::XGUI_Displayer(XGUI_Workshop* theWorkshop)
  : myWorkshop(theWorkshop)
{
  enableUpdateViewer(true);
  myCustomPrs = std::shared_ptr<GeomAPI_ICustomPrs>(new XGUI_CustomPrs());
}

XGUI_Displayer::~XGUI_Displayer()
{
}

bool XGUI_Displayer::isVisible(ObjectPtr theObject) const
{
  return myResult2AISObjectMap.contains(theObject);
}

bool XGUI_Displayer::display(ObjectPtr theObject, bool theUpdateViewer)
{
  bool aDisplayed = false;
  if (isVisible(theObject)) {
#ifdef DEBUG_COMPOSILID_DISPLAY
    ResultCompSolidPtr aCompsolidResult = std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(theObject);
    if (aCompsolidResult.get()) {
      for(int i = 0; i < aCompsolidResult->numberOfSubs(); i++) {
        ResultPtr aSubResult = aCompsolidResult->subResult(i);
        if (aSubResult.get())
          redisplay(aSubResult, false);
      }
      if (theUpdateViewer)
        updateViewer();
    }
    else
#endif
    aDisplayed = redisplay(theObject, theUpdateViewer);
  } else {
    AISObjectPtr anAIS;
    GeomPresentablePtr aPrs = std::dynamic_pointer_cast<GeomAPI_IPresentable>(theObject);
    bool isShading = false;
    if (aPrs.get() != NULL) {
      anAIS = aPrs->getAISObject(anAIS);
    } else {
      ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
      if (aResult.get() != NULL) {
#ifdef DEBUG_COMPOSILID_DISPLAY
        ResultCompSolidPtr aCompsolidResult = std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(theObject);
        if (aCompsolidResult.get()) {
          for(int i = 0; i < aCompsolidResult->numberOfSubs(); i++) {
            ResultPtr aSubResult = aCompsolidResult->subResult(i);
            if (aSubResult.get())
              display(aSubResult, false);
          }
          if (theUpdateViewer)
            updateViewer();
        }
        else {
#endif
        std::shared_ptr<GeomAPI_Shape> aShapePtr = ModelAPI_Tools::shape(aResult);
        if (aShapePtr.get() != NULL) {
          anAIS = AISObjectPtr(new GeomAPI_AISObject());
          anAIS->setImpl(new Handle(AIS_InteractiveObject)(new ModuleBase_ResultPrs(aResult)));
          //anAIS->createShape(aShapePtr);
          isShading = true;
        }
#ifdef DEBUG_COMPOSILID_DISPLAY
        } // close else
#endif
      }
    }
    if (anAIS)
      aDisplayed = display(theObject, anAIS, isShading, theUpdateViewer);
  }
  return aDisplayed;
}

bool canBeShaded(Handle(AIS_InteractiveObject) theAIS)
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
      Handle(ModuleBase_ResultPrs) aPrs = Handle(ModuleBase_ResultPrs)::DownCast(theAIS);
      if (!aPrs.IsNull()) 
        return !aPrs->isSketchMode();
      return true;
    }
  }
  return false;
}

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

    bool isCustomized = customizeObject(theObject);

    int aDispMode = isShading? Shading : Wireframe;
    if (isShading)
      anAISIO->Attributes()->SetFaceBoundaryDraw( Standard_True );
    anAISIO->SetDisplayMode(aDispMode);
    aContext->Display(anAISIO, aDispMode, 0, false, true, AIS_DS_Displayed); 
    aDisplayed = true;

    emit objectDisplayed(theObject, theAIS);
    activate(anAISIO, myActiveSelectionModes, theUpdateViewer);
    // the fix from VPA for more suitable selection of sketcher lines
    if(anAISIO->Width() > 1) {
      for(int aModeIdx = 0; aModeIdx < myActiveSelectionModes.length(); ++aModeIdx) {
        aContext->SetSelectionSensitivity(anAISIO,
          myActiveSelectionModes.value(aModeIdx), anAISIO->Width() + 2);
      }
    }
  } 
  if (theUpdateViewer)
    updateViewer();

  return aDisplayed;
}

bool XGUI_Displayer::erase(ObjectPtr theObject, const bool theUpdateViewer)
{
  bool aErased = false;
  if (!isVisible(theObject))
    return aErased;

  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return aErased;

  AISObjectPtr anObject = myResult2AISObjectMap[theObject];
  if (anObject) {
    Handle(AIS_InteractiveObject) anAIS = anObject->impl<Handle(AIS_InteractiveObject)>();
    if (!anAIS.IsNull()) {
      emit beforeObjectErase(theObject, anObject);
      aContext->Remove(anAIS, theUpdateViewer);
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
  return aErased;
}

bool XGUI_Displayer::redisplay(ObjectPtr theObject, bool theUpdateViewer)
{
  bool aRedisplayed = false;
  if (!isVisible(theObject))
    return aRedisplayed;

  AISObjectPtr aAISObj = getAISObject(theObject);
  Handle(AIS_InteractiveObject) aAISIO = aAISObj->impl<Handle(AIS_InteractiveObject)>();

  GeomPresentablePtr aPrs = std::dynamic_pointer_cast<GeomAPI_IPresentable>(theObject);
  if (aPrs) {
    AISObjectPtr aAIS_Obj = aPrs->getAISObject(aAISObj);
    if (!aAIS_Obj) {
      aRedisplayed = erase(theObject, theUpdateViewer);
      return aRedisplayed;
    }
    if (aAIS_Obj != aAISObj) {
      appendResultObject(theObject, aAIS_Obj);
    }
    aAISIO = aAIS_Obj->impl<Handle(AIS_InteractiveObject)>();
  }

  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull() && !aAISIO.IsNull()) {
    // Check that the visualized shape is the same and the redisplay is not necessary
    // Redisplay of AIS object leads to this object selection compute and the selection 
    // in the browser is lost
    // this check is not necessary anymore because the selection store/restore is realized
    // before and after the values modification.
    // Moreother, this check avoids customize and redisplay presentation if the presentable
    // parameter is changed.
    bool isEqualShapes = false;
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
    if (aResult.get() != NULL) {
      Handle(AIS_Shape) aShapePrs = Handle(AIS_Shape)::DownCast(aAISIO);
      if (!aShapePrs.IsNull()) {
        std::shared_ptr<GeomAPI_Shape> aShapePtr = ModelAPI_Tools::shape(aResult);
        if (aShapePtr.get()) {
          const TopoDS_Shape& aOldShape = aShapePrs->Shape();
          if (!aOldShape.IsNull())
            isEqualShapes = aOldShape.IsEqual(aShapePtr->impl<TopoDS_Shape>());
        }
      }
    }
    // Customization of presentation
    bool isCustomized = customizeObject(theObject);
    #ifdef DEBUG_FEATURE_REDISPLAY
      qDebug(QString("Redisplay: %1, isEqualShapes=%2, isCustomized=%3").
        arg(!isEqualShapes || isCustomized).arg(isEqualShapes).arg(isCustomized).toStdString().c_str());
    #endif
    if (!isEqualShapes || isCustomized) {
      aContext->Redisplay(aAISIO, false);
      aRedisplayed = true;
      #ifdef DEBUG_FEATURE_REDISPLAY
        qDebug("  Redisplay happens");
      #endif
      if (theUpdateViewer)
        updateViewer();
    }
  }
  return aRedisplayed;
}

void XGUI_Displayer::redisplayObjects()
{
  // redisplay objects visualized in the viewer
  static Events_ID EVENT_DISP = Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY);
  static const ModelAPI_EventCreator* aECreator = ModelAPI_EventCreator::get();
  QObjectPtrList aDisplayed = myWorkshop->displayer()->displayedObjects();
  QObjectPtrList::const_iterator anIt = aDisplayed.begin(), aLast = aDisplayed.end();
  for (; anIt != aLast; anIt++) {
    aECreator->sendUpdated(*anIt, EVENT_DISP);
  }
  Events_Loop::loop()->flush(EVENT_DISP);
}

void XGUI_Displayer::deactivate(ObjectPtr theObject, const bool theUpdateViewer)
{
#ifdef DEBUG_DEACTIVATE
  QString anInfoStr = ModuleBase_Tools::objectInfo(theObject);
  qDebug(QString("deactivate: myActiveSelectionModes[%1]: %2, objects = ").
    arg(myActiveSelectionModes.size()).arg(qIntListInfo(myActiveSelectionModes)).
    arg(anInfoStr).
    toStdString().c_str());
#endif
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull() && isVisible(theObject)) {
    AISObjectPtr anObj = myResult2AISObjectMap[theObject];
    Handle(AIS_InteractiveObject) anAIS = anObj->impl<Handle(AIS_InteractiveObject)>();

    deactivateAIS(anAIS);
    // the selection from the previous activation modes should be cleared manually (#26172)
    aContext->LocalContext()->ClearOutdatedSelection(anAIS, true);
    if (theUpdateViewer)
      updateViewer();
  }
}

void XGUI_Displayer::deactivateObjects(const QObjectPtrList& theObjList,
                                       const bool theUpdateViewer)
{
  QObjectPtrList::const_iterator anIt = theObjList.begin(), aLast = theObjList.end();
  for (; anIt != aLast; anIt++) {
    deactivate(*anIt, false);
  }
  if (theUpdateViewer)
    updateViewer();
}

void XGUI_Displayer::getModesOfActivation(ObjectPtr theObject, QIntList& theModes)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull() || !isVisible(theObject))
    return;

  AISObjectPtr aAISObj = getAISObject(theObject);

  if (aAISObj.get() != NULL) {
    Handle(AIS_InteractiveObject) anAISIO = aAISObj->impl<Handle(AIS_InteractiveObject)>();
    TColStd_ListOfInteger aTColModes;
    aContext->ActivatedModes(anAISIO, aTColModes);
    TColStd_ListIteratorOfListOfInteger itr( aTColModes );
    for (; itr.More(); itr.Next() ) {
      theModes.append(itr.Value());
    }
  }
}

void XGUI_Displayer::activateObjects(const QIntList& theModes, const QObjectPtrList& theObjList,
                                     const bool theUpdateViewer)
{
  // Convert shape types to selection types
  QIntList aModes;
  foreach(int aType, theModes) {
    if (aType >= TopAbs_SHAPE) 
      aModes.append(aType);
    else
      aModes.append(AIS_Shape::SelectionMode((TopAbs_ShapeEnum)aType));
  }

#ifdef DEBUG_ACTIVATE_OBJECTS
  QStringList anInfo;
  QObjectPtrList::const_iterator anIt = theObjList.begin(), aLast = theObjList.end();
  for (; anIt != aLast; ++anIt) {
    anInfo.append(ModuleBase_Tools::objectInfo((*anIt)));
  }
  QString anInfoStr = anInfo.join(", ");

  qDebug(QString("activateObjects: aModes[%1] = %2, myActiveSelectionModes[%3] = %4, objects = %5").
    arg(aModes.size()).arg(qIntListInfo(aModes)).
    arg(myActiveSelectionModes.size()).arg(qIntListInfo(myActiveSelectionModes)).
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
  Handle(AIS_InteractiveContext) aContext = AISContext();
  // Open local context if there is no one
  if (aContext.IsNull() || !aContext->HasOpenedContext()) 
    return;

  //aContext->UseDisplayedObjects();
  //myUseExternalObjects = true;

  Handle(AIS_InteractiveObject) anAISIO;
  AIS_ListOfInteractive aPrsList;
  if (theObjList.isEmpty())
    return;
  else {
    foreach(ObjectPtr aObj, theObjList) {
      if (myResult2AISObjectMap.contains(aObj))
        aPrsList.Append(myResult2AISObjectMap[aObj]->impl<Handle(AIS_InteractiveObject)>());
    }
  }

  AIS_ListIteratorOfListOfInteractive aLIt(aPrsList);
  for(aLIt.Initialize(aPrsList); aLIt.More(); aLIt.Next()){
    anAISIO = aLIt.Value();
    activate(anAISIO, myActiveSelectionModes, false);
  }
  if (theUpdateViewer)
    updateViewer();
}

bool XGUI_Displayer::isActive(ObjectPtr theObject) const
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull() || !isVisible(theObject))
    return false;
    
  AISObjectPtr anObj = myResult2AISObjectMap[theObject];
  Handle(AIS_InteractiveObject) anAIS = anObj->impl<Handle(AIS_InteractiveObject)>();

  TColStd_ListOfInteger aModes;
  aContext->ActivatedModes(anAIS, aModes);
  return aModes.Extent() > 0;
}
void XGUI_Displayer::setSelected(const  QList<ModuleBase_ViewerPrs>& theValues, bool theUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return;
  if (aContext->HasOpenedContext()) {
    aContext->UnhilightSelected();
    aContext->ClearSelected();
    foreach (ModuleBase_ViewerPrs aPrs, theValues) {
      const TopoDS_Shape& aShape = aPrs.shape();
      if (!aShape.IsNull()) {
        aContext->AddOrRemoveSelected(aShape, false);
      } else {
        ObjectPtr anObject = aPrs.object();
        ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
        if (aResult.get() && isVisible(aResult)) {
          AISObjectPtr anObj = myResult2AISObjectMap[aResult];
          Handle(AIS_InteractiveObject) anAIS = anObj->impl<Handle(AIS_InteractiveObject)>();
          if (!anAIS.IsNull()) {
            // The methods are replaced in order to provide multi-selection, e.g. restore selection
            // by activating multi selector widget. It also gives an advantage that the multi
            // selection in OB gives multi-selection in the viewer
            //aContext->SetSelected(anAIS, false);
            // The selection in the context was cleared, so the method sets the objects are selected
            aContext->AddOrRemoveSelected(anAIS, false);
          }
        }
      }
    }
  } else {
    aContext->UnhilightCurrents();
    aContext->ClearCurrents();
    foreach (ModuleBase_ViewerPrs aPrs, theValues) {
      ObjectPtr anObject = aPrs.object();
      ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
      if (aResult.get() && isVisible(aResult)) {
        AISObjectPtr anObj = myResult2AISObjectMap[aResult];
        Handle(AIS_InteractiveObject) anAIS = anObj->impl<Handle(AIS_InteractiveObject)>();
        if (!anAIS.IsNull())
          aContext->SetCurrentObject(anAIS, false);
      }
    }
  }
  if (theUpdateViewer)
    updateViewer();
}

void XGUI_Displayer::clearSelected()
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull()) {
    aContext->UnhilightCurrents(false);
    aContext->ClearSelected();
  }
}

bool XGUI_Displayer::eraseAll(const bool theUpdateViewer)
{
  bool aErased = false;
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull()) {
    foreach (ObjectPtr aObj, myResult2AISObjectMap.keys()) {
      AISObjectPtr aAISObj = myResult2AISObjectMap[aObj];
      // erase an object
      Handle(AIS_InteractiveObject) anIO = aAISObj->impl<Handle(AIS_InteractiveObject)>();
      if (!anIO.IsNull()) {
        emit beforeObjectErase(aObj, aAISObj);
        aContext->Remove(anIO, false);
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

void XGUI_Displayer::deactivateTrihedron() const
{
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (!aContext.IsNull()) {
    AIS_ListOfInteractive aList;
    aContext->DisplayedObjects(aList, true);
    AIS_ListIteratorOfListOfInteractive aIt;
    for (aIt.Initialize(aList); aIt.More(); aIt.Next()) {
      Handle(AIS_Trihedron) aTrihedron = Handle(AIS_Trihedron)::DownCast(aIt.Value());
      if (!aTrihedron.IsNull()) {
        aContext->Deactivate(aTrihedron);
      }
    }
  }
}

void XGUI_Displayer::openLocalContext()
{
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  // Open local context if there is no one
  if (!aContext.IsNull() && !aContext->HasOpenedContext()) {
    // Preserve selected objects
    //AIS_ListOfInteractive aAisList;
    //for (aContext->InitCurrent(); aContext->MoreCurrent(); aContext->NextCurrent())
    //  aAisList.Append(aContext->Current());

    // get the filters from the global context and append them to the local context
    // a list of filters in the global context is not cleared and should be cleared here
    SelectMgr_ListOfFilter aFilters;
    aFilters.Assign(aContext->Filters());
    // it is important to remove the filters in the global context, because there is a code
    // in the closeLocalContex, which restore the global context filters
    aContext->RemoveFilters();

    //aContext->ClearCurrents();
    aContext->OpenLocalContext();
    deactivateTrihedron();
    //aContext->NotUseDisplayedObjects();

    //myUseExternalObjects = false;

    SelectMgr_ListIteratorOfListOfFilter aIt(aFilters);
    for (;aIt.More(); aIt.Next()) {
      aContext->AddFilter(aIt.Value());
    }
    // Restore selection
    //AIS_ListIteratorOfListOfInteractive aIt2(aAisList);
    //for(; aIt2.More(); aIt2.Next()) {
    //  aContext->SetSelected(aIt2.Value(), false);
    //}
  }
}

void XGUI_Displayer::closeLocalContexts(const bool theUpdateViewer)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull() && aContext->HasOpenedContext()) {
    // Preserve selected objects
    //AIS_ListOfInteractive aAisList;
    //for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected())
    //  aAisList.Append(aContext->SelectedInteractive());

    // get the filters from the local context and append them to the global context
    // a list of filters in the local context is cleared
    SelectMgr_ListOfFilter aFilters;
    aFilters.Assign(aContext->Filters());

    //aContext->ClearSelected();
    aContext->CloseAllContexts(false);

    // From the moment when the AIS_DS_Displayed flag is used in the Display of AIS object,
    // this code is obsolete. It is temporaty commented and should be removed after
    // the test campaign.
    // Redisplay all object if they were displayed in localContext
    /*Handle(AIS_InteractiveObject) aAISIO;
    foreach (AISObjectPtr aAIS, myResult2AISObjectMap) {
      aAISIO = aAIS->impl<Handle(AIS_InteractiveObject)>();
      if (aContext->DisplayStatus(aAISIO) != AIS_DS_Displayed) {
        aContext->Display(aAISIO, false);
        aContext->SetDisplayMode(aAISIO, Shading, false);
      }
    }*/

    // Append the filters from the local selection in the global selection context
    SelectMgr_ListIteratorOfListOfFilter aIt(aFilters);
    for (;aIt.More(); aIt.Next()) {
      Handle(SelectMgr_Filter) aFilter = aIt.Value();
      aContext->AddFilter(aFilter);
    }

    if (theUpdateViewer)
      updateViewer();
    //myUseExternalObjects = false;

    // Restore selection
    //AIS_ListIteratorOfListOfInteractive aIt2(aAisList);
    //for(; aIt2.More(); aIt2.Next()) {
    //  if (aContext->IsDisplayed(aIt2.Value()))
    //    aContext->SetCurrentObject(aIt2.Value(), false);
    //}
  }
}

AISObjectPtr XGUI_Displayer::getAISObject(ObjectPtr theObject) const
{
  AISObjectPtr anIO;
  if (myResult2AISObjectMap.contains(theObject))
    anIO = myResult2AISObjectMap[theObject];
  return anIO;
}

ObjectPtr XGUI_Displayer::getObject(const AISObjectPtr& theIO) const
{
  Handle(AIS_InteractiveObject) aRefAIS = theIO->impl<Handle(AIS_InteractiveObject)>();
  return getObject(aRefAIS);
}

ObjectPtr XGUI_Displayer::getObject(const Handle(AIS_InteractiveObject)& theIO) const
{
  ObjectPtr anObject;
  foreach (ObjectPtr anObj, myResult2AISObjectMap.keys()) {
    AISObjectPtr aAIS = myResult2AISObjectMap[anObj];
    Handle(AIS_InteractiveObject) anAIS = aAIS->impl<Handle(AIS_InteractiveObject)>();
    if (anAIS == theIO)
      anObject = anObj;
    if (anObject.get())
      break;
  }
  if (!anObject.get()) {
    std::shared_ptr<GeomAPI_AISObject> anAISObj = AISObjectPtr(new GeomAPI_AISObject());
    if (!theIO.IsNull()) {
      anAISObj->setImpl(new Handle(AIS_InteractiveObject)(theIO));
    }
    anObject = myWorkshop->module()->findPresentedObject(anAISObj);
  }
  return anObject;
}

bool XGUI_Displayer::enableUpdateViewer(const bool isEnabled)
{
  bool aWasEnabled = myEnableUpdateViewer;

  myEnableUpdateViewer = isEnabled;

  return aWasEnabled;
}

void XGUI_Displayer::updateViewer() const
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull() && myEnableUpdateViewer) {
    myWorkshop->viewer()->Zfitall();
    aContext->UpdateCurrentViewer();
  }
}

void XGUI_Displayer::activateAIS(const Handle(AIS_InteractiveObject)& theIO,
                                 const int theMode, const bool theUpdateViewer) const
{
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (!aContext.IsNull()) {
    aContext->Activate(theIO, theMode, theUpdateViewer);

#ifdef DEBUG_ACTIVATE_AIS
    ObjectPtr anObject = getObject(theIO);
    anInfo.append(ModuleBase_Tools::objectInfo((*anIt)));
    qDebug(QString("activateAIS: theMode = %1, object = %2").arg(theMode).arg(anInfo).toStdString().c_str());
#endif
  }
}

void XGUI_Displayer::deactivateAIS(const Handle(AIS_InteractiveObject)& theIO, const int theMode) const
{
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (!aContext.IsNull()) {
    if (theMode == -1)
      aContext->Deactivate(theIO);
    else
      aContext->Deactivate(theIO, theMode);

#ifdef DEBUG_DEACTIVATE_AIS
    ObjectPtr anObject = getObject(theIO);
    anInfo.append(ModuleBase_Tools::objectInfo((*anIt)));
    qDebug(QString("deactivateAIS: theMode = %1, object = %2").arg(theMode).arg(anInfo).toStdString().c_str());
#endif
  }
}

Handle(AIS_InteractiveContext) XGUI_Displayer::AISContext() const
{
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (!aContext.IsNull() && !aContext->HasOpenedContext()) {
    aContext->OpenLocalContext();
    deactivateTrihedron();
    aContext->DefaultDrawer()->VIsoAspect()->SetNumber(0);
    aContext->DefaultDrawer()->UIsoAspect()->SetNumber(0);
  }
  return aContext;
}

Handle(SelectMgr_AndFilter) XGUI_Displayer::GetFilter()
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull() && myAndFilter.IsNull()) {
    myAndFilter = new SelectMgr_AndFilter();
    aContext->AddFilter(myAndFilter);
  }
  return myAndFilter;
}

bool XGUI_Displayer::displayAIS(AISObjectPtr theAIS, const bool toActivateInSelectionModes,
                                bool theUpdateViewer)
{
  bool aDisplayed = false;
  Handle(AIS_InteractiveContext) aContext = AISContext();
  Handle(AIS_InteractiveObject) anAISIO = theAIS->impl<Handle(AIS_InteractiveObject)>();
  if (!aContext.IsNull() && !anAISIO.IsNull()) {
    aContext->Display(anAISIO, 0/*wireframe*/, 0, theUpdateViewer, true, AIS_DS_Displayed);
    aDisplayed = true;
    aContext->Deactivate(anAISIO);
    aContext->Load(anAISIO);
    if (toActivateInSelectionModes) {
      if (aContext->HasOpenedContext()) {
        if (myActiveSelectionModes.size() == 0)
          activateAIS(anAISIO, 0, theUpdateViewer);
        else {
          foreach(int aMode, myActiveSelectionModes) {
            activateAIS(anAISIO, aMode, theUpdateViewer);
          }
        }
      }
    }
  }
  return aDisplayed;
}

bool XGUI_Displayer::eraseAIS(AISObjectPtr theAIS, const bool theUpdateViewer)
{
  bool aErased = false;
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull()) {
    Handle(AIS_InteractiveObject) anAISIO = theAIS->impl<Handle(AIS_InteractiveObject)>();
    if (!anAISIO.IsNull() && aContext->IsDisplayed(anAISIO)) {
      aContext->Remove(anAISIO, theUpdateViewer);
      aErased = true;
    }
  }
  return aErased;
}


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

void XGUI_Displayer::deactivateSelectionFilters()
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull() && !myAndFilter.IsNull()) {
    bool aFound = false;
    const SelectMgr_ListOfFilter& aFilters = aContext->Filters();
    SelectMgr_ListIteratorOfListOfFilter anIt(aFilters);
    for (; anIt.More() && !aFound; anIt.Next()) {
      Handle(SelectMgr_Filter) aFilter = anIt.Value();
      aFound = aFilter == myAndFilter;
    }
    if (aFound) {
      aContext->RemoveFilter(myAndFilter);
      myAndFilter.Nullify();
    }
  }
}

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
    qDebug(QString("removeSelectionFilter: filters.count() = %1").arg(aCount).toStdString().c_str());
#endif
  }
}

bool XGUI_Displayer::hasSelectionFilter(const Handle(SelectMgr_Filter)& theFilter)
{
  bool aFilterFound = false;

  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return aFilterFound;
  const SelectMgr_ListOfFilter& aFilters = aContext->Filters();
  SelectMgr_ListIteratorOfListOfFilter aIt(aFilters);
  for (; aIt.More() && !aFilterFound; aIt.Next()) {
    if (theFilter.Access() == aIt.Value().Access())
      aFilterFound = true;
  }
  Handle(SelectMgr_CompositionFilter) aCompositeFilter = GetFilter();
  if (!aCompositeFilter.IsNull()) {
    const SelectMgr_ListOfFilter& aStoredFilters = aCompositeFilter->StoredFilters();
    for (aIt.Initialize(aStoredFilters); aIt.More() && !aFilterFound; aIt.Next()) {
      if (theFilter.Access() == aIt.Value().Access())
        aFilterFound = true;
    }
  }
  return aFilterFound;
}

void XGUI_Displayer::removeFilters()
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return;

  Handle(SelectMgr_CompositionFilter) aCompositeFilter = GetFilter();
  if (!aCompositeFilter.IsNull())
    aCompositeFilter->Clear();
}

void XGUI_Displayer::showOnly(const QObjectPtrList& theList)
{
  QObjectPtrList aDispList = myResult2AISObjectMap.keys();
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

bool XGUI_Displayer::canBeShaded(ObjectPtr theObject) const
{ 
  if (!isVisible(theObject))
    return false;

  AISObjectPtr aAISObj = getAISObject(theObject);
  if (aAISObj.get() == NULL)
    return false;

  Handle(AIS_InteractiveObject) anAIS = aAISObj->impl<Handle(AIS_InteractiveObject)>();
  return ::canBeShaded(anAIS);
}

void XGUI_Displayer::activate(const Handle(AIS_InteractiveObject)& theIO,
                              const QIntList& theModes,
                              const bool theUpdateViewer) const
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull() || theIO.IsNull())
    return;

  // deactivate object in all modes, which are not in the list of activation
  // It seems that after the IO deactivation the selected state of the IO's owners
  // is modified in OCC(version: 6.8.0) and the selection of the object later is lost.
  // By this reason, the number of the IO deactivate is decreased and the object is deactivated
  // only if there is a difference in the current modes and the parameters modes.
  // If the selection problem happens again, it is possible to write a test scenario and create
  // a bug. The bug steps are the following:
  // Create two IO, activate them in 5 modes, select the first IO, deactivate 3 modes for both,
  // with clicked SHIFT select the second object. The result is the selection of the first IO is lost.
  TColStd_ListOfInteger aTColModes;
  aContext->ActivatedModes(theIO, aTColModes);
  TColStd_ListIteratorOfListOfInteger itr( aTColModes );
  QIntList aModesActivatedForIO;
  bool isDeactivated = false;
  for (; itr.More(); itr.Next() ) {
    Standard_Integer aMode = itr.Value();
    if (!theModes.contains(aMode)) {
      deactivateAIS(theIO, aMode);
      isDeactivated = true;
    }
    else {
      aModesActivatedForIO.append(aMode);
    }
  }
  if (isDeactivated) {
    // the selection from the previous activation modes should be cleared manually (#26172)
    aContext->LocalContext()->ClearOutdatedSelection(theIO, true);
    if (theUpdateViewer)
      updateViewer();
  }

  // loading the interactive object allowing the decomposition
  if (aTColModes.IsEmpty()) {
    aContext->Load(theIO, -1, true);
  }

  // trihedron AIS check should be after the AIS loading.
  // If it is not loaded, it is steel selectable in the viewer.
  Handle(AIS_Trihedron) aTrihedron = Handle(AIS_Trihedron)::DownCast(theIO);
  if (aTrihedron.IsNull()) {
      //aContext->Load(anAISIO, -1, true);
      // In order to clear active modes list
    if (theModes.size() == 0) {
      //aContext->Load(anAISIO, 0, true);
      activateAIS(theIO, 0, theUpdateViewer);
    } else {
      foreach(int aMode, theModes) {
        //aContext->Load(anAISIO, aMode, true);
        if (!aModesActivatedForIO.contains(aMode)) {
          activateAIS(theIO, aMode, theUpdateViewer);
        }
      }
    }
  }
}

bool XGUI_Displayer::customizeObject(ObjectPtr theObject)
{
  AISObjectPtr anAISObj = getAISObject(theObject);
  // correct the result's color it it has the attribute
  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);

  // Customization of presentation
  GeomCustomPrsPtr aCustomPrs;
  FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
  if (aFeature.get() != NULL) {
    GeomCustomPrsPtr aCustPrs = std::dynamic_pointer_cast<GeomAPI_ICustomPrs>(aFeature);
    if (aCustPrs.get() != NULL)
      aCustomPrs = aCustPrs;
  }
  if (aCustomPrs.get() == NULL) {
    GeomPresentablePtr aPrs = std::dynamic_pointer_cast<GeomAPI_IPresentable>(theObject);
    // we ignore presentable not customized objects
    if (aPrs.get() == NULL)
      aCustomPrs = myCustomPrs;
  }
  bool isCustomized = aCustomPrs.get() &&
                      aCustomPrs->customisePresentation(aResult, anAISObj, myCustomPrs);
  return isCustomized;
}


QColor XGUI_Displayer::setObjectColor(ObjectPtr theObject, const QColor& theColor, bool theUpdateViewer)
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

void XGUI_Displayer::appendResultObject(ObjectPtr theObject, AISObjectPtr theAIS)
{
  myResult2AISObjectMap[theObject] = theAIS;

#ifdef DEBUG_DISPLAY
  std::ostringstream aPtrStr;
  aPtrStr << theObject.get();
  qDebug(QString("display object: %1").arg(aPtrStr.str().c_str()).toStdString().c_str());
  qDebug(getResult2AISObjectMapInfo().c_str());
#endif
}

std::string XGUI_Displayer::getResult2AISObjectMapInfo() const
{
  QStringList aContent;
  foreach (ObjectPtr aObj, myResult2AISObjectMap.keys()) {
    AISObjectPtr aAISObj = myResult2AISObjectMap[aObj];
    std::ostringstream aPtrStr;
    aPtrStr << "aObj = " << aObj.get() << ":";
    aPtrStr << "anAIS = " << aAISObj.get() << ":";
    aPtrStr << "[" << ModuleBase_Tools::objectInfo(aObj).toStdString().c_str() << "]";
    
    aContent.append(aPtrStr.str().c_str());
  }
  return QString("myResult2AISObjectMap: size = %1\n%2\n").arg(myResult2AISObjectMap.size()).
                                            arg(aContent.join("\n")).toStdString().c_str();
}
