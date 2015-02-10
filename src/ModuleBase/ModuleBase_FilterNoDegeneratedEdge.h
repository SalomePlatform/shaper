// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_ViewerFilters.h
// Created:     10 Feb 2015
// Author:      Natalia ERMOLAEVA


#ifndef ModuleBase_FilterNoDegeneratedEdge_H
#define ModuleBase_FilterNoDegeneratedEdge_H

#include <QStringList>

#include <StdSelect_EdgeFilter.hxx>
#include <SelectMgr_EntityOwner.hxx>

/**
* \ingroup GUI
* \class ModuleBase_FilterNoDegeneratedEdge
* An edge filter, which additionally filters degenerative edges.
*/
DEFINE_STANDARD_HANDLE(ModuleBase_FilterNoDegeneratedEdge, StdSelect_EdgeFilter);
class ModuleBase_FilterNoDegeneratedEdge: public StdSelect_EdgeFilter
{
public:
  /// Constructor
  Standard_EXPORT ModuleBase_FilterNoDegeneratedEdge();

  /**
   * Returns true if the owner is computed from decomposition(it is global selection, not the sub-shapes)
   * of if the selected result is a construction and the result feature is composite and has sub-elements.
   * \param theOwner the result of selection
   * \return whether the owner is selectable in the viewer
  */
  Standard_EXPORT virtual Standard_Boolean IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const;

  DEFINE_STANDARD_RTTI(ModuleBase_FilterNoDegeneratedEdge)

protected:
};

#endif