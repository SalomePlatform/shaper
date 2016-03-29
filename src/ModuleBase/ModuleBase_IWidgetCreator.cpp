// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include "ModuleBase_IWidgetCreator.h"

ModuleBase_IWidgetCreator::ModuleBase_IWidgetCreator()
{
}

ModuleBase_IWidgetCreator::~ModuleBase_IWidgetCreator()
{
}

const std::set<std::string>& ModuleBase_IWidgetCreator::panelTypes()
{
  return std::set<std::string>();
}

const std::set<std::string>& ModuleBase_IWidgetCreator::pageTypes()
{
  return std::set<std::string>();
}

const std::set<std::string>& ModuleBase_IWidgetCreator::widgetTypes()
{
  return std::set<std::string>();
}
