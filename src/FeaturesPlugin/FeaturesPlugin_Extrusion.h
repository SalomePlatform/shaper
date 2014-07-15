// File:        FeaturesPlugin_Extrusion.h
// Created:     30 May 2014
// Author:      Vitaly SMETANNIKOV

#ifndef FeaturesPlugin_Extrusion_HeaderFile
#define FeaturesPlugin_Extrusion_HeaderFile

#include "FeaturesPlugin.h"
#include <ModelAPI_Feature.h>

class FeaturesPlugin_Extrusion: public ModelAPI_Feature
{
public:
  /// Extrusion kind
  inline static const std::string& ID()
  {
    static const std::string MY_EXTRUSION_ID("Extrusion");
    return MY_EXTRUSION_ID;
  }
  /// attribute name of referenced face
  inline static const std::string& FACE_ID()
  {
    static const std::string MY_FACE_ID("FeaturesPlugin_Extrusion::FACE_ID()");
    return MY_FACE_ID;
  }
  /// attribute name of extrusion size
  inline static const std::string& SIZE_ID()
  {
    static const std::string MY_SIZE_ID("FeaturesPlugin_Extrusion::SIZE_ID()");
    return MY_SIZE_ID;
  }
    /// attribute name of reverse direction
  inline static const std::string& REVERSE_ID()
  {
    static const std::string MY_REVERSE_ID("FeaturesPlugin_Extrusion::REVERSE_ID()");
    return MY_REVERSE_ID;
  }

  /// Returns the kind of a feature
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind() 
  { static std::string MY_KIND = FeaturesPlugin_Extrusion::ID(); return MY_KIND; }

  /// Returns to which group in the document must be added feature
  FEATURESPLUGIN_EXPORT virtual const std::string& getGroup() 
  { static std::string MY_GROUP = "Construction";  return MY_GROUP; }

  /// Creates a new part document if needed
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  FeaturesPlugin_Extrusion();
};

#endif
