// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_FilterLinearEdge.h
// Created:     10 Dec 2014
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_FilterLinearEdge_H
#define ModuleBase_FilterLinearEdge_H

#include "ModuleBase.h"

#include "ModuleBase_Filter.h"

/**
* A filter which provides filtering of selection in 3d viewer.
* Installing of this filter lets to select only object of requested type
*/
DEFINE_STANDARD_HANDLE(ModuleBase_FilterLinearEdge, ModuleBase_Filter);
class ModuleBase_FilterLinearEdge: public ModuleBase_Filter
{
public:
  Standard_EXPORT ModuleBase_FilterLinearEdge(): 
      ModuleBase_Filter() {}

  Standard_EXPORT virtual Standard_Boolean IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const;

  DEFINE_STANDARD_RTTI(ModuleBase_FilterLinearEdge)

};

#endif //ModuleBase_FilterLinearEdge
