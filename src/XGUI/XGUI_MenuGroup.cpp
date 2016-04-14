// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        XGUI_MenuGroup.cpp
// Created:     13 Apr 2016
// Author:      Natalia ERMOLAEVA

#include <XGUI_MenuGroup.h>

XGUI_MenuGroup::XGUI_MenuGroup(const std::string& theName)
: myName(theName)
{
}

void XGUI_MenuGroup::setFeatureInfo(const std::shared_ptr<Config_FeatureMessage>& theMessage)
{
  myFeatureInfo.push_back(theMessage);
}

const std::list<std::shared_ptr<Config_FeatureMessage> >& XGUI_MenuGroup::featuresInfo() const
{
  return myFeatureInfo;
}
