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

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Translation();

  INTERFACE_3(FeaturesPlugin_Translation::ID(),
              mainObjects, FeaturesPlugin_Translation::OBJECTS_LIST_ID(),
              ModelAPI_AttributeSelectionList, /** Main objects */,
              axisObject, FeaturesPlugin_Translation::AXIS_OBJECT_ID(),
              ModelAPI_AttributeSelection, /** Axis object */,
              distance, FeaturesPlugin_Translation::DISTANCE_ID(),
              ModelAPI_AttributeDouble, /** Distance */)

  /// Set main objects.
  FEATURESAPI_EXPORT
  void setMainObjects(const std::list<ModelHighAPI_Selection>& theMainObjects);

  /// Set axis object.
  FEATURESAPI_EXPORT
  void setAxisObject(const ModelHighAPI_Selection& theAxisObject);

  /// Set distance.
  FEATURESAPI_EXPORT
  void setDistance(const ModelHighAPI_Double& theDistance);

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

#endif // FeaturesAPI_Translation_H_
