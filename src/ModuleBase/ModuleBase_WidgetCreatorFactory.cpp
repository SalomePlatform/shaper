// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include <ModuleBase_WidgetCreatorFactory.h>
#include <ModuleBase_IWidgetCreator.h>

#include <Config_WidgetAPI.h>

#include <Events_InfoMessage.h>

#include <QStringList>

/// Factory instance that will be initialized by first getting, one per application
std::shared_ptr<ModuleBase_WidgetCreatorFactory> MY_WIDGET_CREATOR_FACTORY;

std::shared_ptr<ModuleBase_WidgetCreatorFactory> ModuleBase_WidgetCreatorFactory::get()
{
  if (!MY_WIDGET_CREATOR_FACTORY) {
    MY_WIDGET_CREATOR_FACTORY =
      std::shared_ptr<ModuleBase_WidgetCreatorFactory>(new ModuleBase_WidgetCreatorFactory());
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
      Events_InfoMessage("ModuleBase_WidgetCreatorFactory",
        "The %1 panel XML definition has been already used by another widget creator")
        .arg(aKey).send();
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
      Events_InfoMessage("ModuleBase_WidgetCreatorFactory",
        "The %1 widget XML definition has been already used by another widget creator")
        .arg(aKey).send();
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
      Events_InfoMessage("ModuleBase_WidgetCreatorFactory",
        "The %1 page XML definition has been already used by another widget creator")
        .arg(aKey).send();
    }
  }
}

bool ModuleBase_WidgetCreatorFactory::hasPanelWidget(const std::string& theType)
{
  return myPanelToCreator.contains(theType);
}

QWidget* ModuleBase_WidgetCreatorFactory::createPanelByType(const std::string& theType,
                                                            QWidget* theParent,
                                                            const FeaturePtr& theFeature)
{
  QWidget* aPanel = 0;
  if (myPanelToCreator.contains(theType)) {
    WidgetCreatorPtr aCreator = myPanelToCreator[theType];
    aPanel = aCreator->createPanelByType(theType, theParent, theFeature);
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
