// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ParametersPlugin_WidgetCreator.cpp
// Created:     11 Apr 2016
// Author:      Vitaly SMETANNIKOV

#include <ParametersPlugin_WidgetCreator.h>
#include <ParametersPlugin_WidgetParamsMgr.h>


ParametersPlugin_WidgetCreator::ParametersPlugin_WidgetCreator()
  : ModuleBase_IWidgetCreator()
{
}


void ParametersPlugin_WidgetCreator::widgetTypes(std::set<std::string>& theTypes)
{
  theTypes.clear();
  theTypes.insert("parameters-manager");
}


ModuleBase_ModelWidget* 
  ParametersPlugin_WidgetCreator::createWidgetByType(const std::string& theType,
                                                     QWidget* theParent,
                                                     Config_WidgetAPI* theWidgetApi,
                                                     ModuleBase_IWorkshop* /*theWorkshop*/)
{
  ModuleBase_ModelWidget* aModelWidget = 0;
  if (theType == "parameters-manager") {
    aModelWidget = new ParametersPlugin_WidgetParamsMgr(theParent, theWidgetApi);
  }
  return aModelWidget;
}
