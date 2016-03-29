// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include "ModuleBase_IWidgetCreator.h"

ModuleBase_IWidgetCreator::ModuleBase_IWidgetCreator()
{
}

ModuleBase_IWidgetCreator::~ModuleBase_IWidgetCreator()
{
}

QWidget* ModuleBase_IWidgetCreator::createPanelByType(const std::string& theType,
                                                      QWidget* theParent)
{
  return 0;
}

ModuleBase_PageBase* ModuleBase_IWidgetCreator::createPageByType(const std::string& theType,
                                                                 QWidget* theParent,
                                                                 Config_WidgetAPI* theWidgetApi)
{
  return 0;
}

ModuleBase_ModelWidget* ModuleBase_IWidgetCreator::createWidgetByType(const std::string& theType,
                                                                 QWidget* theParent,
                                                                 Config_WidgetAPI* theWidgetApi,
                                                                 ModuleBase_IWorkshop* theWorkshop)
{
  return 0;
}
