// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_Pipe.h
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#ifndef FeaturesAPI_Pipe_H_
#define FeaturesAPI_Pipe_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Pipe.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Dumper;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_Pipe
/// \ingroup CPPHighAPI
/// \brief Interface for Pipe feature.
class FeaturesAPI_Pipe: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Pipe(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Pipe(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                            const std::list<ModelHighAPI_Selection>& theBaseObjects,
                            const ModelHighAPI_Selection& thePath);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Pipe(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                            const std::list<ModelHighAPI_Selection>& theBaseObjects,
                            const ModelHighAPI_Selection& thePath,
                            const ModelHighAPI_Selection& theBiNoramal);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Pipe(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                            const std::list<ModelHighAPI_Selection>& theBaseObjects,
                            const ModelHighAPI_Selection& thePath,
                            const std::list<ModelHighAPI_Selection>& theLocations);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Pipe();

  INTERFACE_5(FeaturesPlugin_Pipe::ID(),
              baseObjects, FeaturesPlugin_Pipe::BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList, /** Base objects */,
              path, FeaturesPlugin_Pipe::PATH_OBJECT_ID(), ModelAPI_AttributeSelection, /** Path */,
              creationMethod, FeaturesPlugin_Pipe::CREATION_METHOD(), ModelAPI_AttributeString, /** Creation method */,
              biNormal, FeaturesPlugin_Pipe::BINORMAL_ID(), ModelAPI_AttributeSelection, /** Bi-Normal */,
              locations, FeaturesPlugin_Pipe::LOCATIONS_ID(), ModelAPI_AttributeSelectionList, /** Locations */)

  /// Modify base attribute of the feature.
  FEATURESAPI_EXPORT
  void setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Modify path attribute of the feature.
  FEATURESAPI_EXPORT
  void setPath(const ModelHighAPI_Selection& thePath);

  /// Modify creation method, base and path.
  FEATURESAPI_EXPORT
  void setByBasePath(const std::list<ModelHighAPI_Selection>& theBaseObjects,
                     const ModelHighAPI_Selection& thePath);

  /// Modify creation method, base, path and bi-normal.
  FEATURESAPI_EXPORT
  void setByBasePathBiNormal(const std::list<ModelHighAPI_Selection>& theBaseObjects,
                             const ModelHighAPI_Selection& thePath,
                             const ModelHighAPI_Selection& theBiNoramal);

  /// Modify creation method, base, path and locations.
  FEATURESAPI_EXPORT
  void setByBasePathLocations(const std::list<ModelHighAPI_Selection>& theBaseObjects,
                              const ModelHighAPI_Selection& thePath,
                              const std::list<ModelHighAPI_Selection>& theLocations);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Pipe object.
typedef std::shared_ptr<FeaturesAPI_Pipe> PipePtr;

/// \ingroup CPPHighAPI
/// \brief Create Pipe feature.
FEATURESAPI_EXPORT
PipePtr addPipe(const std::shared_ptr<ModelAPI_Document>& thePart,
                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                const ModelHighAPI_Selection& thePath);

/// \ingroup CPPHighAPI
/// \brief Create Pipe feature.
FEATURESAPI_EXPORT
PipePtr addPipe(const std::shared_ptr<ModelAPI_Document>& thePart,
                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                const ModelHighAPI_Selection& thePath,
                const ModelHighAPI_Selection& theBiNoramal);

/// \ingroup CPPHighAPI
/// \brief Create Pipe feature.
FEATURESAPI_EXPORT
PipePtr addPipe(const std::shared_ptr<ModelAPI_Document>& thePart,
                const std::list<ModelHighAPI_Selection>& theBaseObjects,
                const ModelHighAPI_Selection& thePath,
                const std::list<ModelHighAPI_Selection>& theLocations);

#endif // FeaturesAPI_Pipe_H_
