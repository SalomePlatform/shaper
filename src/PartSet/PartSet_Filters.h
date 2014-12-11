// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_Filters.h
// Created:     08 Nov 2014
// Author:      Vitaly SMETANNIKOV

#ifndef PartSet_Filters_H
#define PartSet_Filters_H

#include <ModuleBase_ViewerFilters.h>


/**
* A class which filters groups object in addition to documents (see issue #310)
*/
DEFINE_STANDARD_HANDLE(PartSet_GlobalFilter, ModuleBase_ShapeDocumentFilter);
class PartSet_GlobalFilter: public ModuleBase_ShapeDocumentFilter
{
public:
  PartSet_GlobalFilter(ModuleBase_IWorkshop* theWorkshop)
    : ModuleBase_ShapeDocumentFilter(theWorkshop) {}

  Standard_EXPORT virtual Standard_Boolean IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const;

  DEFINE_STANDARD_RTTI(PartSet_GlobalFilter)
};

#endif