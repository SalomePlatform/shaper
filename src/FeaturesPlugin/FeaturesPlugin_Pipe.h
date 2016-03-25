// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Pipe.h
// Created:     16 March 2016
// Author:      Dmitry Bobylev

#ifndef FeaturesPlugin_Pipe_H_
#define FeaturesPlugin_Pipe_H_

#include "FeaturesPlugin.h"

#include <GeomAlgoAPI_Pipe.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultBody.h>

/// \class FeaturesPlugin_Pipe
/// \ingroup Plugins
/// \brief Feature for creation of extrusion along a path.
/// Pipe creates extrusion of objects along a path. It produces the following results from objects:\n
/// Vertex -> Edge\n
/// Edge -> Face\n
/// Wire -> Shell\n
/// Face -> Solid
class FeaturesPlugin_Pipe : public ModelAPI_Feature
{
public:
  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_FEATURE_ID("Pipe");
    return MY_FEATURE_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD()
  {
    static const std::string MY_CREATION_METHOD("creation_method");
    return MY_CREATION_METHOD;
  }

  /// Attribute name of base objects.
  inline static const std::string& BASE_OBJECTS_ID()
  {
    static const std::string MY_BASE_OBJECTS_ID("base_objects");
    return MY_BASE_OBJECTS_ID;
  }

  /// Attribute name of path object.
  inline static const std::string& PATH_OBJECT_ID()
  {
    static const std::string MY_PATH_OBJECT_ID("path_object");
    return MY_PATH_OBJECT_ID;
  }

  /// Attribute name of Bi-Normal.
  inline static const std::string& BINORMAL_ID()
  {
    static const std::string MY_BINORMAL_ID("binormal");
    return MY_BINORMAL_ID;
  }

  /// Attribute name of locations.
  inline static const std::string& LOCATIONS_ID()
  {
    static const std::string MY_LOCATIONS_ID("locations_objects");
    return MY_LOCATIONS_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Pipe::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  FeaturesPlugin_Pipe();

private:
  void storeResult(const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                   GeomAlgoAPI_Pipe& thePipeAlgo,
                   const int theResultIndex = 0);

  void storeResult(const ListOfShape& theBaseShapes,
                   GeomAlgoAPI_Pipe& thePipeAlgo,
                   const int theResultIndex = 0);

  void storeShapes(ResultBodyPtr theResultBody,
                   const GeomAPI_Shape::ShapeType theBaseShapeType,
                   const std::shared_ptr<GeomAPI_DataMapOfShapeShape> theMapOfSubShapes,
                   const ListOfShape& theShapes,
                   const std::string theName,
                   int& theTag);
};

#endif
