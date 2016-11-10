// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_RemoveSubShapes.h
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#ifndef FeaturesAPI_RemoveSubShapes_H_
#define FeaturesAPI_RemoveSubShapes_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_RemoveSubShapes.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Dumper;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_RemoveSubShapes
/// \ingroup CPPHighAPI
/// \brief Interface for RemoveSubShapes feature.
class FeaturesAPI_RemoveSubShapes: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_RemoveSubShapes(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_RemoveSubShapes(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                       const ModelHighAPI_Selection& theBase);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_RemoveSubShapes();

  INTERFACE_2(FeaturesPlugin_RemoveSubShapes::ID(),
              base, FeaturesPlugin_RemoveSubShapes::BASE_SHAPE_ID(),
              ModelAPI_AttributeSelection, /** Base */,
              subshapes, FeaturesPlugin_RemoveSubShapes::SUBSHAPES_ID(),
              ModelAPI_AttributeSelectionList, /** Subshapes */)

  /// Modify objects attribute of the feature.
  FEATURESAPI_EXPORT
  void setBase(const ModelHighAPI_Selection& theBase);

  /// Modify tools attribute of the feature.
  FEATURESAPI_EXPORT
  void setSubShapesToKeep(const std::list<ModelHighAPI_Selection>& theSubShapes);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on RemoveSubShapes object.
typedef std::shared_ptr<FeaturesAPI_RemoveSubShapes> RemoveSubShapesPtr;

/// \ingroup CPPHighAPI
/// \brief Create RemoveSubShapes feature.
FEATURESAPI_EXPORT
RemoveSubShapesPtr addRemoveSubShapes(const std::shared_ptr<ModelAPI_Document>& thePart,
                                      const ModelHighAPI_Selection& theBase);

#endif // FeaturesAPI_RemoveSubShapes_H_
