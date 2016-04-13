// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        XGUI_MenuWorkbench.cpp
// Created:     13 Apr 2016
// Author:      Natalia ERMOLAEVA

#include <XGUI_MenuWorkbench.h>

#include <XGUI_MenuGroup.h>

XGUI_MenuWorkbench::XGUI_MenuWorkbench(const std::string& theName)
: myName(theName)
{
}

std::shared_ptr<XGUI_MenuGroup> XGUI_MenuWorkbench::findGroup(const std::string& theGroupName)
{
  std::list< std::shared_ptr<XGUI_MenuGroup> >::const_iterator anIt = myGroups.begin(),
                                                              aLast = myGroups.end();
  std::shared_ptr<XGUI_MenuGroup> aResultGroup = 0;
  for (; anIt != aLast && !aResultGroup; anIt++) {
    std::shared_ptr<XGUI_MenuGroup> aGroup = *anIt;
    if (aGroup->getName() == theGroupName)
      aResultGroup = aGroup;
  }
  if (!aResultGroup) {
    aResultGroup = std::shared_ptr<XGUI_MenuGroup>(new XGUI_MenuGroup(theGroupName));
    myGroups.push_back(aResultGroup);
  }
  return aResultGroup;
}

const std::list<std::shared_ptr<XGUI_MenuGroup> >& XGUI_MenuWorkbench::groups() const
{
  return myGroups;
}
