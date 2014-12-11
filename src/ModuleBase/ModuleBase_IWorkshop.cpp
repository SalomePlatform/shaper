// File:        ModuleBase_ModelWidget.h
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "ModuleBase_IWorkshop.h"
#include "ModuleBase_FilterFactory.h"

ModuleBase_FilterFactory* ModuleBase_IWorkshop::selectionFilters() const
{
  static ModuleBase_FilterFactory* aFactory = new ModuleBase_FilterFactory;
  return aFactory;
}
