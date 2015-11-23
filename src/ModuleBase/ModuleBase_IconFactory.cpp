// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_IconFactory.cpp
// Created:     28 Jul 2015
// Author:      Vitaly SMETANNIKOV


#include "ModuleBase_IconFactory.h"

ModuleBase_IconFactory* MYIconFactory = 0;


ModuleBase_IconFactory::ModuleBase_IconFactory()
{
  setFactory(this);
}

void ModuleBase_IconFactory::setFactory(ModuleBase_IconFactory* theFactory)
{
  if (MYIconFactory)
    delete MYIconFactory;
  MYIconFactory = theFactory;
}

ModuleBase_IconFactory* ModuleBase_IconFactory::get()
{
  if (!MYIconFactory) {
    MYIconFactory = new ModuleBase_IconFactory();
  }
  return MYIconFactory;
}

QIcon ModuleBase_IconFactory::getIcon(ObjectPtr theIcon)
{
  return QIcon();
}
