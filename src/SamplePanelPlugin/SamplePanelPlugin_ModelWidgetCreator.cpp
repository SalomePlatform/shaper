// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SamplePanelPlugin_ModelWidgetCreator.cpp
// Created:     29 Mar 2015
// Author:      Natalia ERMOLAEVA

#include "SamplePanelPlugin_ModelWidgetCreator.h"
#include "SamplePanelPlugin_ModelWidget.h"

#include <Config_WidgetAPI.h>

SamplePanelPlugin_ModelWidgetCreator::SamplePanelPlugin_ModelWidgetCreator()
: ModuleBase_IWidgetCreator()
{
  myModelWidgetTypes.insert("QtModelWidget");
}

void SamplePanelPlugin_ModelWidgetCreator::widgetTypes(std::set<std::string>& theTypes)
{
  theTypes = myModelWidgetTypes;
}

ModuleBase_ModelWidget* SamplePanelPlugin_ModelWidgetCreator::createWidgetByType(const std::string& theType,
                                                            QWidget* theParent,
                                                            Config_WidgetAPI* theWidgetApi,
                                                            ModuleBase_IWorkshop* /*theWorkshop*/)
{
  ModuleBase_ModelWidget* aModelWidget = 0;
  if (myModelWidgetTypes.find(theType) == myModelWidgetTypes.end())
    return aModelWidget;

  if (theType == "QtModelWidget") {
    aModelWidget = new SamplePanelPlugin_ModelWidget(theParent, theWidgetApi);
  }

  return aModelWidget;
}
