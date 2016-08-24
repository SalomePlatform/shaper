// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_Boolean.h
// Created:     07 June 2016
// Author:      Dmitry Bobylev

#ifndef FeaturesAPI_Boolean_H_
#define FeaturesAPI_Boolean_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Boolean.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Integer;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_Boolean
/// \ingroup CPPHighAPI
/// \brief Interface for Boolean feature.
class FeaturesAPI_Boolean: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Boolean(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  FeaturesAPI_Boolean(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                      const ModelHighAPI_Integer& theBoolType,
                      const std::list<ModelHighAPI_Selection>& theMainObjects,
                      const std::list<ModelHighAPI_Selection>& theToolObjects);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Boolean();

  INTERFACE_3(FeaturesPlugin_Boolean::ID(),
              boolType, FeaturesPlugin_Boolean::TYPE_ID(), ModelAPI_AttributeInteger, /** Operation type */,
              mainObjects, FeaturesPlugin_Boolean::OBJECT_LIST_ID(), ModelAPI_AttributeSelectionList, /** Main objects */,
              toolObjects, FeaturesPlugin_Boolean::TOOL_LIST_ID(), ModelAPI_AttributeSelectionList, /** Tool objects*/)

  /// Set operation type.
  FEATURESAPI_EXPORT
  void setBoolType(const ModelHighAPI_Integer& theBoolType);

  /// Set main objects.
  FEATURESAPI_EXPORT
  void setMainObjects(const std::list<ModelHighAPI_Selection>& theMainObjects);

  /// Set tool objects.
  FEATURESAPI_EXPORT
  void setToolObjects(const std::list<ModelHighAPI_Selection>& theToolObjects);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Boolean object.
typedef std::shared_ptr<FeaturesAPI_Boolean> BooleanPtr;

/// \ingroup CPPHighAPI
/// \brief Create Boolean Cut feature.
FEATURESAPI_EXPORT
BooleanPtr addCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const std::list<ModelHighAPI_Selection>& theMainObjects,
                  const std::list<ModelHighAPI_Selection>& theToolObjects);

/// \ingroup CPPHighAPI
/// \brief Create Boolean Fuse feature.
FEATURESAPI_EXPORT
BooleanPtr addFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                   const std::list<ModelHighAPI_Selection>& theObjects);

/// \ingroup CPPHighAPI
/// \brief Create Boolean Fuse feature.
FEATURESAPI_EXPORT
BooleanPtr addFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                   const std::list<ModelHighAPI_Selection>& theMainObjects,
                   const std::list<ModelHighAPI_Selection>& theToolObjects);

/// \ingroup CPPHighAPI
/// \brief Create Boolean Common feature.
FEATURESAPI_EXPORT
BooleanPtr addCommon(const std::shared_ptr<ModelAPI_Document>& thePart,
                     const std::list<ModelHighAPI_Selection>& theMainObjects,
                     const std::list<ModelHighAPI_Selection>& theToolObjects);

/// \ingroup CPPHighAPI
/// \brief Create Boolean Smash feature.
FEATURESAPI_EXPORT
BooleanPtr addSmash(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const std::list<ModelHighAPI_Selection>& theMainObjects,
                    const std::list<ModelHighAPI_Selection>& theToolObjects);

/// \ingroup CPPHighAPI
/// \brief Create Boolean Fill feature.
FEATURESAPI_EXPORT
BooleanPtr addFill(const std::shared_ptr<ModelAPI_Document>& thePart,
                   const std::list<ModelHighAPI_Selection>& theMainObjects,
                   const std::list<ModelHighAPI_Selection>& theToolObjects);

#endif // FeaturesAPI_Boolean_H_
