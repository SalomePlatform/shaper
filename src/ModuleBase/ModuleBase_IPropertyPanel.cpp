// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ModuleBase_IPropertyPanel.cpp
 *
 *  Created on: Oct 01, 2014
 *      Author: vsv
 */

#include "ModuleBase_IPropertyPanel.h"
#include "ModuleBase_ModelWidget.h"
#include "ModuleBase_ToolBox.h"

#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>

ModuleBase_IPropertyPanel::ModuleBase_IPropertyPanel(QWidget* theParent) : QDockWidget(theParent), myIsEditing(false)
{

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
