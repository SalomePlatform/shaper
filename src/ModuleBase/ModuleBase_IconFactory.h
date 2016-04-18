// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_IconFactory.h
// Created:     28 Jul 2015
// Author:      Vitaly SMETANNIKOV

#ifndef ModuleBase_IconFactory_H
#define ModuleBase_IconFactory_H

#include "ModuleBase.h"
#include <ModelAPI_Object.h>

#include <QPixmap>
#include <QIcon>
#include <QImage>

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

  /// Finds an icon for the given value. The icon is loaded: firstly from qrc file,
  /// then by the icon path. If the path contains ":" , it will be treated as absolute path to
  /// the icon, or relative path from the plugin library
  /// \param theValue an icon information
  /// \return a loaded or empty icon
  static QIcon loadIcon(const QString& theValue);

  /// Finds a pixmap for the given value. It is loaded: firstly from qrc file,
  /// then by the absolute path. If the pixmap is not loaded, it is searched in relative plugin path
  /// \param theValue a pixmap information
  /// \return a loaded or empty pixmap
  static QPixmap loadPixmap(const QString& theValue);

  /// Finds an image for the given value. It is loaded: firstly from qrc file,
  /// then by the absolute path. If the image is not loaded, it is searched in relative plugin path
  /// \param theValue a image information
  /// \return a loaded or empty pixmap
  static QImage loadImage(const QString& theValue);

protected:
  ModuleBase_IconFactory();

  /// Set the current icons factory instance
  /// \param theFactory a new factory
  static void setFactory(ModuleBase_IconFactory* theFactory);
};

#endif