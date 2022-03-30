// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include "XGUI_Selection.h"
#include "XGUI_Workshop.h"
#include "XGUI_Displayer.h"
#include "XGUI_ViewerProxy.h"
#include "XGUI_ObjectsBrowser.h"

#ifndef HAVE_SALOME
#include <AIS_ViewCube.hxx>
#endif

#include "ModuleBase_BRepOwner.h"
#include "ModuleBase_ResultPrs.h"
#include "ModuleBase_ViewerPrs.h"

#include <ModelAPI_Feature.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>

#include <AIS_InteractiveContext.hxx>
#ifdef BEFORE_TRIHEDRON_PATCH
#include <AIS_Axis.hxx>
#include <AIS_Point.hxx>
#else
#include <AIS_TrihedronOwner.hxx>
#endif
#include <Geom_Line.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Edge.hxx>
#include <Geom_Point.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Prs3d_DatumAspect.hxx>

#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <StdSelect_BRepOwner.hxx>

#include <set>

#ifdef WIN32
#pragma warning(disable : 4456) // for nested foreach
#endif

#define DEBUG_DELIVERY

XGUI_Selection::XGUI_Selection(XGUI_Workshop* theWorkshop)
    : myWorkshop(theWorkshop)
{
}

QList<ModuleBase_ViewerPrsPtr> XGUI_Selection::getSelected(const SelectionPlace& thePlace) const
{
  QList<ModuleBase_ViewerPrsPtr> aPresentations;
  QList<ModuleBase_ViewerPrsPtr> aToRemove;

  switch (thePlace) {
    case Browser:
      getSelectedInBrowser(aPresentations);
    break;
    case Viewer:
      getSelectedInViewer(aPresentations);
    break;
  case AllControls:
    // Get selection from object browser
    getSelectedInBrowser(aPresentations);

    // Filter out all objects except feature if there is no selected results in object browser
    // Filter out all features if in object browser there are selected features and their results
    bool aHasFeature = false;
    bool aHasResult = false;
    foreach(ModuleBase_ViewerPrsPtr aVal, aPresentations) {
      if (aVal->object().get()) {
        FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aVal->object());
        if (aFeature.get()) {
          aHasFeature = true;
          std::list<ResultPtr> aResList = aFeature->results();
          std::list<ResultPtr>::const_iterator aIt;
          for (aIt = aResList.cbegin(); aIt != aResList.cend(); aIt++) {
            foreach(ModuleBase_ViewerPrsPtr aSel, aPresentations) {
              if (aSel->object() == (*aIt)) {
                aHasResult = true;
                break;
              }
            }
            if (aHasResult)
              break;
          }
        }
      }
      if (aHasFeature && aHasResult)
        break;
    }
    //Get selection from a viewer
    getSelectedInViewer(aPresentations);

    // Filter out extra objects
    if (aHasFeature && aHasResult) {
      foreach(ModuleBase_ViewerPrsPtr aVal, aPresentations) {
        if (aVal->object().get()) {
          FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aVal->object());
          if (aFeature.get()) {
            aToRemove.append(aVal);
          }
        }
      }
    }
    else if (aHasFeature && (!aHasResult)) {
      foreach(ModuleBase_ViewerPrsPtr aVal, aPresentations) {
        if (aVal->object().get()) {
          FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aVal->object());
          if (!aFeature.get()) {
            aToRemove.append(aVal);
          }
        }
      }
    }
    foreach(ModuleBase_ViewerPrsPtr aVal, aToRemove) {
      aPresentations.removeAll(aVal);
    }
    break;
  }
  return aPresentations;
}

Handle(AIS_InteractiveObject) XGUI_Selection::getIO(const ModuleBase_ViewerPrsPtr& thePrs)
{
  Handle(AIS_InteractiveObject) anIO = thePrs->interactive();
  if (anIO.IsNull()) {
    Handle(SelectMgr_EntityOwner) anOwner = thePrs->owner();
    if (!anOwner.IsNull())
      anIO = Handle(AIS_InteractiveObject)::DownCast(anOwner->Selectable());

    if (anIO.IsNull() && thePrs->object()) {
      XGUI_Displayer* aDisplayer = myWorkshop->displayer();
      AISObjectPtr anAISObject = aDisplayer->getAISObject(thePrs->object());
      if (anAISObject.get())
        anIO = anAISObject->impl<Handle(AIS_InteractiveObject)>();
    }
  }
  return anIO;
}

