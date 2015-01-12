// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_FilterMulti.cpp
// Created:     12 Jan 2015
// Author:      Natalia ERMOLAEVA


#include "ModuleBase_FilterMulti.h"

#include "ModuleBase_FilterLinearEdge.h"
#include "ModuleBase_FilterShapeType.h"

#include <SelectMgr_OrFilter.hxx>

#include <Events_Error.h>

#include <QString>
#include <QMap>

ModuleBase_Filter* ModuleBase_FilterMulti::findFilter(const std::string& theType)
{
  ModuleBase_Filter* aFilter = 0;

  if (theType == "line") {
    aFilter = new ModuleBase_FilterLinearEdge();
    std::list<std::string> anArguments;
    anArguments.push_back(theType);
    aFilter->setArguments(anArguments);
  }
  if (theType == "vertex") {
    aFilter = new ModuleBase_FilterShapeType();
    std::list<std::string> anArguments;
    anArguments.push_back(theType);
    aFilter->setArguments(anArguments);
  }

  return aFilter;
}

void ModuleBase_FilterMulti::createFilter()
{
  myFilter = new SelectMgr_OrFilter();

  // set filter arguments
  Handle(SelectMgr_OrFilter) anOrFilter = Handle(SelectMgr_OrFilter)::DownCast(myFilter);
  if (anOrFilter.IsNull())
    return;

  std::list<std::string>::const_iterator anIt = myArguments.begin(),
                                         aLast = myArguments.end();
  for (; anIt != aLast; ++anIt) {
    std::string aType = *anIt;
    ModuleBase_Filter* aFilter = findFilter(aType);
    if (aFilter) {
      anOrFilter->Add(aFilter->getFilter());
    }
  }
}

void ModuleBase_FilterMulti::setArguments(const std::list<std::string>& theArguments)
{
  myArguments.clear();
  myArguments = theArguments;
}
