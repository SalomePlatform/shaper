// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        BuildPlugin_SubShapes.h
// Created:     14 April 2016
// Author:      Dmitry Bobylev

#ifndef BuildPlugin_SubShapes_H_
#define BuildPlugin_SubShapes_H_

#include "BuildPlugin.h"

#include <ModelAPI_Feature.h>

/// \class BuildPlugin_SubShapes
/// \ingroup Plugins
/// \brief Feature for adding or removing sub-shapes from shape.
class BuildPlugin_SubShapes: public ModelAPI_Feature
{
public:
  /// Use plugin manager for features creation
  BuildPlugin_SubShapes();

  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("SubShapes");
    return MY_ID;
  }

  /// Attribute name of base shape.
  inline static const std::string& BASE_SHAPE_ID()
  {
    static const std::string MY_BASE_SHAPE_ID("base_shape");
    return MY_BASE_SHAPE_ID;
  }

  /// Attribute name of sub-shapes.
  inline static const std::string& SUB_SHAPES_ID()
  {
    static const std::string MY_SUB_SHAPES_ID("sub_shapes");
    return MY_SUB_SHAPES_ID;
  }

  /// \return the kind of a feature.
  BUILDPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = BuildPlugin_SubShapes::ID();
    return MY_KIND;
  }

  /// Request for initialization of data model of the feature: adding all attributes.
  BUILDPLUGIN_EXPORT virtual void initAttributes();

  /// Called on change of any argument-attribute of this object.
  /// \param[in] theID identifier of changed attribute.
  BUILDPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Creates a new part document if needed.
  BUILDPLUGIN_EXPORT virtual void execute();
};

#endif
