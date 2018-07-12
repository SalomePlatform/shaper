// Copyright (C) 2017-20xx  CEA/DEN, EDF R&D
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

#include "BuildPlugin_Plugin.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <BuildPlugin_Vertex.h>
#include <BuildPlugin_Edge.h>
#include <BuildPlugin_Wire.h>
#include <BuildPlugin_Polyline.h>
#include <BuildPlugin_Face.h>
#include <BuildPlugin_Shell.h>
#include <BuildPlugin_Solid.h>
#include <BuildPlugin_CompSolid.h>
#include <BuildPlugin_Compound.h>
#include <BuildPlugin_SubShapes.h>
#include <BuildPlugin_Filling.h>
#include <BuildPlugin_Validators.h>

// the only created instance of this plugin
static BuildPlugin_Plugin* MY_INSTANCE = new BuildPlugin_Plugin();

//=================================================================================================
BuildPlugin_Plugin::BuildPlugin_Plugin()
{
  // Register validators.
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  aFactory->registerValidator("BuildPlugin_ValidatorBaseForBuild",
                              new BuildPlugin_ValidatorBaseForBuild());
  aFactory->registerValidator("BuildPlugin_ValidatorBaseForWire",
                              new BuildPlugin_ValidatorBaseForWire());
  aFactory->registerValidator("BuildPlugin_ValidatorBaseForFace",
                              new BuildPlugin_ValidatorBaseForFace());
  aFactory->registerValidator("BuildPlugin_ValidatorBaseForSolids",
                              new BuildPlugin_ValidatorBaseForSolids());
  aFactory->registerValidator("BuildPlugin_ValidatorSubShapesSelection",
                              new BuildPlugin_ValidatorSubShapesSelection());
  aFactory->registerValidator("BuildPlugin_ValidatorFillingSelection",
                              new BuildPlugin_ValidatorFillingSelection());

  // Register this plugin.
  ModelAPI_Session::get()->registerPlugin(this);
}

//=================================================================================================
FeaturePtr BuildPlugin_Plugin::createFeature(std::string theFeatureID)
{
  if(theFeatureID == BuildPlugin_Vertex::ID()) {
    return FeaturePtr(new BuildPlugin_Vertex());
  } else if(theFeatureID == BuildPlugin_Edge::ID()) {
    return FeaturePtr(new BuildPlugin_Edge());
  } else if(theFeatureID == BuildPlugin_Wire::ID()) {
    return FeaturePtr(new BuildPlugin_Wire());
  } else if (theFeatureID == BuildPlugin_Wire::ID()) {
	return FeaturePtr(new BuildPlugin_Wire());
  } else if (theFeatureID == BuildPlugin_Polyline::ID()) {
	return FeaturePtr(new BuildPlugin_Polyline());
  } else if (theFeatureID == BuildPlugin_Face::ID()) {
    return FeaturePtr(new BuildPlugin_Face());
  } else if(theFeatureID == BuildPlugin_Shell::ID()) {
    return FeaturePtr(new BuildPlugin_Shell());
  } else if(theFeatureID == BuildPlugin_Solid::ID()) {
    return FeaturePtr(new BuildPlugin_Solid());
  } else if(theFeatureID == BuildPlugin_CompSolid::ID()) {
    return FeaturePtr(new BuildPlugin_CompSolid());
  } else if(theFeatureID == BuildPlugin_Compound::ID()) {
    return FeaturePtr(new BuildPlugin_Compound());
  } else if(theFeatureID == BuildPlugin_SubShapes::ID()) {
    return FeaturePtr(new BuildPlugin_SubShapes());
  } else if(theFeatureID == BuildPlugin_Filling::ID()) {
    return FeaturePtr(new BuildPlugin_Filling());
  }

  // Feature of such kind is not found.
  return FeaturePtr();
}
