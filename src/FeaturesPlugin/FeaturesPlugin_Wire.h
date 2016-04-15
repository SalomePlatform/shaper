// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_Wire.h
// Created:     14 April 2016
// Author:      Dmitry Bobylev

#ifndef FeaturesPlugin_Wire_H_
#define FeaturesPlugin_Wire_H_

#include "FeaturesPlugin.h"

#include <ModelAPI_Feature.h>

/// \class FeaturesPlugin_Wire
/// \ingroup Plugins
/// \brief Feature for creation of wire from sketch edges or existing wires.
class FeaturesPlugin_Wire: public ModelAPI_Feature
{
public:
  /// Use plugin manager for features creation
  FeaturesPlugin_Wire();

  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Wire");
    return MY_ID;
  }

  /// Attribute name of base objects.
  inline static const std::string& BASE_OBJECTS_ID()
  {
    static const std::string MY_BASE_OBJECTS_ID("base_objects");
    return MY_BASE_OBJECTS_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Wire::ID();
    return MY_KIND;
  }

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Creates a new part document if needed.
  FEATURESPLUGIN_EXPORT virtual void execute();
};

#endif
