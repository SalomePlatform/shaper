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

#include <FeaturesPlugin_Plugin.h>

#include <FeaturesPlugin_BooleanCut.h>
#include <FeaturesPlugin_BooleanFuse.h>
#include <FeaturesPlugin_BooleanCommon.h>
#include <FeaturesPlugin_BooleanSmash.h>
#include <FeaturesPlugin_BooleanFill.h>
#include <FeaturesPlugin_Extrusion.h>
#include <FeaturesPlugin_ExtrusionCut.h>
#include <FeaturesPlugin_ExtrusionFuse.h>
#include <FeaturesPlugin_Fillet.h>
#include <FeaturesPlugin_Intersection.h>
#include <FeaturesPlugin_Measurement.h>
#include <FeaturesPlugin_MultiRotation.h>
#include <FeaturesPlugin_MultiTranslation.h>
#include <FeaturesPlugin_Partition.h>
#include <FeaturesPlugin_Pipe.h>
#include <FeaturesPlugin_Placement.h>
#include <FeaturesPlugin_Recover.h>
#include <FeaturesPlugin_RemoveSubShapes.h>
#include <FeaturesPlugin_Revolution.h>
#include <FeaturesPlugin_RevolutionCut.h>
#include <FeaturesPlugin_RevolutionFuse.h>
#include <FeaturesPlugin_Rotation.h>
#include <FeaturesPlugin_Scale.h>
#include <FeaturesPlugin_Symmetry.h>
#include <FeaturesPlugin_Translation.h>
#include <FeaturesPlugin_Union.h>
#include <FeaturesPlugin_FusionFaces.h>
#include <FeaturesPlugin_ValidatorTransform.h>
#include <FeaturesPlugin_Validators.h>

#include <ModelAPI_Session.h>

#include <string>

#include <memory>

// the only created instance of this plugin
static FeaturesPlugin_Plugin* MY_FEATURES_INSTANCE = new FeaturesPlugin_Plugin();

FeaturesPlugin_Plugin::FeaturesPlugin_Plugin()
{
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  aFactory->registerValidator("FeaturesPlugin_ValidatorTransform",
                              new FeaturesPlugin_ValidatorTransform);
  aFactory->registerValidator("FeaturesPlugin_ValidatorCompositeLauncher",
                              new FeaturesPlugin_ValidatorCompositeLauncher);
  aFactory->registerValidator("FeaturesPlugin_ValidatorBaseForGeneration",
                              new FeaturesPlugin_ValidatorBaseForGeneration);
  aFactory->registerValidator("FeaturesPlugin_ValidatorBaseForGenerationSketchOrSketchObjects",
                              new FeaturesPlugin_ValidatorBaseForGenerationSketchOrSketchObjects);
  aFactory->registerValidator("FeaturesPlugin_ValidatorPipeLocations",
                              new FeaturesPlugin_ValidatorPipeLocations);
  aFactory->registerValidator("FeaturesPlugin_ValidatorExtrusionDir",
                              new FeaturesPlugin_ValidatorExtrusionDir);
  aFactory->registerValidator("FeaturesPlugin_ValidatorBooleanSelection",
                              new FeaturesPlugin_ValidatorBooleanSelection);
  aFactory->registerValidator("FeaturesPlugin_ValidatorPartitionSelection",
                              new FeaturesPlugin_ValidatorPartitionSelection);
  aFactory->registerValidator("FeaturesPlugin_ValidatorRemoveSubShapesSelection",
                              new FeaturesPlugin_ValidatorRemoveSubShapesSelection);
  aFactory->registerValidator("FeaturesPlugin_ValidatorRemoveSubShapesResult",
                              new FeaturesPlugin_ValidatorRemoveSubShapesResult);
  aFactory->registerValidator("FeaturesPlugin_ValidatorPipePath",
                              new FeaturesPlugin_ValidatorPipePath);
  aFactory->registerValidator("FeaturesPlugin_ValidatorUnionSelection",
                              new FeaturesPlugin_ValidatorUnionSelection);
  aFactory->registerValidator("FeaturesPlugin_ValidatorUnionArguments",
                              new FeaturesPlugin_ValidatorUnionArguments);
  aFactory->registerValidator("FeaturesPlugin_ValidatorConcealedResult",
                              new FeaturesPlugin_ValidatorConcealedResult);
  aFactory->registerValidator("FeaturesPlugin_ValidatorFilletSelection",
                              new FeaturesPlugin_ValidatorFilletSelection);
  aFactory->registerValidator("FeaturesPlugin_ValidatorCircular",
                              new FeaturesPlugin_ValidatorCircular);
  aFactory->registerValidator("FeaturesPlugin_ValidatorBooleanArguments",
                              new FeaturesPlugin_ValidatorBooleanArguments);
  aFactory->registerValidator("FeaturesPlugin_ValidatorBooleanSmashSelection",
                              new FeaturesPlugin_ValidatorBooleanSmashSelection);
  aFactory->registerValidator("FeaturesPlugin_IntersectionSelection",
                              new FeaturesPlugin_IntersectionSelection);

  // register this plugin
  ModelAPI_Session::get()->registerPlugin(this);
}

