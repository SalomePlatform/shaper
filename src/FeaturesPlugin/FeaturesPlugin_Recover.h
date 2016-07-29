// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Recover.h
// Created:     29 Jul 2016
// Author:      Natalia ERMOLAEVA

#ifndef FEATURESPLUGIN_RECOVER_H_
#define FEATURESPLUGIN_RECOVER_H_

#include "FeaturesPlugin.h"
#include <ModelAPI_Feature.h>
#include <GeomAPI_Shape.h>

/**\class FeaturesPlugin_Recover
 * \ingroup Plugins
 * \brief Feature for display of concealed attributes of some feature.
 */
class FeaturesPlugin_Recover : public ModelAPI_Feature
{
 public:
  /// Extrusion kind
  inline static const std::string& ID()
  {
    static const std::string MY_GROUP_ID("Recover");
    return MY_GROUP_ID;
  }
  /// Attribute name of base shape.
  inline static const std::string& BASE_SHAPE_ID()
  {
    static const std::string MY_BASE_SHAPE_ID("base_shape");
    return MY_BASE_SHAPE_ID;
  }

  /// Attribute name of base shape.
  inline static const std::string& RECOVERED_ENTITIES()
  {
    static const std::string MY_RECOVERED_ENTITIES_ID("recovered_entities");
    return MY_RECOVERED_ENTITIES_ID;
  }

  /// Returns the kind of a feature
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Recover::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  FeaturesPlugin_Recover();

};

#endif
