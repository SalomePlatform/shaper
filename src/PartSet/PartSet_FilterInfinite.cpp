// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_Filters.cpp
// Created:     19 Jun 2015
// Author:      Natalia ERMOLAEVA

#include "PartSet_FilterInfinite.h"

#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>
#include <StdSelect_BRepOwner.hxx>

IMPLEMENT_STANDARD_HANDLE(PartSet_FilterInfinite, SelectMgr_Filter);
IMPLEMENT_STANDARD_RTTIEXT(PartSet_FilterInfinite, SelectMgr_Filter);

Standard_Boolean PartSet_FilterInfinite::IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  Standard_Boolean aValid = Standard_True;
  Handle(AIS_InteractiveObject) anAISObj =
    Handle(AIS_InteractiveObject)::DownCast(theOwner->Selectable());
  if (!anAISObj.IsNull()) {
    Handle(AIS_InteractiveObject) anObj = 
        Handle(AIS_InteractiveObject)::DownCast(theOwner->Selectable());
    Handle(AIS_Shape) aAISShape = Handle(AIS_Shape)::DownCast(anObj);
    if (!aAISShape.IsNull()) {
      TopoDS_Shape anAISShape = aAISShape->Shape();
      if (!anAISShape.IsNull() && anAISShape.Infinite()) {
        aValid = Standard_False;
        // for infinite object, the selection is possible only for shapes of owners, which are coincide
        // to the shape of corresponded AIS object. In other words, for axis, only edge can be selected
        // (vertices are not selectable), for planes, only faces can be selected (not edges or vertices)
        TopoDS_Shape anOwnerShape;
        Handle(StdSelect_BRepOwner) aBRO = Handle(StdSelect_BRepOwner)::DownCast(theOwner);
        if( !aBRO.IsNull() ) {
          anOwnerShape = aBRO->Shape();
          if (!anOwnerShape.IsNull()) {
            aValid = anAISShape.ShapeType() == anOwnerShape.ShapeType();
          }
        }
      }
    }
  }
  return aValid;
}
