// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetCreatorFactory.cpp
// Created:     03 Dec 2015
// Author:      Natalia ERMOLAEVA

#include <ModuleBase_WidgetCreatorFactory.h>
#include <ModuleBase_IWidgetCreator.h>

#include <Config_WidgetAPI.h>

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
    if (!myCreators.contains(aKey))
      myCreators[aKey] = theCreator;
    else {
      Events_Error::send("The" + aKey + " widget XML definition has been already \
used by another widget creator");
    }
  }

  const std::set<std::string>& aPTypes = theCreator->pageTypes();
  for (anIt = aPTypes.begin(), aLast = aPTypes.end(); anIt != aLast; anIt++) {
    std::string aKey = *anIt;
    if (!myPageToCreator.contains(aKey))
      myPageToCreator[aKey] = theCreator;
    else {
      Events_Error::send("The" + aKey + " page XML definition has been already \
used by another widget creator");
    }
  }
}

bool ModuleBase_WidgetCreatorFactory::hasPageWidget(const std::string& theType)
{
  return myPageToCreator.contains(theType);
}

ModuleBase_PageBase* ModuleBase_WidgetCreatorFactory::createPageByType(
                                          const std::string& theType, QWidget* theParent,
                                          Config_WidgetAPI* theWidgetApi)
{
  ModuleBase_PageBase* aPage = 0;

  if (myPageToCreator.contains(theType)) {
    WidgetCreatorPtr aCreator = myPageToCreator[theType];
    aPage = aCreator->createPageByType(theType, theParent, theWidgetApi);
  }

  return aPage;
}


ModuleBase_ModelWidget* ModuleBase_WidgetCreatorFactory::createWidgetByType(
                                                const std::string& theType, QWidget* theParent,
                                                Config_WidgetAPI* theWidgetApi,
                                                ModuleBase_IWorkshop* theWorkshop)
{
  ModuleBase_ModelWidget* aWidget = 0;

  if (myCreators.contains(theType)) {
    WidgetCreatorPtr aCreator = myCreators[theType];
    aWidget = aCreator->createWidgetByType(theType, theParent, theWidgetApi, theWorkshop);
  }

  return aWidget;
}
