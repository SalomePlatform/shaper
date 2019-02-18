// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include "ModuleBase_IPropertyPanel.h"
#include "ModuleBase_ModelWidget.h"
#include "ModuleBase_ToolBox.h"

#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>

ModuleBase_IPropertyPanel::ModuleBase_IPropertyPanel(QWidget* theParent)
 : QDockWidget(theParent), myIsEditing(false)
{
}

ModuleBase_ModelWidget* ModuleBase_IPropertyPanel::modelWidget(
                                          const std::string& theAttributeId) const
{
  ModuleBase_ModelWidget* aWidget = 0;
  QList<ModuleBase_ModelWidget*> aWidgets = modelWidgets();
  ModelAPI_ValidatorsFactory* aValidators = ModelAPI_Session::get()->validators();
  for (QList<ModuleBase_ModelWidget*>::const_iterator anIt = aWidgets.begin();
    anIt != aWidgets.end() && !aWidget; anIt++) {
    ModuleBase_ModelWidget* aCurrentWidget = *anIt;
    if (aCurrentWidget->attributeID() == theAttributeId &&
        aCurrentWidget->canAcceptFocus() &&
        aValidators->isCase(aCurrentWidget->feature(), aCurrentWidget->attributeID()))
      aWidget = aCurrentWidget;
  }

  return aWidget;
}

ModuleBase_ModelWidget* ModuleBase_IPropertyPanel::findFirstAcceptingValueWidget()
{
  return ModuleBase_IPropertyPanel::findFirstAcceptingValueWidget(modelWidgets());
}

ModuleBase_ModelWidget* ModuleBase_IPropertyPanel::findFirstAcceptingValueWidget(
                                        const QList<ModuleBase_ModelWidget*>& theWidgets)
{
  ModuleBase_ModelWidget* aFirstWidget = 0;

  ModelAPI_ValidatorsFactory* aValidators = ModelAPI_Session::get()->validators();

  ModuleBase_ModelWidget* aWgt;
  QList<ModuleBase_ModelWidget*>::const_iterator aWIt;
  bool isOffToolBox = false;
  for (aWIt = theWidgets.begin(); aWIt != theWidgets.end() && !aFirstWidget; ++aWIt) {
    aWgt = (*aWIt);
    if (!aValidators->isCase(aWgt->feature(), aWgt->attributeID()))
      continue; // this attribute is not participated in the current case

    if (!aWgt->canAcceptFocus())
      continue;

    /// workaround for the same attributes used in different stacked widgets(attribute types)
    if (ModuleBase_ToolBox::isOffToolBoxParent(aWgt))
      continue;

    aFirstWidget = aWgt;
  }
  return aFirstWidget;
}
