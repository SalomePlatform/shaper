// Copyright (C) 2014-2024  CEA, EDF
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

#ifndef FILTERSPLUGIN_PLUGIN_H_
#define FILTERSPLUGIN_PLUGIN_H_

#include "FiltersPlugin.h"
#include <ModelAPI_Plugin.h>
#include <ModelAPI_Feature.h>

/**\class GeomValidators_Plugin
 * \ingroup Plugins
 * \brief Interface common for any plugin: allows to use plugin by the plugins manager.
 */
class FILTERS_EXPORT FiltersPlugin_Plugin : public ModelAPI_Plugin
{
public:
  /// Creates the feature object of this plugin by the feature string ID
  virtual FeaturePtr createFeature(std::string theFeatureID);

public:
  FiltersPlugin_Plugin();
};

#endif
