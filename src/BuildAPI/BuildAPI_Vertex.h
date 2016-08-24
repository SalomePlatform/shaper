// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        BuildAPI_Vertex.h
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#ifndef BuildAPI_Vertex_H_
#define BuildAPI_Vertex_H_

#include "BuildAPI.h"

#include <BuildPlugin_Vertex.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Selection;

/// \class BuildAPI_Vertex
/// \ingroup CPPHighAPI
/// \brief Interface for Vertex feature.
class BuildAPI_Vertex: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  BUILDAPI_EXPORT
  explicit BuildAPI_Vertex(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  BUILDAPI_EXPORT
  explicit BuildAPI_Vertex(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                           const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Destructor.
  BUILDAPI_EXPORT
  virtual ~BuildAPI_Vertex();

  INTERFACE_1(BuildPlugin_Vertex::ID(),
              baseObjects, BuildPlugin_Vertex::BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList, /** Base objects */)

  /// Modify base attribute of the feature.
  BUILDAPI_EXPORT
  void setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Dump wrapped feature
  BUILDAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Vertex object.
typedef std::shared_ptr<BuildAPI_Vertex> VertexPtr;

/// \ingroup CPPHighAPI
/// \brief Create Vertex feature.
BUILDAPI_EXPORT
VertexPtr addVertex(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const std::list<ModelHighAPI_Selection>& theBaseObjects);

#endif // BuildAPI_Vertex_H_
