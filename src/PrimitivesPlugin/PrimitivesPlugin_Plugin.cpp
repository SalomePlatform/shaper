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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include <PrimitivesPlugin_Plugin.h>

#include <PrimitivesPlugin_Box.h>
#include <PrimitivesPlugin_Cylinder.h>
#include <ModelAPI_Session.h>

#include <string>
#include <memory>
#include <iostream>

// the only created instance of this plugin
static PrimitivesPlugin_Plugin* MY_PRIMITIVES_INSTANCE = new PrimitivesPlugin_Plugin();

PrimitivesPlugin_Plugin::PrimitivesPlugin_Plugin()
{
  // register this plugin
  ModelAPI_Session::get()->registerPlugin(this);
}

FeaturePtr PrimitivesPlugin_Plugin::createFeature(std::string theFeatureID)
{
  if (theFeatureID == PrimitivesPlugin_Box::ID()) {
    return FeaturePtr(new PrimitivesPlugin_Box);
  } else if (theFeatureID == PrimitivesPlugin_Cylinder::ID()) {
    return FeaturePtr(new PrimitivesPlugin_Cylinder);
  }
  // feature of such kind is not found
  return FeaturePtr();
}
