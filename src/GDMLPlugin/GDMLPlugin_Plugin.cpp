// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include <ModelAPI_Session.h>

#include <GDMLPlugin_Plugin.h>

#include <GDMLPlugin_ConeSegment.h>
#include <GDMLPlugin_Ellipsoid.h>

// the only created instance of this plugin
static GDMLPlugin_Plugin* MY_GDML_INSTANCE = new GDMLPlugin_Plugin();

GDMLPlugin_Plugin::GDMLPlugin_Plugin()
{
  // register this plugin
  ModelAPI_Session::get()->registerPlugin(this);
}

FeaturePtr GDMLPlugin_Plugin::createFeature(std::string theFeatureID)
{
  if (theFeatureID == GDMLPlugin_ConeSegment::ID()) {
    return FeaturePtr(new GDMLPlugin_ConeSegment);
  } else if (theFeatureID == GDMLPlugin_Ellipsoid::ID()) {
    return FeaturePtr(new GDMLPlugin_Ellipsoid);
  }

  // feature of such kind is not found
  return FeaturePtr();
}

