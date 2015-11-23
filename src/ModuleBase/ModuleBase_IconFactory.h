// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_IconFactory.h
// Created:     28 Jul 2015
// Author:      Vitaly SMETANNIKOV

#ifndef ModuleBase_IconFactory_H
#define ModuleBase_IconFactory_H

#include "ModuleBase.h"
#include <ModelAPI_Object.h>
#include <QIcon>

/**\class ModuleBase_IconFactory
 * \ingroup GUI
 * \brief This is a class which provides icons of objects for object browser 
 */
class MODULEBASE_EXPORT ModuleBase_IconFactory
{
public:
  /// Returns icons factory instance
  static ModuleBase_IconFactory* get();

  /// Returns Icon for the given object
  /// \param theObj an object
  virtual QIcon getIcon(ObjectPtr theObj);

protected:
  ModuleBase_IconFactory();

  /// Set the current icons factory instance
  /// \param theFactory a new factory
  static void setFactory(ModuleBase_IconFactory* theFactory);
};

#endif