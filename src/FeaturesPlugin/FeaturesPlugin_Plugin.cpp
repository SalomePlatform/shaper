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

#include <FeaturesPlugin_Plugin.h>

#include <FeaturesPlugin_BooleanCut.h>
#include <FeaturesPlugin_BooleanFuse.h>
#include <FeaturesPlugin_BooleanCommon.h>
#include <FeaturesPlugin_BooleanSmash.h>
#include <FeaturesPlugin_BooleanFill.h>
#include <FeaturesPlugin_BoundingBox.h>
#include <FeaturesPlugin_Chamfer.h>
#include <FeaturesPlugin_Defeaturing.h>
#include <FeaturesPlugin_Extrusion.h>
#include <FeaturesPlugin_ExtrusionCut.h>
#include <FeaturesPlugin_ExtrusionFuse.h>
#include <FeaturesPlugin_Fillet.h>
#include <FeaturesPlugin_Fillet1D.h>
#include <FeaturesPlugin_GeometryCalculation.h>
#include <FeaturesPlugin_GlueFaces.h>
#include <FeaturesPlugin_GroupSharedFaces.h>
#include <FeaturesPlugin_InspectBoundingBox.h>
#include <FeaturesPlugin_InspectNormalToFace.h>
#include <FeaturesPlugin_Intersection.h>
#include <FeaturesPlugin_LimitTolerance.h>
#include <FeaturesPlugin_Measurement.h>
#include <FeaturesPlugin_PointCoordinates.h>
#include <FeaturesPlugin_MultiRotation.h>
#include <FeaturesPlugin_MultiTranslation.h>
#include <FeaturesPlugin_NormalToFace.h>
#include <FeaturesPlugin_Partition.h>
#include <FeaturesPlugin_Pipe.h>
#include <FeaturesPlugin_Loft.h>
#include <FeaturesPlugin_Placement.h>
#include <FeaturesPlugin_PointCloudOnFace.h>
#include <FeaturesPlugin_Recover.h>
#include <FeaturesPlugin_RemoveSubShapes.h>
#include <FeaturesPlugin_Revolution.h>
#include <FeaturesPlugin_RevolutionCut.h>
#include <FeaturesPlugin_RevolutionFuse.h>
#include <FeaturesPlugin_Rotation.h>
#include <FeaturesPlugin_Scale.h>
#include <FeaturesPlugin_Sewing.h>
#include <FeaturesPlugin_SharedFaces.h>
#include <FeaturesPlugin_Symmetry.h>
#include <FeaturesPlugin_Translation.h>
#include <FeaturesPlugin_Union.h>
#include <FeaturesPlugin_FusionFaces.h>
#include <FeaturesPlugin_RemoveResults.h>
#include <FeaturesPlugin_Copy.h>
#include <FeaturesPlugin_ImportResult.h>
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
  aFactory->registerValidator("FeaturesPlugin_ValidatorPipeLocationsNumber",
                              new FeaturesPlugin_ValidatorPipeLocationsNumber);
  aFactory->registerValidator("FeaturesPlugin_ValidatorLoftSameTypeShape",
                              new FeaturesPlugin_ValidatorLoftSameTypeShape);
  aFactory->registerValidator("FeaturesPlugin_ValidatorExtrusionDir",
                              new FeaturesPlugin_ValidatorExtrusionDir);
  aFactory->registerValidator("FeaturesPlugin_ValidatorExtrusionBoundary",
                              new FeaturesPlugin_ValidatorExtrusionBoundaryFace);
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
  aFactory->registerValidator("FeaturesPlugin_ValidatorFillet1DSelection",
                              new FeaturesPlugin_ValidatorFillet1DSelection);
  aFactory->registerValidator("FeaturesPlugin_ValidatorCircular",
                              new FeaturesPlugin_ValidatorCircular);
  aFactory->registerValidator("FeaturesPlugin_ValidatorBooleanArguments",
                              new FeaturesPlugin_ValidatorBooleanArguments);
  aFactory->registerValidator("FeaturesPlugin_ValidatorBooleanSmashSelection",
                              new FeaturesPlugin_ValidatorBooleanSmashSelection);
  aFactory->registerValidator("FeaturesPlugin_IntersectionSelection",
                              new FeaturesPlugin_IntersectionSelection);
  aFactory->registerValidator("FeaturesPlugin_ValidatorBooleanFuseSelection",
                              new FeaturesPlugin_ValidatorBooleanFuseSelection);
  aFactory->registerValidator("FeaturesPlugin_ValidatorBooleanFuseArguments",
                              new FeaturesPlugin_ValidatorBooleanFuseArguments);
  aFactory->registerValidator("FeaturesPlugin_ValidatorBooleanCommonSelection",
                              new FeaturesPlugin_ValidatorBooleanCommonSelection);
  aFactory->registerValidator("FeaturesPlugin_ValidatorBooleanCommonArguments",
                              new FeaturesPlugin_ValidatorBooleanCommonArguments);
  aFactory->registerValidator("FeaturesPlugin_ValidatorImportResults",
                              new FeaturesPlugin_ValidatorImportResults);
  aFactory->registerValidator("FeaturesPlugin_ValidatorDefeaturingSelection",
                              new FeaturesPlugin_ValidatorDefeaturingSelection);
  aFactory->registerValidator("FeaturesPlugin_ValidatorSewingSelection",
                              new FeaturesPlugin_ValidatorSewingSelection);
  aFactory->registerValidator("FeaturesPlugin_ValidatorGlueFacesSelection",
                              new FeaturesPlugin_ValidatorGlueFacesSelection);

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
  } else if (theFeatureID == FeaturesPlugin_BooleanFill::ID() || theFeatureID == "Fill") {
    return FeaturePtr(new FeaturesPlugin_BooleanFill);
  } else if (theFeatureID == FeaturesPlugin_Intersection::ID()) {
    return FeaturePtr(new FeaturesPlugin_Intersection);
  } else if (theFeatureID == FeaturesPlugin_Partition::ID()) {
    return FeaturePtr(new FeaturesPlugin_Partition);
  } else if (theFeatureID == FeaturesPlugin_Pipe::ID()) {
    return FeaturePtr(new FeaturesPlugin_Pipe);
  } else if (theFeatureID == FeaturesPlugin_Loft::ID()) {
    return FeaturePtr(new FeaturesPlugin_Loft);
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
  } else if (theFeatureID == FeaturesPlugin_Sewing::ID()) {
    return FeaturePtr(new FeaturesPlugin_Sewing);
  } else if (theFeatureID == FeaturesPlugin_MultiTranslation::ID()) {
    return FeaturePtr(new FeaturesPlugin_MultiTranslation);
  } else if (theFeatureID == FeaturesPlugin_MultiRotation::ID()) {
    return FeaturePtr(new FeaturesPlugin_MultiRotation);
  } else if (theFeatureID == FeaturesPlugin_Fillet::ID()) {
    return FeaturePtr(new FeaturesPlugin_Fillet);
  } else if (theFeatureID == FeaturesPlugin_Fillet1D::ID()) {
    return FeaturePtr(new FeaturesPlugin_Fillet1D);
  } else if (theFeatureID == FeaturesPlugin_Measurement::ID()) {
    return FeaturePtr(new FeaturesPlugin_Measurement);
  } else if (theFeatureID == FeaturesPlugin_RemoveResults::ID()) {
    return FeaturePtr(new FeaturesPlugin_RemoveResults);
  } else if (theFeatureID == FeaturesPlugin_Chamfer::ID()) {
    return FeaturePtr(new FeaturesPlugin_Chamfer);
  } else if (theFeatureID == FeaturesPlugin_Copy::ID()) {
    return FeaturePtr(new FeaturesPlugin_Copy);
  } else if (theFeatureID == FeaturesPlugin_GlueFaces::ID()) {
    return FeaturePtr(new FeaturesPlugin_GlueFaces);
  } else if (theFeatureID == FeaturesPlugin_ImportResult::ID()) {
    return FeaturePtr(new FeaturesPlugin_ImportResult);
  } else if (theFeatureID == FeaturesPlugin_Defeaturing::ID()) {
    return FeaturePtr(new FeaturesPlugin_Defeaturing);
  } else if (theFeatureID == FeaturesPlugin_PointCoordinates::ID()) {
    return FeaturePtr(new FeaturesPlugin_PointCoordinates);
  } else if (theFeatureID == FeaturesPlugin_GeometryCalculation::ID()) {
    return FeaturePtr(new FeaturesPlugin_GeometryCalculation);
  } else if (theFeatureID == FeaturesPlugin_InspectBoundingBox::ID()) {
    return FeaturePtr(new FeaturesPlugin_InspectBoundingBox);
  } else if (theFeatureID == FeaturesPlugin_BoundingBox::ID()) {
    return FeaturePtr(new FeaturesPlugin_BoundingBox);
  } else if (theFeatureID == FeaturesPlugin_InspectNormalToFace::ID()) {
    return FeaturePtr(new FeaturesPlugin_InspectNormalToFace);
  } else if (theFeatureID == FeaturesPlugin_NormalToFace::ID()) {
    return FeaturePtr(new FeaturesPlugin_NormalToFace);
  } else if (theFeatureID == FeaturesPlugin_PointCloudOnFace::ID()) {
    return FeaturePtr(new FeaturesPlugin_PointCloudOnFace);
  } else if (theFeatureID == FeaturesPlugin_LimitTolerance::ID()) {
    return FeaturePtr(new FeaturesPlugin_LimitTolerance);
  } else if (theFeatureID == FeaturesPlugin_SharedFaces::ID()) {
    return FeaturePtr(new FeaturesPlugin_SharedFaces);
  } else if (theFeatureID == FeaturesPlugin_GroupSharedFaces::ID()) {
    return FeaturePtr(new FeaturesPlugin_GroupSharedFaces);
  }


  // feature of such kind is not found
  return FeaturePtr();
}