FeaturePtr FeaturesPlugin_Plugin::createFeature(std::string theFeatureID)
{
  if (theFeatureID == FeaturesPlugin_Extrusion::ID()) {
    return FeaturePtr(new FeaturesPlugin_Extrusion);
  } else if (theFeatureID == FeaturesPlugin_Revolution::ID()) {
   return FeaturePtr(new FeaturesPlugin_Revolution);
  } else if (theFeatureID == FeaturesPlugin_Rotation::ID()) {
    return FeaturePtr(new FeaturesPlugin_Rotation);
  } else if (theFeatureID == FeaturesPlugin_Translation::ID()) {
    return FeaturePtr(new FeaturesPlugin_Translation);
  } else if (theFeatureID == FeaturesPlugin_BooleanCut::ID()) {
    return FeaturePtr(new FeaturesPlugin_BooleanCut);
  } else if (theFeatureID == FeaturesPlugin_BooleanFuse::ID()) {
    return FeaturePtr(new FeaturesPlugin_BooleanFuse);
  } else if (theFeatureID == FeaturesPlugin_BooleanCommon::ID()) {
    return FeaturePtr(new FeaturesPlugin_BooleanCommon);
  } else if (theFeatureID == FeaturesPlugin_BooleanSmash::ID()) {
    return FeaturePtr(new FeaturesPlugin_BooleanSmash);
  } else if (theFeatureID == FeaturesPlugin_BooleanFill::ID()) {
    return FeaturePtr(new FeaturesPlugin_BooleanFill);
  } else if (theFeatureID == FeaturesPlugin_Intersection::ID()) {
    return FeaturePtr(new FeaturesPlugin_Intersection);
  } else if (theFeatureID == FeaturesPlugin_Partition::ID()) {
    return FeaturePtr(new FeaturesPlugin_Partition);
  } else if (theFeatureID == FeaturesPlugin_Pipe::ID()) {
    return FeaturePtr(new FeaturesPlugin_Pipe);
  } else if (theFeatureID == FeaturesPlugin_Placement::ID()) {
    return FeaturePtr(new FeaturesPlugin_Placement);
  } else if (theFeatureID == FeaturesPlugin_Recover::ID()) {
    return FeaturePtr(new FeaturesPlugin_Recover);
  } else if (theFeatureID == FeaturesPlugin_ExtrusionCut::ID()) {
    return FeaturePtr(new FeaturesPlugin_ExtrusionCut);
  } else if (theFeatureID == FeaturesPlugin_ExtrusionFuse::ID()) {
    return FeaturePtr(new FeaturesPlugin_ExtrusionFuse);
  } else if (theFeatureID == FeaturesPlugin_RevolutionCut::ID()) {
    return FeaturePtr(new FeaturesPlugin_RevolutionCut);
  } else if (theFeatureID == FeaturesPlugin_RevolutionFuse::ID()) {
    return FeaturePtr(new FeaturesPlugin_RevolutionFuse);
  } else if (theFeatureID == FeaturesPlugin_RemoveSubShapes::ID()) {
    return FeaturePtr(new FeaturesPlugin_RemoveSubShapes);
  } else if (theFeatureID == FeaturesPlugin_Union::ID()) {
    return FeaturePtr(new FeaturesPlugin_Union);
  } else if (theFeatureID == FeaturesPlugin_FusionFaces::ID()) {
    return FeaturePtr(new FeaturesPlugin_FusionFaces);
  } else if (theFeatureID == FeaturesPlugin_Symmetry::ID()) {
    return FeaturePtr(new FeaturesPlugin_Symmetry);
  } else if (theFeatureID == FeaturesPlugin_Scale::ID()) {
    return FeaturePtr(new FeaturesPlugin_Scale);
  } else if (theFeatureID == FeaturesPlugin_MultiTranslation::ID()) {
    return FeaturePtr(new FeaturesPlugin_MultiTranslation);
  } else if (theFeatureID == FeaturesPlugin_MultiRotation::ID()) {
    return FeaturePtr(new FeaturesPlugin_MultiRotation);
  } else if (theFeatureID == FeaturesPlugin_Fillet::ID()) {
    return FeaturePtr(new FeaturesPlugin_Fillet);
  } else if (theFeatureID == FeaturesPlugin_Measurement::ID()) {
    return FeaturePtr(new FeaturesPlugin_Measurement);
  }

  // feature of such kind is not found
  return FeaturePtr();
}
