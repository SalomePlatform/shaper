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
  ModuleBase_ModelWidget* aFirstWidget = 0;

  QList<ModuleBase_ModelWidget*> aWidgets = modelWidgets();
  ModuleBase_ModelWidget* aWgt;
  QList<ModuleBase_ModelWidget*>::const_iterator aWIt;
  for (aWIt = aWidgets.begin(); aWIt != aWidgets.end() && !aFirstWidget; ++aWIt) {
    aWgt = (*aWIt);
    if (aWgt->canSetValue())
      aFirstWidget = aWgt;
  }
  return aFirstWidget;
}