void XGUI_Selection::getSelectedInViewer(QList<ModuleBase_ViewerPrsPtr>& thePresentations) const
{
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (!aContext.IsNull()) {
    QList<size_t> aSelectedIds; // Remember of selected address in order to avoid duplicates
    for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected()) {
      ModuleBase_ViewerPrsPtr aPrs(new ModuleBase_ViewerPrs());
      Handle(SelectMgr_EntityOwner) anOwner = aContext->SelectedOwner();

      if (aSelectedIds.contains((size_t)anOwner.get()))
        continue;
      aSelectedIds.append((size_t)anOwner.get());

      fillPresentation(aPrs, anOwner);

      if (!thePresentations.contains(aPrs)) // TODO: check whether the presentation in a list
        thePresentations.append(aPrs);
    }
  }
}

void XGUI_Selection::getSelectedInBrowser(QList<ModuleBase_ViewerPrsPtr>& thePresentations) const
{
  // collect the objects  of the parameter presentation to avoid a repeted objects in the result
  QObjectPtrList aPresentationObjects;
  QList<ModuleBase_ViewerPrsPtr>::const_iterator aPrsIt = thePresentations.begin(),
                                              aPrsLast = thePresentations.end();
  for (; aPrsIt != aPrsLast; aPrsIt++) {
    aPresentationObjects.push_back((*aPrsIt)->object());
  }

  QObjectPtrList anObjects = selectedObjects();
  QObjectPtrList::const_iterator anIt = anObjects.begin(), aLast = anObjects.end();
  for (; anIt != aLast; anIt++) {
    ObjectPtr anObject = *anIt;
    if (anObject.get() != NULL && !aPresentationObjects.contains(anObject)) {
      thePresentations.append(std::shared_ptr<ModuleBase_ViewerPrs>(
               new ModuleBase_ViewerPrs(anObject, GeomShapePtr(), NULL)));
    }
  }
}

void XGUI_Selection::fillPresentation(ModuleBase_ViewerPrsPtr& thePrs,
                                      const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  Handle(AIS_InteractiveObject) anIO =
                           Handle(AIS_InteractiveObject)::DownCast(theOwner->Selectable());
#ifndef HAVE_SALOME
  Handle(AIS_ViewCube) aCube = Handle(AIS_ViewCube)::DownCast(anIO);
  if (!aCube.IsNull())
    return;
#endif

  thePrs->setOwner(theOwner);
  thePrs->setInteractive(anIO);

  // we should not check the appearance of this feature because there can be some selected shapes
  // for one feature
  Handle(StdSelect_BRepOwner) aBRO = Handle(StdSelect_BRepOwner)::DownCast(theOwner);
  if( !aBRO.IsNull() && aBRO->HasShape() ) {
    TopoDS_Shape aShape = aBRO->Shape();
    Handle(ModuleBase_ResultPrs) aPrsObj =
      Handle(ModuleBase_ResultPrs)::DownCast(aBRO->Selectable());
    if (!aPrsObj.IsNull()) {
      if (aPrsObj->isSubstituted()) {
        if (aPrsObj->Shape().IsSame(aShape))
          aShape = aPrsObj->originalShape();
      }
    }
    // the located method is called in the context to obtain the shape by the SelectedShape()
    // method, so the shape is located by the same rules
    aShape = aShape.Located(aBRO->Location() * aShape.Location());
#ifdef BEFORE_TRIHEDRON_PATCH
#ifndef DEBUG_DELIVERY
    if (aShape.IsNull())
      aShape = findAxisShape(anIO);
#endif
#endif
    if (!aShape.IsNull()) {
      std::shared_ptr<GeomAPI_Shape> aGeomShape =
        std::shared_ptr<GeomAPI_Shape>(new GeomAPI_Shape());
      aGeomShape->setImpl(new TopoDS_Shape(aShape));
      thePrs->setShape(aGeomShape);
    }
  } else {
#ifdef DEBUG_DELIVERY
    // Fill by trihedron shapes
#ifdef BEFORE_TRIHEDRON_PATCH
    Handle(AIS_Axis) aAxis = Handle(AIS_Axis)::DownCast(anIO);
    DocumentPtr aDoc = ModelAPI_Session::get()->moduleDocument();
    int aSize = aDoc->size(ModelAPI_ResultConstruction::group());
    ObjectPtr aObj;
    if (!aAxis.IsNull()) {
      // an Axis from Trihedron
      gp_Lin aLine = aAxis->Component()->Lin();
      gp_Dir aDir = aLine.Direction();
      std::string aAxName;
      if (aDir.X() == 1.)
        aAxName = "OX";
      else if (aDir.Y() == 1.)
        aAxName = "OY";
      else if (aDir.Z() == 1.)
        aAxName = "OZ";
      if (aAxName.length() > 0) {
        ResultPtr aAx;
        for (int i = 0; i < aSize; i++) {
          aObj = aDoc->object(ModelAPI_ResultConstruction::group(), i);
          if (aObj->data()->name() == aAxName) {
            aAx = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aObj);
            break;
          }
        }
        if (aAx.get()) {
          thePrs->setObject(aAx);
          thePrs->setShape(aAx->shape());
          return;
        }
      }
    } else {
      Handle(AIS_Point) aPoint = Handle(AIS_Point)::DownCast(anIO);
      if (!aPoint.IsNull()) {
        // An origin point from trihedron
        ResultPtr aOrigin;
        for (int i = 0; i < aSize; i++) {
          aObj = aDoc->object(ModelAPI_ResultConstruction::group(), i);
          if (aObj->data()->name() == "Origin") {
            aOrigin = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aObj);
            break;
          }
        }
        if (aOrigin.get()) {
          thePrs->setObject(aOrigin);
          thePrs->setShape(aOrigin->shape());
          return;
        }
      }
    }
