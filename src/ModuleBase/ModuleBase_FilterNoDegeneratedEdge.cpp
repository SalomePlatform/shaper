// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_ViewerFilters.cpp
// Created:     10 Feb 2015
// Author:      Natalia ERMOLAEVA


#include "ModuleBase_FilterNoDegeneratedEdge.h"

#include <StdSelect_BRepOwner.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <BRep_Tool.hxx>

IMPLEMENT_STANDARD_HANDLE(ModuleBase_FilterNoDegeneratedEdge, StdSelect_EdgeFilter);
IMPLEMENT_STANDARD_RTTIEXT(ModuleBase_FilterNoDegeneratedEdge, StdSelect_EdgeFilter);

ModuleBase_FilterNoDegeneratedEdge::ModuleBase_FilterNoDegeneratedEdge()
: StdSelect_EdgeFilter(StdSelect_AnyEdge)
{
}

Standard_Boolean ModuleBase_FilterNoDegeneratedEdge::IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  bool isEdge = StdSelect_EdgeFilter::IsOk(theOwner);
  if (isEdge) {
    Handle(StdSelect_BRepOwner) anOwner = Handle(StdSelect_BRepOwner)::DownCast(theOwner);
    if (!anOwner.IsNull() && anOwner->HasShape()) {
      const TopoDS_Shape& aShape = anOwner->Shape();
      if (BRep_Tool::Degenerated(TopoDS::Edge(aShape)))
        return Standard_False;
    }
  }
  return isEdge;
}

