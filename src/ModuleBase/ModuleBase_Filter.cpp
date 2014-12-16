// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_Filter.cpp
// Created:     10 Dec 2014
// Author:      Natalia ERMOLAEVA

#include "ModuleBase_Filter.h"

Handle(SelectMgr_Filter) ModuleBase_Filter::getFilter()
{
  if (myFilter.IsNull())
    createFilter();

  return myFilter;
}
