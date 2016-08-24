// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_Union.h
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#ifndef FeaturesAPI_Union_H_
#define FeaturesAPI_Union_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Union.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Dumper;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_Union
/// \ingroup CPPHighAPI
/// \brief Interface for Union feature.
class FeaturesAPI_Union: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Union(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Union(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Union();

  INTERFACE_1(FeaturesPlugin_Union::ID(),
              baseObjects, FeaturesPlugin_Union::BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList, /** Base objects */)

  /// Modify base attribute of the feature.
  FEATURESAPI_EXPORT
  void setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Union object.
typedef std::shared_ptr<FeaturesAPI_Union> UnionPtr;

/// \ingroup CPPHighAPI
/// \brief Create Union feature.
FEATURESAPI_EXPORT
UnionPtr addUnion(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const std::list<ModelHighAPI_Selection>& theBaseObjects);

#endif // FeaturesAPI_Union_H_
