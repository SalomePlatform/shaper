// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#include <FeaturesPlugin_Plugin.h>

#include <FeaturesPlugin_Boolean.h>
#include <FeaturesPlugin_Extrusion.h>
#include <FeaturesPlugin_ExtrusionCut.h>
#include <FeaturesPlugin_ExtrusionFuse.h>
#include <FeaturesPlugin_Group.h>
#include <FeaturesPlugin_Intersection.h>
#include <FeaturesPlugin_Translation.h>
#include <FeaturesPlugin_Partition.h>
#include <FeaturesPlugin_Pipe.h>
#include <FeaturesPlugin_Placement.h>
#include <FeaturesPlugin_Recover.h>
#include <FeaturesPlugin_RemoveSubShapes.h>
#include <FeaturesPlugin_Revolution.h>
#include <FeaturesPlugin_RevolutionCut.h>
#include <FeaturesPlugin_RevolutionFuse.h>
#include <FeaturesPlugin_Rotation.h>
#include <FeaturesPlugin_Union.h>
#include <FeaturesPlugin_ValidatorTransform.h>
#include <FeaturesPlugin_Validators.h>

#include <ModelAPI_Session.h>

#include <string>

#include <memory>

using namespace std;

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

  // register this plugin
  ModelAPI_Session::get()->registerPlugin(this);
}

FeaturePtr FeaturesPlugin_Plugin::createFeature(string theFeatureID)
{
  if (theFeatureID == FeaturesPlugin_Extrusion::ID()) {
    return FeaturePtr(new FeaturesPlugin_Extrusion);
  } else if (theFeatureID == FeaturesPlugin_Revolution::ID()) {
   return FeaturePtr(new FeaturesPlugin_Revolution);
  } else if (theFeatureID == FeaturesPlugin_Rotation::ID()) {
    return FeaturePtr(new FeaturesPlugin_Rotation);
  } else if (theFeatureID == FeaturesPlugin_Translation::ID()) {
    return FeaturePtr(new FeaturesPlugin_Translation);
  } else if (theFeatureID == FeaturesPlugin_Boolean::ID()) {
    return FeaturePtr(new FeaturesPlugin_Boolean);
  } else if (theFeatureID == FeaturesPlugin_Group::ID()) {
    return FeaturePtr(new FeaturesPlugin_Group);
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
  }

  // feature of such kind is not found
  return FeaturePtr();
}
