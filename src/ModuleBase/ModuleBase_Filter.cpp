// File:        ModuleBase_Filter.cpp
// Created:     10 Dec 2014
// Author:      Natalia ERMOLAEVA


#include "ModuleBase_Filter.h"

IMPLEMENT_STANDARD_HANDLE(ModuleBase_Filter, SelectMgr_Filter);
IMPLEMENT_STANDARD_RTTIEXT(ModuleBase_Filter, SelectMgr_Filter);

Standard_Boolean ModuleBase_Filter::IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  return Standard_True;
}