#else
    /// find axis direction
    Handle(AIS_TrihedronOwner) aTrihedronOwner = Handle(AIS_TrihedronOwner)::DownCast(theOwner);
    if (!aTrihedronOwner.IsNull()) {
      const Prs3d_DatumParts& aPart = aTrihedronOwner->DatumPart();
      std::wstring aName;
      switch (aPart) {
        case Prs3d_DP_Origin: aName = L"Origin"; break;
        case Prs3d_DP_XAxis: aName = L"OX"; break;
        case Prs3d_DP_YAxis: aName = L"OY"; break;
        case Prs3d_DP_ZAxis: aName = L"OZ"; break;
        default: break;
      }
      if (aName.length() > 0) {
        DocumentPtr aDoc = ModelAPI_Session::get()->moduleDocument();
        int aSize = aDoc->size(ModelAPI_ResultConstruction::group());
        ObjectPtr aObj;
        ResultPtr aResult;
        for (int i = 0; i < aSize; i++) {
          aObj = aDoc->object(ModelAPI_ResultConstruction::group(), i);
          if (aObj->data()->name() == aName) {
            aResult = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aObj);
            break;
          }
        }
        if (aResult.get()) {
          thePrs->setObject(aResult);
          thePrs->setShape(aResult->shape());
          return;
        }
      }
    }
#endif
#endif
  }

  XGUI_Displayer* aDisplayer = myWorkshop->displayer();
  ObjectPtr aFeature = aDisplayer->getObject(anIO);

  Handle(ModuleBase_BRepOwner) aCompSolidBRO = Handle(ModuleBase_BRepOwner)::DownCast(theOwner);
  if (!aCompSolidBRO.IsNull()) {
    // If ModuleBase_BRepOwner object is created then it means that TopAbs_COMPSOLID selection mode
    // is On and we have to use parent result which corresponds to the CompSolid shape
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(aFeature);
    if (aResult.get()) {
      ResultBodyPtr aCompSolid = ModelAPI_Tools::bodyOwner(aResult);
      if (aCompSolid.get()) {
        GeomShapePtr aShape = aCompSolid->shape();
        if (aShape.get() && aShape->isEqual(thePrs->shape())) {
          thePrs->setObject(aCompSolid);
          return;
        }
      }
    }
  }
  thePrs->setObject(aFeature);
}

QList<ModuleBase_ViewerPrsPtr> XGUI_Selection::getHighlighted() const
{
  QList<ModuleBase_ViewerPrsPtr> aPresentations;
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (aContext.IsNull())
    return aPresentations;

  QList<size_t> aSelectedIds; // Remember of selected address in order to avoid duplicates
  for (aContext->InitDetected(); aContext->MoreDetected(); aContext->NextDetected()) {
    Handle(SelectMgr_EntityOwner) anOwner = aContext->DetectedOwner();
    if (!anOwner.IsNull()) {
      if (aSelectedIds.contains((size_t)anOwner.get()))
        continue;
      aSelectedIds.append((size_t)anOwner.get());

      ModuleBase_ViewerPrsPtr aPrs(new ModuleBase_ViewerPrs());
      fillPresentation(aPrs, anOwner);
      aPresentations.push_back(aPrs);
    }
  }
  return aPresentations;
}

