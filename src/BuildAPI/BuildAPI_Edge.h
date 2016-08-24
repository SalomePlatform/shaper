// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        BuildAPI_Edge.h
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#ifndef BuildAPI_Edge_H_
#define BuildAPI_Edge_H_

#include "BuildAPI.h"

#include <BuildPlugin_Edge.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Selection;

/// \class BuildAPI_Edge
/// \ingroup CPPHighAPI
/// \brief Interface for Edge feature.
class BuildAPI_Edge: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  BUILDAPI_EXPORT
  explicit BuildAPI_Edge(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  BUILDAPI_EXPORT
  explicit BuildAPI_Edge(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                         const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Destructor.
  BUILDAPI_EXPORT
  virtual ~BuildAPI_Edge();

  INTERFACE_1(BuildPlugin_Edge::ID(),
              baseObjects, BuildPlugin_Edge::BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList, /** Base objects */)

  /// Modify base attribute of the feature.
  BUILDAPI_EXPORT
  void setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Dump wrapped feature
  BUILDAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Edge object.
typedef std::shared_ptr<BuildAPI_Edge> EdgePtr;

/// \ingroup CPPHighAPI
/// \brief Create Edge feature.
BUILDAPI_EXPORT
EdgePtr addEdge(const std::shared_ptr<ModelAPI_Document>& thePart,
                const std::list<ModelHighAPI_Selection>& theBaseObjects);

#endif // BuildAPI_Edge_H_
