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
  std::set<std::string>::const_iterator anIt, aLast;
  /// fill map of panels
  std::set<std::string> aPanelTypes;
  theCreator->panelTypes(aPanelTypes);
  for (anIt = aPanelTypes.begin(), aLast = aPanelTypes.end(); anIt != aLast; anIt++) {
    std::string aKey = *anIt;
    if (!myPanelToCreator.contains(aKey))
      myPanelToCreator[aKey] = theCreator;
    else {
      Events_Error::send("The" + aKey + " panel XML definition has been already \
used by another widget creator");
    }
  }

  /// fill map of widgets
  std::set<std::string> aTypes;
  theCreator->widgetTypes(aTypes);
  for (anIt = aTypes.begin(), aLast = aTypes.end(); anIt != aLast; anIt++) {
    std::string aKey = *anIt;
    if (!myCreators.contains(aKey))
      myCreators[aKey] = theCreator;
    else {
      Events_Error::send("The" + aKey + " widget XML definition has been already \
used by another widget creator");
    }
  }

  /// fill map of pages
  std::set<std::string> aPTypes;
  theCreator->pageTypes(aPTypes);
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

bool ModuleBase_WidgetCreatorFactory::hasPanelWidget(const std::string& theType)
{
  return myPanelToCreator.contains(theType);
}

QWidget* ModuleBase_WidgetCreatorFactory::createPanel(const std::string& theType, QWidget* theParent)
{
  QWidget* aPanel = 0;
  if (myPanelToCreator.contains(theType)) {
    WidgetCreatorPtr aCreator = myPanelToCreator[theType];
    aPanel = aCreator->createPanelByType(theType, theParent);
  }
  return aPanel;
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