QObjectPtrList XGUI_Selection::selectedObjects() const
{
  if (myWorkshop->objectBrowser())
    return myWorkshop->objectBrowser()->selectedObjects();
  return QObjectPtrList();
}

QObjectPtrList XGUI_Selection::selectedPresentations() const
{
  QObjectPtrList aSelectedList;

  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (!aContext.IsNull()) {
    for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected()) {
      Handle(AIS_InteractiveObject) anIO = aContext->SelectedInteractive();
      ObjectPtr aResult = myWorkshop->displayer()->getObject(anIO);
      if (aResult)
        aSelectedList.append(aResult);
    }
  }
  return aSelectedList;
}

//**************************************************************
QModelIndexList XGUI_Selection::selectedIndexes() const
{
  return myWorkshop->objectBrowser()->selectedIndexes();
}

//**************************************************************
ObjectPtr XGUI_Selection::getSelectableObject(const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  ObjectPtr anObject;

  Handle(SelectMgr_EntityOwner) aEO = theOwner;
  if (!aEO.IsNull()) {
    Handle(AIS_InteractiveObject) anObj =
      Handle(AIS_InteractiveObject)::DownCast(aEO->Selectable());
    anObject = myWorkshop->displayer()->getObject(anObj);
  }
  return anObject;
}

//**************************************************************
void XGUI_Selection::selectedOwners(SelectMgr_IndexedMapOfOwner& theSelectedOwners) const
{
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (!aContext.IsNull()) {
    for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected()) {
      theSelectedOwners.Add(aContext->SelectedOwner());
    }
  }
}

//**************************************************************
void XGUI_Selection::entityOwners(const Handle(AIS_InteractiveObject)& theObject,
                                  SelectMgr_IndexedMapOfOwner& theOwners) const
{
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (aContext.IsNull() || theObject.IsNull())
    return;

  TColStd_ListOfInteger aModes;
  aContext->ActivatedModes(theObject, aModes);

  TColStd_ListIteratorOfListOfInteger anIt(aModes);
  for (; anIt.More(); anIt.Next()) {
    int aMode = anIt.Value();
    if (!theObject->HasSelection(aMode))
      continue;

    Handle(SelectMgr_Selection) aSelection = theObject->Selection(aMode);
    NCollection_Vector<Handle(SelectMgr_SensitiveEntity)> anEntities = aSelection->Entities();
    for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator anEntIt(anEntities);
         anEntIt.More(); anEntIt.Next()) {
      Handle(SelectMgr_SensitiveEntity) anEntity = anEntIt.Value();
      if (anEntity.IsNull())
        continue;
      Handle(SelectMgr_EntityOwner) anOwner = anEntity->BaseSensitive()->OwnerId();
      if (!anOwner.IsNull())
        theOwners.Add(anOwner);
    }
  }
}

//**************************************************************
#ifdef BEFORE_TRIHEDRON_PATCH
TopoDS_Shape XGUI_Selection::findAxisShape(Handle(AIS_InteractiveObject) theIO) const
{
  TopoDS_Shape aShape;
  // Fill by trihedron shapes
  Handle(AIS_Axis) aAxis = Handle(AIS_Axis)::DownCast(theIO);
  if (!aAxis.IsNull()) {
    // an Axis from Trihedron
    Handle(Geom_Line) aLine = aAxis->Component();
    Handle(Prs3d_DatumAspect) DA = aAxis->Attributes()->DatumAspect();
    Handle(Geom_TrimmedCurve) aTLine = new Geom_TrimmedCurve(aLine, 0, DA->FirstAxisLength());

    BRep_Builder aBuilder;
    TopoDS_Edge aEdge;
    aBuilder.MakeEdge(aEdge, aTLine, Precision::Confusion());
    if (!aEdge.IsNull())
      aShape = aEdge;
  } else {
    Handle(AIS_Point) aPoint = Handle(AIS_Point)::DownCast(theIO);
    if (!aPoint.IsNull()) {
      // A point from trihedron
      Handle(Geom_Point) aPnt = aPoint->Component();
      BRep_Builder aBuilder;
      TopoDS_Vertex aVertex;
      aBuilder.MakeVertex(aVertex, aPnt->Pnt(), Precision::Confusion());
      if (!aVertex.IsNull())
        aShape = aVertex;
    }
  }
  return aShape;
}
#endif
