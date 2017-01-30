// Copyright (C) 2014-201x CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_Scale.h
// Created:     24 Jan 2017
// Author:      Clarisse Genrault (CEA)

#ifndef FEATURESAPI_SCALE_H_
#define FEATURESAPI_SCALE_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Scale.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Double;
class ModelHighAPI_Dumper;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_Scale
/// \ingroup CPPHighAPI
/// \brief Interface for Scale feature.
class FeaturesAPI_Scale: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Scale(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Scale(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const std::list<ModelHighAPI_Selection>& theMainObjects,
                             const ModelHighAPI_Selection& theCenterPoint,
                             const ModelHighAPI_Double& theScaleFactor);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Scale();

  INTERFACE_3(FeaturesPlugin_Scale::ID(),
              mainObjects, FeaturesPlugin_Scale::OBJECTS_LIST_ID(),
              ModelAPI_AttributeSelectionList, /** Main objects */,
              centerPoint, FeaturesPlugin_Scale::CENTER_POINT_ID(),
              ModelAPI_AttributeSelection, /** Center point */,
              scaleFactor, FeaturesPlugin_Scale::SCALE_FACTOR_ID(),
              ModelAPI_AttributeDouble, /** Scale factor */)

  /// Set main objects.
  FEATURESAPI_EXPORT
  void setMainObjects(const std::list<ModelHighAPI_Selection>& theMainObjects);

  /// Modify center_point attribute of the feature.
  FEATURESAPI_EXPORT
  void setCenterPoint(const ModelHighAPI_Selection& theCenterPoint);

  /// Modify Creation Method and scale_factor attribute of the feature.
  FEATURESAPI_EXPORT
  void setScaleFactor(const ModelHighAPI_Double& theScaleFactor);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Scale object.
typedef std::shared_ptr<FeaturesAPI_Scale> ScalePtr;

/// \ingroup CPPHighAPI
/// \brief Create Scale feature.
FEATURESAPI_EXPORT
ScalePtr addScale(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const std::list<ModelHighAPI_Selection>& theMainObjects,
                  const ModelHighAPI_Selection& theCenterPoint,
                  const ModelHighAPI_Double& theScaleFactor);

#endif // FEATURESAPI_SCALE_H_