// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_ErrorMgr.cpp
// Created:     22 July 2015
// Author:      Sergey POKHODENKO

#include "ModuleBase_IErrorMgr.h"

#include "ModuleBase_IPropertyPanel.h"
#include "ModuleBase_ModelWidget.h"

ModuleBase_IErrorMgr::ModuleBase_IErrorMgr(QObject* theParent /*= 0*/)
  : QObject(theParent)
  , myPropertyPanel(NULL)
{

}

ModuleBase_IErrorMgr::~ModuleBase_IErrorMgr()
{

}

void ModuleBase_IErrorMgr::setPropertyPanel(ModuleBase_IPropertyPanel* theProp)
{
  myPropertyPanel = theProp; 

  if (myPropertyPanel) {
    foreach(const ModuleBase_ModelWidget* aWgt, myPropertyPanel->modelWidgets()) {
      connect(aWgt, SIGNAL(afterValuesChanged()), this, SLOT(onWidgetChanged()));
      connect(aWgt, SIGNAL(afterValuesRestored()), this, SLOT(onWidgetChanged()));
    }
  }
}
