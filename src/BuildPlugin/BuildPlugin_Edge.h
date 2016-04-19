// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        BuildPlugin_Edge.h
// Created:     18 April 2016
// Author:      Dmitry Bobylev

#ifndef BuildPlugin_Edge_H_
#define BuildPlugin_Edge_H_

#include "BuildPlugin.h"

#include <ModelAPI_Feature.h>

/// \class BuildPlugin_Edge
/// \ingroup Plugins
/// \brief Feature for creation of edge from sketch edge or existing edges.
class BuildPlugin_Edge: public ModelAPI_Feature
{
public:
  /// Use plugin manager for features creation
  BuildPlugin_Edge();

  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Edge");
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
    static std::string MY_KIND = BuildPlugin_Edge::ID();
    return MY_KIND;
  }

  /// Request for initialization of data model of the feature: adding all attributes.
  BUILDPLUGIN_EXPORT virtual void initAttributes();

  /// Creates a new part document if needed.
  BUILDPLUGIN_EXPORT virtual void execute();
};

#endif
