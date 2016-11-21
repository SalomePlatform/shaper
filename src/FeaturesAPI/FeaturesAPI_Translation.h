// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_Translation.h
// Created:     07 June 2016
// Author:      Dmitry Bobylev

#ifndef FeaturesAPI_Translation_H_
#define FeaturesAPI_Translation_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Translation.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Double;
class ModelHighAPI_Dumper;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_Translation
/// \ingroup CPPHighAPI
/// \brief Interface for Translation feature.
class FeaturesAPI_Translation: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Translation(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  FeaturesAPI_Translation(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                          const std::list<ModelHighAPI_Selection>& theMainObjects,
                          const ModelHighAPI_Selection& theAxisObject,
                          const ModelHighAPI_Double& theDistance);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  FeaturesAPI_Translation(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                          const std::list<ModelHighAPI_Selection>& theMainObjects,
                          const ModelHighAPI_Double& theDx,
                          const ModelHighAPI_Double& theDy,
                          const ModelHighAPI_Double& theDz);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Translation();

  INTERFACE_7(FeaturesPlugin_Translation::ID(),
              creationMethod, FeaturesPlugin_Translation::CREATION_METHOD(),
              ModelAPI_AttributeString, /** Creation method */,
              mainObjects, FeaturesPlugin_Translation::OBJECTS_LIST_ID(),
              ModelAPI_AttributeSelectionList, /** Main objects */,
              axisObject, FeaturesPlugin_Translation::AXIS_OBJECT_ID(),
              ModelAPI_AttributeSelection, /** Axis object */,
              distance, FeaturesPlugin_Translation::DISTANCE_ID(),
              ModelAPI_AttributeDouble, /** Distance */,
              dx, FeaturesPlugin_Translation::DX_ID(),
              ModelAPI_AttributeDouble, /** Dimension in X */,
              dy, FeaturesPlugin_Translation::DY_ID(),
              ModelAPI_AttributeDouble, /** Dimension in Y */,
              dz, FeaturesPlugin_Translation::DZ_ID(),
              ModelAPI_AttributeDouble, /** Dimension in Z */
             )

  /// Set main objects.
  FEATURESAPI_EXPORT
  void setMainObjects(const std::list<ModelHighAPI_Selection>& theMainObjects);

  /// Modify CreationMethod, axis_object, distance attributes of the feature.
  FEATURESAPI_EXPORT
  void setAxisAndDistance(const ModelHighAPI_Selection& theAxisObject,
                          const ModelHighAPI_Double& theDistance);

  /// Modify CreationMethod, dx, dy, dz attributes of the feature.
  FEATURESAPI_EXPORT
  void setDimensions(const ModelHighAPI_Double& theDx,
                     const ModelHighAPI_Double& theDy,
                     const ModelHighAPI_Double& theDz);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Translation object.
typedef std::shared_ptr<FeaturesAPI_Translation> TranslationPtr;

/// \ingroup CPPHighAPI
/// \brief Create Translation feature.
FEATURESAPI_EXPORT
TranslationPtr addTranslation(const std::shared_ptr<ModelAPI_Document>& thePart,
                              const std::list<ModelHighAPI_Selection>& theMainObjects,
                              const ModelHighAPI_Selection& theAxisObject,
                              const ModelHighAPI_Double& theDistance);

/// \ingroup CPPHighAPI
/// \brief Create Translation feature.
FEATURESAPI_EXPORT
TranslationPtr addTranslation(const std::shared_ptr<ModelAPI_Document>& thePart,
                              const std::list<ModelHighAPI_Selection>& theMainObjects,
                              const ModelHighAPI_Double& theDx,
                              const ModelHighAPI_Double& theDy,
                              const ModelHighAPI_Double& theDz);

#endif // FeaturesAPI_Translation_H_
