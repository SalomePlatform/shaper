// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_Operation.cpp
// Created:     24 Nov 2014
// Author:      Vitaly SMETANNIKOV

#include "PartSet_Operation.h"

#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_ISelection.h>

void PartSet_Operation::onSelectionChanged()
{
  if (isEditOperation()) {
    ModuleBase_ISelection* aSelection = myWorkshop->selection();
    QList<ModuleBase_ViewerPrs> aHighlighted = aSelection->getHighlighted();
    QList<ModuleBase_ViewerPrs> aSelected = aSelection->getSelected();
  }
}