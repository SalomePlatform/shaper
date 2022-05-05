// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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

ModuleBase_ModelWidget* SamplePanelPlugin_ModelWidgetCreator::createWidgetByType(
                                                            const std::string& theType,
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
