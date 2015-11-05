// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ModuleBase_IPropertyPanel.cpp
 *
 *  Created on: Oct 01, 2014
 *      Author: vsv
 */

#include "ModuleBase_IPropertyPanel.h"
#include "ModuleBase_ModelWidget.h"

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

  ModuleBase_ModelWidget* aWgt;
  QList<ModuleBase_ModelWidget*>::const_iterator aWIt;
  for (aWIt = theWidgets.begin(); aWIt != theWidgets.end() && !aFirstWidget; ++aWIt) {
    aWgt = (*aWIt);
    if (aWgt->canSetValue())
      aFirstWidget = aWgt;
  }
  return aFirstWidget;
}
