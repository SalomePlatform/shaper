// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        BuildAPI_Face.h
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#ifndef BuildAPI_Face_H_
#define BuildAPI_Face_H_

#include "BuildAPI.h"

#include <BuildPlugin_Face.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Selection;

/// \class BuildAPI_Face
/// \ingroup CPPHighAPI
/// \brief Interface for Face feature.
class BuildAPI_Face: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  BUILDAPI_EXPORT
  explicit BuildAPI_Face(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  BUILDAPI_EXPORT
  explicit BuildAPI_Face(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                         const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Destructor.
  BUILDAPI_EXPORT
  virtual ~BuildAPI_Face();

  INTERFACE_1(BuildPlugin_Face::ID(),
              baseObjects, BuildPlugin_Face::BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList, /** Base objects */)

  /// Modify base attribute of the feature.
  BUILDAPI_EXPORT
  void setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Dump wrapped feature
  BUILDAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Face object.
typedef std::shared_ptr<BuildAPI_Face> FacePtr;

/// \ingroup CPPHighAPI
/// \brief Create Face feature.
BUILDAPI_EXPORT
FacePtr addFace(const std::shared_ptr<ModelAPI_Document>& thePart,
                const std::list<ModelHighAPI_Selection>& theBaseObjects);

#endif // BuildAPI_Face_H_
