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

#include <XGUI_PropertyPanelSelector.h>
#include <XGUI_PropertyPanel.h>

//********************************************************************
XGUI_PropertyPanelSelector::XGUI_PropertyPanelSelector(XGUI_PropertyPanel* thePanel)
: myPanel(thePanel), myWidgetToBeActivated (NULL)
{
  connect(myPanel, SIGNAL(propertyPanelActivated()), this, SIGNAL(activated()));
  connect(myPanel, SIGNAL(propertyPanelDeactivated()), this, SIGNAL(deactivated()));
}

//********************************************************************
void XGUI_PropertyPanelSelector::reset()
{
  myWidgetToBeActivated = NULL;
}

//********************************************************************
void XGUI_PropertyPanelSelector::setActive(const bool& isActive)
{
  if (isActive && myWidgetToBeActivated) {
    // e.g. widget sketch label
    myPanel->activateWidget(myWidgetToBeActivated, true);
    myWidgetToBeActivated = NULL;
    return;
  }

  if (!isActive) { // on deactivating, store previous active widget
    ModuleBase_ModelWidget* aWidget = myPanel->activeWidget();
    if (aWidget && aWidget->needToBeActivated())
    {
      myWidgetToBeActivated = aWidget;
    }
    myPanel->activateWidget(NULL, false);
  }
}

//********************************************************************
bool XGUI_PropertyPanelSelector::needToBeActivated() const
{
  return myWidgetToBeActivated != NULL;
}

//********************************************************************
void XGUI_PropertyPanelSelector::processSelection()
{
  ModuleBase_ModelWidget* aWidget = myPanel->activeWidget();
  if (!aWidget)
    return;

  aWidget->processAction(ActionSelection);
}
