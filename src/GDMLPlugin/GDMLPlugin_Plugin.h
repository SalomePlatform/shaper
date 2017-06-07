// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef GDMLPLUGIN_PLUGIN_H_
#define GDMLPLUGIN_PLUGIN_H_

#include "GDMLPlugin.h"
#include <ModelAPI_Plugin.h>
#include <ModelAPI_Feature.h>

/**\class GDMLPlugin_Plugin
 * \ingroup Plugins
 * \brief The main class managing GDML Features as plugins.
 */
class GDMLPLUGIN_EXPORT GDMLPlugin_Plugin : public ModelAPI_Plugin
{
 public:
  /// Creates the feature object of this plugin by the feature string ID
  virtual FeaturePtr createFeature(std::string theFeatureID);

 public:
  /// Default constructor
  GDMLPlugin_Plugin();
};

#endif // GDMLPLUGIN_PLUGIN_H_
