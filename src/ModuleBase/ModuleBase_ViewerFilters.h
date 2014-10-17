// File:        ModuleBase_ViewerFilters.h
// Created:     07 Okt 2014
// Author:      Vitaly SMETANNIKOV


#ifndef ModuleBase_ViewerFilters_H
#define ModuleBase_ViewerFilters_H

#include <SelectMgr_Filter.hxx>
#include <SelectMgr_EntityOwner.hxx>


class ModuleBase_IWorkshop;

DEFINE_STANDARD_HANDLE(ModuleBase_ShapeDocumentFilter, SelectMgr_Filter);

class ModuleBase_ShapeDocumentFilter: public SelectMgr_Filter
{
public:
  Standard_EXPORT ModuleBase_ShapeDocumentFilter(ModuleBase_IWorkshop* theWorkshop): SelectMgr_Filter(),
    myWorkshop(theWorkshop) {}

  Standard_EXPORT virtual Standard_Boolean IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const;

  DEFINE_STANDARD_RTTI(ModuleBase_ShapeDocumentFilter)

private:
  ModuleBase_IWorkshop* myWorkshop;
};

#endif