// Copyright (C) 2014-2023  CEA, EDF
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

#include "ConstructionPlugin_Plugin.h"
#include "ConstructionPlugin_Point.h"
#include "ConstructionPlugin_Axis.h"
#include "ConstructionPlugin_Plane.h"
#include "ConstructionPlugin_Validators.h"

#include <Config_PropManager.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>

#define SKETCH_WIDTH        "4"
/// face of the square-face displayed for selection of general plane
#define PLANE_SIZE          "200"

// the only created instance of this plugin
static ConstructionPlugin_Plugin* MY_CONSTRUCTION_INSTANCE = new ConstructionPlugin_Plugin();

ConstructionPlugin_Plugin::ConstructionPlugin_Plugin()
{
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  aFactory->registerValidator("ConstructionPlugin_ValidatorPointLines",
                              new ConstructionPlugin_ValidatorPointLines());
  aFactory->registerValidator("ConstructionPlugin_ValidatorPointEdgeAndPlaneNotParallel",
                              new ConstructionPlugin_ValidatorPointEdgeAndPlaneNotParallel());
  aFactory->registerValidator("ConstructionPlugin_ValidatorPlaneThreePoints",
                              new ConstructionPlugin_ValidatorPlaneThreePoints());
  aFactory->registerValidator("ConstructionPlugin_ValidatorPlaneLinePoint",
                              new ConstructionPlugin_ValidatorPlaneLinePoint());
  aFactory->registerValidator("ConstructionPlugin_ValidatorPlaneTwoParallelPlanes",
                              new ConstructionPlugin_ValidatorPlaneTwoParallelPlanes());
  aFactory->registerValidator("ConstructionPlugin_ValidatorAxisTwoNotParallelPlanes",
                              new ConstructionPlugin_ValidatorAxisTwoNotParallelPlanes());
  aFactory->registerValidator("ConstructionPlugin_ValidatorPointThreeNonParallelPlanes",
                              new ConstructionPlugin_ValidatorPointThreeNonParallelPlanes());
  aFactory->registerValidator("ConstructionPlugin_ValidatorNotFeature",
                              new ConstructionPlugin_ValidatorNotFeature());

  Config_PropManager::registerProp(SKETCH_TAB_NAME, "planes_size", "Size", Config_Prop::DblSpin,
                                   PLANE_SIZE, "0", "1000");
  Config_PropManager::registerProp(SKETCH_TAB_NAME, "planes_thickness", "Thickness",
    Config_Prop::IntSpin, SKETCH_WIDTH);
  Config_PropManager::registerProp(SKETCH_TAB_NAME, "angular_tolerance", "Angular tolerance",
    Config_Prop::DblSpin, "0.04");
  Config_PropManager::registerProp(SKETCH_TAB_NAME, "spline_weight", "Default spline weight",
    Config_Prop::DblSpin, "1.0");
  Config_PropManager::registerProp(SKETCH_TAB_NAME, "rotate_to_plane",
    "Rotate to plane when selected", Config_Prop::Boolean, "false");
  Config_PropManager::registerProp(SKETCH_TAB_NAME, "operation_cursor",
    "Cursor for Sketch operation", Config_Prop::Cursor, "0");
  Config_PropManager::registerProp(SKETCH_TAB_NAME, "create_by_dragging",
    "Create sketch entities by dragging", Config_Prop::Boolean, "false");

  // register this plugin
  ModelAPI_Session::get()->registerPlugin(this);

  // register construction properties
  Config_PropManager::registerProp("Visualization", ConstructionPlugin_Plane::COLOR_NAME(),
    "Construction plane color", Config_Prop::Color, ConstructionPlugin_Plane::DEFAULT_COLOR());

  Config_PropManager::registerProp("Visualization", ConstructionPlugin_Point::COLOR_NAME(),
    "Construction point color", Config_Prop::Color, ConstructionPlugin_Point::DEFAULT_COLOR());
}

FeaturePtr ConstructionPlugin_Plugin::createFeature(std::string theFeatureID)
{
  if (theFeatureID == ConstructionPlugin_Point::ID()) {
    return FeaturePtr(new ConstructionPlugin_Point);
  }
  else if (theFeatureID == ConstructionPlugin_Axis::ID()) {
    return FeaturePtr(new ConstructionPlugin_Axis);
  }
  else if (theFeatureID == ConstructionPlugin_Plane::ID()) {
    return FeaturePtr(new ConstructionPlugin_Plane);
  }
  // feature of such kind is not found
  return FeaturePtr();
}
