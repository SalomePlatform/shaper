// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SamplePanelPlugin_WidgetCreator.cpp
// Created:     29 Mar 2015
// Author:      Natalia ERMOLAEVA

#include "SamplePanelPlugin_WidgetCreator.h"

#include "SamplePanelPlugin_Panel.h"

SamplePanelPlugin_WidgetCreator::SamplePanelPlugin_WidgetCreator()
: ModuleBase_IWidgetCreator()
{
  myPanelTypes.insert("SampePanel");
}

void SamplePanelPlugin_WidgetCreator::panelTypes(std::set<std::string>& theTypes)
{
  theTypes = myPanelTypes;
}

QWidget* SamplePanelPlugin_WidgetCreator::createPanelByType(const std::string& theType,
                                                            QWidget* theParent)
{
  QWidget* aWidget = 0;
  if (myPanelTypes.find(theType) == myPanelTypes.end())
    return aWidget;

  if (theType == "SampePanel") {
    aWidget = new SamplePanelPlugin_Panel(theParent);
  }

  return aWidget;
}
