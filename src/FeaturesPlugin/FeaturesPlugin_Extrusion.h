// File:        FeaturesPlugin_Extrusion.h
// Created:     30 May 2014
// Author:      Vitaly SMETANNIKOV

#ifndef FeaturesPlugin_Extrusion_HeaderFile
#define FeaturesPlugin_Extrusion_HeaderFile

#include "FeaturesPlugin.h"
#include <ModelAPI_Feature.h>

/// Extrusion kind
const std::string FEATURES_EXTRUSION_KIND("Extrusion");

/// attribute name of referenced face
const std::string EXTRUSION_FACE = "extrusion_face";

/// attribute name of extrusion size
const std::string EXTRUSION_SIZE = "extrusion_size";

/// attribute name of reverse direction
const std::string EXTRUSION_REVERSE = "extrusion_reverse";


class FeaturesPlugin_Extrusion: public ModelAPI_Feature
{
public:
  /// Returns the kind of a feature
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind() 
  { static std::string MY_KIND = FEATURES_EXTRUSION_KIND; return MY_KIND; }

  /// Creates a new part document if needed
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  FeaturesPlugin_Extrusion();
};

#endif
