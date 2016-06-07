// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_Rotation.h
// Created:     07 June 2016
// Author:      Dmitry Bobylev

#ifndef FeaturesAPI_Rotation_H_
#define FeaturesAPI_Rotation_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Rotation.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Double;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_Rotation
/// \ingroup CPPHighAPI
/// \brief Interface for Rotation feature.
class FeaturesAPI_Rotation : public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Rotation(const std::shared_ptr<ModelAPI_Feature> & theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  FeaturesAPI_Rotation(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                       const std::list<ModelHighAPI_Selection>& theMainObjects,
                       const ModelHighAPI_Selection& theAxisObject,
                       const ModelHighAPI_Double& theAngle);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Rotation();

  INTERFACE_3(FeaturesPlugin_Rotation::ID(),
              mainObjects, FeaturesPlugin_Rotation::OBJECTS_LIST_ID(), ModelAPI_AttributeSelectionList, /** Main objects */,
              axisObject, FeaturesPlugin_Rotation::AXIS_OBJECT_ID(), ModelAPI_AttributeSelection, /** Axis object */,
              angle, FeaturesPlugin_Rotation::ANGLE_ID(), ModelAPI_AttributeDouble, /** Angle */)

  /// Set main objects.
  FEATURESAPI_EXPORT
  void setMainObjects(const std::list<ModelHighAPI_Selection>& theMainObjects);

  /// Set axis object.
  FEATURESAPI_EXPORT
  void setAxisObject(const ModelHighAPI_Selection& theAxisObject);

  /// Set angle.
  FEATURESAPI_EXPORT
  void setAngle(const ModelHighAPI_Double& theAngle);
};

/// Pointer on Rotation object.
typedef std::shared_ptr<FeaturesAPI_Rotation> RotationPtr;

/// \ingroup CPPHighAPI
/// \brief Create Rotation feature.
FEATURESAPI_EXPORT
RotationPtr addRotation(const std::shared_ptr<ModelAPI_Document> & thePart,
                        const std::list<ModelHighAPI_Selection>& theMainObjects,
                        const ModelHighAPI_Selection& theAxisObject,
                        const ModelHighAPI_Double& theDistance);

#endif // FeaturesAPI_Rotation_H_
