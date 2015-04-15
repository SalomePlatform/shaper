// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_SelectionValidator.cpp
// Created:     8 Jul 2014
// Author:      Vitaly SMETANNIKOV

#include <ModuleBase_SelectionValidator.h>

ModuleBase_SelectionValidator::~ModuleBase_SelectionValidator()
{
}

bool ModuleBase_SelectionValidator::isValid(const ModuleBase_ISelection* theSelection,
                                            const std::list<std::string>& theArguments) const
{
  return isValid(theSelection);
}
