// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        CollectionPlugin_WidgetCreator.cpp
// Created:     15 Nov 2016
// Author:      Vitaly SMETANNIKOV

#include "CollectionPlugin_WidgetCreator.h"
#include "CollectionPlugin_WidgetField.h"


CollectionPlugin_WidgetCreator::CollectionPlugin_WidgetCreator()
: ModuleBase_IWidgetCreator()
{
  myPanelTypes.insert("field-panel");
}

void CollectionPlugin_WidgetCreator::widgetTypes(std::set<std::string>& theTypes)
{
  theTypes = myPanelTypes;
}


ModuleBase_ModelWidget* CollectionPlugin_WidgetCreator::createWidgetByType(
                                                     const std::string& theType,
                                                     QWidget* theParent,
                                                     Config_WidgetAPI* theWidgetApi,
                                                     ModuleBase_IWorkshop* theWorkshop)
{
  ModuleBase_ModelWidget* aWidget = 0;
  if (myPanelTypes.find(theType) == myPanelTypes.end())
    return aWidget;

  if (theType == "field-panel") {
    aWidget = new CollectionPlugin_WidgetField(theParent, theWorkshop, theWidgetApi);
  }

  return aWidget;
}
