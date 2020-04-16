// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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