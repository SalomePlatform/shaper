// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetShapeSelector.cpp
// Created:     27 Nov 2014
// Author:      Vitaly Smetannikov

#include "PartSet_WidgetEditor.h"
#include "PartSet_Module.h"

#include <ModuleBase_IWorkshop.h>

#include <Config_WidgetAPI.h>

#include <QWidget>

PartSet_WidgetEditor::PartSet_WidgetEditor(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
                                           const Config_WidgetAPI* theData,
                                           const std::string& theParentId)
 : ModuleBase_WidgetEditor(theParent, theData, theParentId), myWorkshop(theWorkshop)
{
}

bool PartSet_WidgetEditor::focusTo()
{
  PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(myWorkshop->module());
  if (aModule->isMouseOverWindow())
    return ModuleBase_WidgetEditor::focusTo();
  else {
    return ModuleBase_WidgetDoubleValue::focusTo();
  }
}
