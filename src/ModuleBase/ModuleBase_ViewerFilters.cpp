// File:        ModuleBase_ViewerFilters.cpp
// Created:     07 Okt 2014
// Author:      Vitaly SMETANNIKOV


#include "ModuleBase_ViewerFilters.h"
#include "ModuleBase_IWorkshop.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>

#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>

#include <StdSelect_BRepOwner.hxx>

#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <Geom_Curve.hxx>

IMPLEMENT_STANDARD_HANDLE(ModuleBase_ShapeDocumentFilter, SelectMgr_Filter);
IMPLEMENT_STANDARD_RTTIEXT(ModuleBase_ShapeDocumentFilter, SelectMgr_Filter);


//TODO (VSV): Check bug in OCCT: Filter result is ignored (bug25340)
Standard_Boolean ModuleBase_ShapeDocumentFilter::IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  if (theOwner->HasSelectable()) {
    Handle(AIS_InteractiveObject) aAisObj = 
      Handle(AIS_InteractiveObject)::DownCast(theOwner->Selectable());
    if (!aAisObj.IsNull()) {
      boost::shared_ptr<GeomAPI_AISObject> aAISObj = AISObjectPtr(new GeomAPI_AISObject());
      aAISObj->setImpl(new Handle(AIS_InteractiveObject)(aAisObj));
      ObjectPtr aObj = myWorkshop->findPresentedObject(aAISObj);
      if (aObj) {
        DocumentPtr aDoc = aObj->document();
        SessionPtr aMgr = ModelAPI_Session::get();
        return (aDoc == aMgr->activeDocument()) || (aDoc == aMgr->moduleDocument());
      }
    }
  }
  return Standard_False;
}


IMPLEMENT_STANDARD_HANDLE(ModuleBase_ShapeInPlaneFilter, SelectMgr_Filter);
IMPLEMENT_STANDARD_RTTIEXT(ModuleBase_ShapeInPlaneFilter, SelectMgr_Filter);

Standard_Boolean ModuleBase_ShapeInPlaneFilter::IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  if (theOwner->HasSelectable()) {
    Handle(StdSelect_BRepOwner) aShapeOwner = Handle(StdSelect_BRepOwner)::DownCast(theOwner);
    if (!aShapeOwner.IsNull()) {
      TopoDS_Shape aShape = aShapeOwner->Shape();
      TopAbs_ShapeEnum aType = aShape.ShapeType();
      switch (aType) {
      case TopAbs_VERTEX:
        {
          gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(aShape));
          return myPlane.Distance(aPnt) < Precision::Confusion();
        }
      case TopAbs_EDGE:
        {
          TopoDS_Edge aEdge = TopoDS::Edge(aShape);
          Standard_Real aFirst, aLast;
          Handle(Geom_Curve) aCurve = BRep_Tool::Curve(aEdge, aFirst, aLast);
          gp_Pnt aFirstPnt = aCurve->Value(aFirst);
          gp_Pnt aMidPnt = aCurve->Value((aFirst + aLast) / 2.);
          gp_Pnt aLastPnt = aCurve->Value(aLast);
          bool aD1 = myPlane.Distance(aFirstPnt) < Precision::Confusion();
          bool aD2 = myPlane.Distance(aMidPnt) < Precision::Confusion();
          bool aD3 = myPlane.Distance(aLastPnt) < Precision::Confusion();
          return aD1 && aD2 && aD3;
        }
      }
    }
  }
  return Standard_False;
}
