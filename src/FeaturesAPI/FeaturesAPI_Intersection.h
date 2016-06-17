// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_Intersection.h
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#ifndef FeaturesAPI_Intersection_H_
#define FeaturesAPI_Intersection_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Intersection.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Selection;

/// \class FeaturesAPI_Intersection
/// \ingroup CPPHighAPI
/// \brief Interface for Intersection feature.
class FeaturesAPI_Intersection: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Intersection(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Intersection(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                    const std::list<ModelHighAPI_Selection>& theObjects,
                                    const std::list<ModelHighAPI_Selection>& theTools);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Intersection();

  INTERFACE_2(FeaturesPlugin_Intersection::ID(),
              objects, FeaturesPlugin_Intersection::OBJECT_LIST_ID(), ModelAPI_AttributeSelectionList, /** Objects */,
              tools, FeaturesPlugin_Intersection::TOOL_LIST_ID(), ModelAPI_AttributeSelectionList, /** Tools */)

  /// Modify objects attribute of the feature.
  FEATURESAPI_EXPORT
  void setObjects(const std::list<ModelHighAPI_Selection>& theObjects);

  /// Modify tools attribute of the feature.
  FEATURESAPI_EXPORT
  void setTools(const std::list<ModelHighAPI_Selection>& theTools);
};

/// Pointer on Intersection object.
typedef std::shared_ptr<FeaturesAPI_Intersection> IntersectionPtr;

/// \ingroup CPPHighAPI
/// \brief Create Intersection feature.
FEATURESAPI_EXPORT
IntersectionPtr addIntersection(const std::shared_ptr<ModelAPI_Document>& thePart,
                                const std::list<ModelHighAPI_Selection>& theObjects,
                                const std::list<ModelHighAPI_Selection>& theTools);

#endif // FeaturesAPI_Intersection_H_
