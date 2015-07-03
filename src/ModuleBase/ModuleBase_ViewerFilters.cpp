// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_ViewerFilters.cpp
// Created:     07 Okt 2014
// Author:      Vitaly SMETANNIKOV


#include "ModuleBase_ViewerFilters.h"
#include "ModuleBase_IWorkshop.h"
#include "ModuleBase_IModule.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_ResultConstruction.h>

#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>

#include <StdSelect_BRepOwner.hxx>

#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <Geom_Curve.hxx>
#include <gp_Pln.hxx>

IMPLEMENT_STANDARD_HANDLE(ModuleBase_ShapeDocumentFilter, SelectMgr_Filter);
IMPLEMENT_STANDARD_RTTIEXT(ModuleBase_ShapeDocumentFilter, SelectMgr_Filter);


//TODO (VSV): Check bug in OCCT: Filter result is ignored (bug25340)
Standard_Boolean ModuleBase_ShapeDocumentFilter::IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  ModuleBase_Operation* anOperation = myWorkshop->module()->currentOperation();
  // the shapes from different documents should be provided if there is no started operation
  // in order to show/hide results
  if (!anOperation)
    return true;

  if (theOwner->HasSelectable()) {
    Handle(AIS_InteractiveObject) aAisObj = 
      Handle(AIS_InteractiveObject)::DownCast(theOwner->Selectable());
    if (!aAisObj.IsNull()) {
      std::shared_ptr<GeomAPI_AISObject> aAISObj = AISObjectPtr(new GeomAPI_AISObject());
      aAISObj->setImpl(new Handle(AIS_InteractiveObject)(aAisObj));
      ObjectPtr aObj = myWorkshop->findPresentedObject(aAISObj);
      if (aObj) {
        DocumentPtr aDoc = aObj->document();
        SessionPtr aMgr = ModelAPI_Session::get();
        return (aDoc == aMgr->activeDocument() || aDoc == aMgr->moduleDocument());
      }
      else {
        // This is not object controlled by the filter
        return Standard_True;
      }
    }
  }
  return Standard_False;
}

IMPLEMENT_STANDARD_HANDLE(ModuleBase_ShapeInPlaneFilter, SelectMgr_Filter);
IMPLEMENT_STANDARD_RTTIEXT(ModuleBase_ShapeInPlaneFilter, SelectMgr_Filter);

Standard_Boolean ModuleBase_ShapeInPlaneFilter::IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  if (!myPlane.get())
    return Standard_True;

  if (theOwner->HasSelectable()) {
    Handle(StdSelect_BRepOwner) aShapeOwner = Handle(StdSelect_BRepOwner)::DownCast(theOwner);
    if (!aShapeOwner.IsNull()) {
      TopoDS_Shape aShape = aShapeOwner->Shape();
      TopAbs_ShapeEnum aType = aShape.ShapeType();
      gp_Pln aPlane = myPlane->impl<gp_Pln>();
      switch (aType) {
      case TopAbs_VERTEX:
        {
          gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(aShape));
          return aPlane.Distance(aPnt) < Precision::Confusion();
        }
      case TopAbs_EDGE:
        {
          TopoDS_Edge aEdge = TopoDS::Edge(aShape);
          Standard_Real aFirst, aLast;
          Handle(Geom_Curve) aCurve = BRep_Tool::Curve(aEdge, aFirst, aLast);
          gp_Pnt aFirstPnt = aCurve->Value(aFirst);
          gp_Pnt aMidPnt = aCurve->Value((aFirst + aLast) / 2.);
          gp_Pnt aLastPnt = aCurve->Value(aLast);
          bool aD1 = aPlane.Distance(aFirstPnt) < Precision::Confusion();
          bool aD2 = aPlane.Distance(aMidPnt) < Precision::Confusion();
          bool aD3 = aPlane.Distance(aLastPnt) < Precision::Confusion();
          return aD1 && aD2 && aD3;
        }
      default:
        // The object can be selected in Object browser and contain, for example, compound.
        // The compound could not belong to any plane, so the result is false
        return Standard_False;
      break;
      }
    } else {
      // This is not object controlled by the filter
      return Standard_True;
    }
  }
  return Standard_False;
}
