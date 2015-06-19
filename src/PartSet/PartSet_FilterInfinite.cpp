// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_Filters.cpp
// Created:     19 Jun 2015
// Author:      Natalia ERMOLAEVA

#include "PartSet_FilterInfinite.h"

#include <AIS_InteractiveObject.hxx>

IMPLEMENT_STANDARD_HANDLE(PartSet_FilterInfinite, SelectMgr_Filter);
IMPLEMENT_STANDARD_RTTIEXT(PartSet_FilterInfinite, SelectMgr_Filter);

Standard_Boolean PartSet_FilterInfinite::IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  Handle(AIS_InteractiveObject) anAISObj =
    Handle(AIS_InteractiveObject)::DownCast(theOwner->Selectable());
  if (!anAISObj.IsNull() && anAISObj->IsInfinite()) {
    return Standard_False;
  }
  return Standard_True;
}
