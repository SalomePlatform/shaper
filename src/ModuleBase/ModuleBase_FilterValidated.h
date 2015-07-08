// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_FilterValidated.h
// Created:     17 Mar 2015
// Author:      Natalia ERMOLAEVA


#ifndef ModuleBase_FilterValidated_H
#define ModuleBase_FilterValidated_H

#include <SelectMgr_Filter.hxx>
#include <SelectMgr_EntityOwner.hxx>

class ModuleBase_IWorkshop;

//#define DEBUG_FILTERS

/**
* \ingroup GUI
* \class ModuleBase_FilterValidated
* A filter which provides filtering of selection in 3d viewer.
* Installing of this filter lets to select objects which belong to 
* currently active document or to global document
*/
DEFINE_STANDARD_HANDLE(ModuleBase_FilterValidated, SelectMgr_Filter);
class ModuleBase_FilterValidated: public SelectMgr_Filter
{
public:
  /// Constructor
  /// \param theWorkshop instance of workshop interface
  Standard_EXPORT ModuleBase_FilterValidated(ModuleBase_IWorkshop* theWorkshop):
      SelectMgr_Filter(), myWorkshop(theWorkshop) {}

  /**
   * Returns true if the owner is computed from decomposition(it is global selection, not the sub-shapes)
   * of if the selected result is a construction and the result feature is composite and has sub-elements.
   * \param theOwner the result of selection
   * \return whether the owner is selectable in the viewer
  */
  Standard_EXPORT virtual Standard_Boolean IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const;

  DEFINE_STANDARD_RTTI(ModuleBase_FilterValidated)

protected:
  /// Instance of workshop interface
  ModuleBase_IWorkshop* myWorkshop;
};

#endif