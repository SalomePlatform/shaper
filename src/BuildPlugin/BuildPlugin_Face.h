// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        BuildPlugin_Face.h
// Created:     14 April 2016
// Author:      Dmitry Bobylev

#ifndef BuildPlugin_Face_H_
#define BuildPlugin_Face_H_

#include "BuildPlugin.h"

#include <ModelAPI_Feature.h>

/// \class BuildPlugin_Face
/// \ingroup Plugins
/// \brief Feature for creation of face from sketch edges or existing wires.
class BuildPlugin_Face: public ModelAPI_Feature
{
public:
  /// Use plugin manager for features creation
  BuildPlugin_Face();

  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Face");
    return MY_ID;
  }

  /// Attribute name of base objects.
  inline static const std::string& BASE_OBJECTS_ID()
  {
    static const std::string MY_BASE_OBJECTS_ID("base_objects");
    return MY_BASE_OBJECTS_ID;
  }

  /// \return the kind of a feature.
  BUILDPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = BuildPlugin_Face::ID();
    return MY_KIND;
  }

  /// Request for initialization of data model of the feature: adding all attributes.
  BUILDPLUGIN_EXPORT virtual void initAttributes();

  /// Creates a new part document if needed.
  BUILDPLUGIN_EXPORT virtual void execute();
};

#endif
