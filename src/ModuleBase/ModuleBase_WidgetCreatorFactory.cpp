// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetCreatorFactory.cpp
// Created:     03 Dec 2015
// Author:      Natalia ERMOLAEVA

#include <ModuleBase_WidgetCreatorFactory.h>
#include <ModuleBase_IWidgetCreator.h>

#include <Events_Error.h>

#include <QStringList>

/// Factory instance that will be initialized by first getting, one per application
std::shared_ptr<ModuleBase_WidgetCreatorFactory> MY_WIDGET_CREATOR_FACTORY;

std::shared_ptr<ModuleBase_WidgetCreatorFactory> ModuleBase_WidgetCreatorFactory::get()
{
  if (!MY_WIDGET_CREATOR_FACTORY) {
    MY_WIDGET_CREATOR_FACTORY =  std::shared_ptr<ModuleBase_WidgetCreatorFactory>(new ModuleBase_WidgetCreatorFactory());
  }
  return MY_WIDGET_CREATOR_FACTORY;
}

ModuleBase_WidgetCreatorFactory::ModuleBase_WidgetCreatorFactory()
{
}

ModuleBase_WidgetCreatorFactory::~ModuleBase_WidgetCreatorFactory()
{
}

void ModuleBase_WidgetCreatorFactory::registerCreator(const WidgetCreatorPtr& theCreator)
{
  const std::set<std::string>& aTypes = theCreator->widgetTypes();
  std::set<std::string>::const_iterator anIt = aTypes.begin(), aLast = aTypes.end();
  for (; anIt != aLast; anIt++) {
    std::string aKey = *anIt;
    if (!myModelWidgets.contains(aKey))
      myModelWidgets[aKey] = theCreator;
    else {
      Events_Error::send("The" + aKey + " widget XML definition has been already\
                                        used by another widget creator");
    }
  }
}

ModuleBase_ModelWidget* ModuleBase_WidgetCreatorFactory::createWidgetByType(
                              const std::string& theType, QWidget* theParent)
{
  ModuleBase_ModelWidget* aWidget = 0;

  if (myModelWidgets.contains(theType)) {
    WidgetCreatorPtr aCreator = myModelWidgets[theType];
    aWidget = aCreator->createWidgetByType(theType, theParent);
  }

  return aWidget;
}
