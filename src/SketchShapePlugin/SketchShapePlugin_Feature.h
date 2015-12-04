// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchShapePlugin_Feature.h
// Created:     25 Nov 2015
// Author:      Natalia ERMOLAEVA

#ifndef SKETCHSHAPEPLUGIN_FEATURE_H_
#define SKETCHSHAPEPLUGIN_FEATURE_H_

#include "SketchShapePlugin.h"

#include <ModelAPI_Feature.h>

/**\class SketchShapePlugin_Feature
 * \ingroup Plugins
 * \brief Feature for extraction shapes from the sketch.
 * The visualization of this object is a result and
 * it is calculated if all attributes are initialized.
 * It is possible to extract simultaneously vertices, edges and faces.
 * 
 */
class SketchShapePlugin_Feature : public ModelAPI_Feature
{
 public:
  /// SketchShape feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_SKETCH_SHAPE_ID("SketchShape");
    return MY_SKETCH_SHAPE_ID;
  }

  /// Sketch feature
  inline static const std::string& SKETCH_ID()
  {
    static const std::string MY_SKETCH_ID("Sketch");
    return MY_SKETCH_ID;
  }

  /// List of vertices to be extracted
  inline static const std::string& VERTEX_LIST_ID()
  {
    static const std::string MY_VERTEX_LIST_ID("VertexList");
    return MY_VERTEX_LIST_ID;
  }

  /// State whether the vertices are selectable
  inline static const std::string& VERTEX_CHOICE_ID()
  {
    static const std::string MY_VERTEX_CHOICE_ID("VertexChoice");
    return MY_VERTEX_CHOICE_ID;
  }

  /// Returns the kind of a feature
  SKETCHSHAPEPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchShapePlugin_Feature::ID();
    return MY_KIND;
  }

  /// Creates an arc-shape
  SKETCHSHAPEPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  SKETCHSHAPEPLUGIN_EXPORT virtual void initAttributes();

  /// Called on change of any argument-attribute of this object
  /// \param theID identifier of changed attribute
  SKETCHSHAPEPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Use plugin manager for features creation
  SketchShapePlugin_Feature();
};

#endif
