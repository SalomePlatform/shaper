// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_Filters.h
// Created:     08 Nov 2014
// Author:      Vitaly SMETANNIKOV

#ifndef PartSet_Filters_H
#define PartSet_Filters_H

#include <ModuleBase_ViewerFilters.h>


/**
* \class PartSet_GlobalFilter
* \ingroup Modules
* A class which filters groups object in addition to documents (see issue #310)
*/
DEFINE_STANDARD_HANDLE(PartSet_GlobalFilter, ModuleBase_ShapeDocumentFilter);
class PartSet_GlobalFilter: public ModuleBase_ShapeDocumentFilter
{
public:
  /// Constructor
  /// \param theWorkshop a pointer to workshop
  PartSet_GlobalFilter(ModuleBase_IWorkshop* theWorkshop)
    : ModuleBase_ShapeDocumentFilter(theWorkshop) {}

  /// Returns True if selected presentation can be selected
  /// \param theOwner an owner of the persentation
  Standard_EXPORT virtual Standard_Boolean IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const;

  DEFINE_STANDARD_RTTI(PartSet_GlobalFilter)
};


/// \class PartSet_CirclePointFilter
/// \ingroup GUI
/// \brief A filter which provide filtering of selection in 3d viewer.
/// Installing of this filter disables selection of point on circle in sketch
DEFINE_STANDARD_HANDLE(PartSet_CirclePointFilter, SelectMgr_Filter);
class PartSet_CirclePointFilter: public SelectMgr_Filter
{
public:

  /// Constructor
  /// \param theWorkshop instance of workshop interface
  Standard_EXPORT PartSet_CirclePointFilter(ModuleBase_IWorkshop* theWorkshop):
    SelectMgr_Filter(),
    myWorkshop(theWorkshop) {};

  /// Returns True if the given owner is acceptable for selection
  /// \param theOwner the selected owner
  Standard_EXPORT virtual Standard_Boolean IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const;

  DEFINE_STANDARD_RTTI(PartSet_CirclePointFilter)

private:

  /// Reference to workshop
  ModuleBase_IWorkshop* myWorkshop;
};

#endif