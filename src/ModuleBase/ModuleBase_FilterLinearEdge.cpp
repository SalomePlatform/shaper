// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_FilterLinearEdge.cpp
// Created:     10 Dec 2014
// Author:      Natalia ERMOLAEVA


#include "ModuleBase_FilterLinearEdge.h"
#include "ModuleBase_IWorkshop.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_ResultConstruction.h>

#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>

#include <StdSelect_BRepOwner.hxx>

#include <GeomAPI_Edge.h>

#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <Geom_Curve.hxx>


IMPLEMENT_STANDARD_HANDLE(ModuleBase_FilterLinearEdge, SelectMgr_Filter);
IMPLEMENT_STANDARD_RTTIEXT(ModuleBase_FilterLinearEdge, SelectMgr_Filter);


Standard_Boolean ModuleBase_FilterLinearEdge::IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  Standard_Boolean isOk = ModuleBase_Filter::IsOk(theOwner);
  if (isOk && theOwner->HasSelectable()) {
    Handle(AIS_InteractiveObject) anAIS = 
      Handle(AIS_InteractiveObject)::DownCast(theOwner->Selectable());
    if (!anAIS.IsNull()) {
      Handle(AIS_Shape) aShapeAIS = Handle(AIS_Shape)::DownCast(anAIS);
      if (aShapeAIS) {
        const TopoDS_Shape& aShape = aShapeAIS->Shape();
        if (aShape.ShapeType()  == TopAbs_EDGE) {
          std::shared_ptr<GeomAPI_Edge> anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge());
          anEdge->setImpl(new TopoDS_Shape(aShape));

          isOk = anEdge->isLine();
        }
      }
    }
  }
  return Standard_False;
}
