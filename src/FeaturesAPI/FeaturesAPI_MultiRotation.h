// Copyright (C) 2017-2019  CEA/DEN, EDF R&D
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

// File:        FeaturesAPI_MultiRotation.h
// Created:     04 Apr 2017
// Author:      Clarisse Genrault (CEA)

#ifndef FEATURESAPI_MULTIROTATION_H_
#define FEATURESAPI_MULTIROTATION_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_MultiRotation.h>

#include <ModelHighAPI_Integer.h>
#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Double;
class ModelHighAPI_Dumper;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_MultiRotation
/// \ingroup CPPHighAPI
/// \brief Interface for Translation feature.
class FeaturesAPI_MultiRotation: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_MultiRotation(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  FeaturesAPI_MultiRotation(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                            const std::list<ModelHighAPI_Selection>& theMainObjects,
                            const ModelHighAPI_Selection& theAxis,
                            const ModelHighAPI_Integer& theNumber);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  FeaturesAPI_MultiRotation(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                            const std::list<ModelHighAPI_Selection>& theMainObjects,
                            const ModelHighAPI_Selection& theAxis,
                            const ModelHighAPI_Double& theStep,
                            const ModelHighAPI_Integer& theNumber);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_MultiRotation();

  INTERFACE_5(FeaturesPlugin_MultiRotation::ID(),
              mainObjects, FeaturesPlugin_MultiRotation::OBJECTS_LIST_ID(),
              ModelAPI_AttributeSelectionList, /** Main objects */,
              axisAngular, FeaturesPlugin_MultiRotation::AXIS_ANGULAR_ID(),
              ModelAPI_AttributeSelection, /** Angular direction */,
              useStepAngular, FeaturesPlugin_MultiRotation::USE_ANGULAR_STEP_ID(),
              ModelAPI_AttributeString, /** Use step angular */,
              stepAngular, FeaturesPlugin_MultiRotation::STEP_ANGULAR_ID(),
              ModelAPI_AttributeDouble, /** Step angular */,
              nbAngular, FeaturesPlugin_MultiRotation::NB_COPIES_ANGULAR_ID(),
              ModelAPI_AttributeInteger, /** Number of copies for angular */
             )

  /// Set main objects.
  FEATURESAPI_EXPORT
  void setMainObjects(const std::list<ModelHighAPI_Selection>& theMainObjects);

  /// Modify CreationMethod, axis_angular attribute of the feature.
  FEATURESAPI_EXPORT
  void setAxisAngular(const ModelHighAPI_Selection& theAxis);

  /// Modify CreationMethod, step_angular attribute of the feature.
  FEATURESAPI_EXPORT
  void setStepAngular(const ModelHighAPI_Double& theStep);

  /// Modify CreationMethod, nb_angular attribute of the feature.
  FEATURESAPI_EXPORT
  void setNumberAngular(const ModelHighAPI_Integer& theNumber);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Multirotation object.
typedef std::shared_ptr<FeaturesAPI_MultiRotation> MultiRotationPtr;

/// \ingroup CPPHighAPI
/// \brief Create MultiRotation feature.
FEATURESAPI_EXPORT
MultiRotationPtr addMultiRotation(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theMainObjects,
                                  const ModelHighAPI_Selection& theAxis,
                                  const ModelHighAPI_Double& theStep,
                                  const ModelHighAPI_Integer& theNumber = ModelHighAPI_Integer(""),
                                  const bool keepSubResults = false);

#endif // FEATURESAPI_MULTIROTATION_H_
