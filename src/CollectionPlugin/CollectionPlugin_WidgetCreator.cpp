// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
