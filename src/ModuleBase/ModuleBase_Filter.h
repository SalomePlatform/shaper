// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_Filter.h
// Created:     10 Dec 2014
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_Filter_H
#define ModuleBase_Filter_H

#include "ModuleBase.h"

#include <SelectMgr_Filter.hxx>

/**
 * This object is assigned by the name
 * in the XML file to the specific attribute or to the whole feature.
 * If isOK method of the filter returns "false", it is signalized in user interface
 * that the processed entity should not be selected.
 * Filterss must be registered in the filters factory to be
 * correctly identified by the XML string-ID.
 */
DEFINE_STANDARD_HANDLE(ModuleBase_Filter, SelectMgr_Filter);
class ModuleBase_Filter: public SelectMgr_Filter
{
public:
  Standard_EXPORT ModuleBase_Filter(): SelectMgr_Filter() {}

  Standard_EXPORT virtual Standard_Boolean IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const;

  DEFINE_STANDARD_RTTI(ModuleBase_Filter)
};

#endif //ModuleBase_Filter
