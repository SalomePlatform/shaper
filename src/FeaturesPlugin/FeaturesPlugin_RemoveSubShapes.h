// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_RemoveSubShapes.h
// Created:     23 May 2016
// Author:      Dmitry Bobylev

#ifndef FeaturesPlugin_RemoveSubShapes_H_
#define FeaturesPlugin_RemoveSubShapes_H_

#include "FeaturesPlugin.h"

#include <ModelAPI_Feature.h>

/// \class FeaturesPlugin_RemoveSubShapes
/// \ingroup Plugins
/// \brief Feature for removing sub-shapes from collections(wire, shell, compsolid, compound).
class FeaturesPlugin_RemoveSubShapes: public ModelAPI_Feature
{
public:
  /// Use plugin manager for features creation
  FeaturesPlugin_RemoveSubShapes();

  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Remove_SubShapes");
    return MY_ID;
  }

  /// Attribute name of base shape.
  inline static const std::string& BASE_SHAPE_ID()
  {
    static const std::string MY_BASE_SHAPE_ID("base_shape");
    return MY_BASE_SHAPE_ID;
  }

  /// Attribute name of sub-shapes.
  inline static const std::string& SUBSHAPES_ID()
  {
    static const std::string MY_SUBSHAPES_ID("subshapes");
    return MY_SUBSHAPES_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_RemoveSubShapes::ID();
    return MY_KIND;
  }

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Called on change of any argument-attribute of this object.
  /// \param[in] theID identifier of changed attribute.
  FEATURESPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Creates a new part document if needed.
  FEATURESPLUGIN_EXPORT virtual void execute();
};

#endif
