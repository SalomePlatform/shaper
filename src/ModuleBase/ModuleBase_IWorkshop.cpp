// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_IWorkshop.cpp
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "ModuleBase_IWorkshop.h"
#include "ModuleBase_FilterFactory.h"

ModuleBase_IWorkshop::ModuleBase_IWorkshop(QObject* theParent)
: QObject(theParent)
{
  myValidatorFilter = new ModuleBase_FilterValidated(this);
}

Handle(ModuleBase_FilterValidated) ModuleBase_IWorkshop::validatorFilter()
{
  return myValidatorFilter;
}

ModuleBase_FilterFactory* ModuleBase_IWorkshop::selectionFilters() const
{
  static ModuleBase_FilterFactory* aFactory = new ModuleBase_FilterFactory;
  return aFactory;
}
