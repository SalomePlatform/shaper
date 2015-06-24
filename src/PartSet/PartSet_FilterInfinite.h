// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_Filters.h
// Created:     19 Jun 2015
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_FilterInfinite_H
#define PartSet_FilterInfinite_H

#include <ModuleBase_ViewerFilters.h>

#include <SelectMgr_Filter.hxx>

class ModuleBase_IWorkshop;

/**
* \class PartSet_FilterInfinite
* \ingroup Modules
* A class which filters AIS objects with the true infinite state
*/
DEFINE_STANDARD_HANDLE(PartSet_FilterInfinite, SelectMgr_Filter);
class PartSet_FilterInfinite : public SelectMgr_Filter
{
public:
  /// Constructor
  /// \param theWorkshop a pointer to workshop
  PartSet_FilterInfinite(ModuleBase_IWorkshop* theWorkshop);

  /// Returns True if selected presentation can be selected
  /// \param theOwner an owner of the persentation
  Standard_EXPORT virtual Standard_Boolean IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const;

  DEFINE_STANDARD_RTTI(PartSet_FilterInfinite)

protected:
  /// Reference to workshop
  ModuleBase_IWorkshop* myWorkshop;
};

#endif