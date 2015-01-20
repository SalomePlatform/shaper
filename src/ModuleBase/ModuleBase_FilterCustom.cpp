// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_FilterCustom.cpp
// Created:     10 Dec 2014
// Author:      Natalia ERMOLAEVA


#include "ModuleBase_FilterCustom.h"

#include <StdSelect_EdgeFilter.hxx>
#include <StdSelect_TypeOfEdge.hxx>

#include <Events_Error.h>

#include <QString>
#include <QMap>

ModuleBase_FilterCustom::ModuleBase_FilterCustom(Handle(SelectMgr_Filter) theFilter)
: ModuleBase_Filter()
{
  myFilter = theFilter;
}

void ModuleBase_FilterCustom::createFilter()
{
}

void ModuleBase_FilterCustom::setArguments(const std::list<std::string>& theArguments)
{
}
