// File:        FeaturesPlugin_Extrusion.h
// Created:     30 May 2014
// Author:      Vitaly SMETANNIKOV

#ifndef FeaturesPlugin_Extrusion_H_
#define FeaturesPlugin_Extrusion_H_

#include "FeaturesPlugin.h"
#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultBody.h>
#include <GeomAlgoAPI_Extrusion.h>
#include <GeomAPI_Shape.h>
class FeaturesPlugin_Extrusion : public ModelAPI_Feature
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
    static const std::string MY_FACE_ID("extrusion_face");
    return MY_FACE_ID;
  }
  /// attribute name of extrusion size
  inline static const std::string& SIZE_ID()
  {
    static const std::string MY_SIZE_ID("extrusion_size");
    return MY_SIZE_ID;
  }
  /// attribute name of reverse direction
  inline static const std::string& REVERSE_ID()
  {
    static const std::string MY_REVERSE_ID("extrusion_reverse");
    return MY_REVERSE_ID;
  }

  /// Returns the kind of a feature
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Extrusion::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  FeaturesPlugin_Extrusion();

  /// Load Naming data structure of the feature to the document
  void LoadNamingDS(GeomAlgoAPI_Extrusion& theFeature, boost::shared_ptr<ModelAPI_ResultBody> theResultBody,
	                boost::shared_ptr<GeomAPI_Shape> theBasis,
	                boost::shared_ptr<GeomAPI_Shape> theContext);
};

#endif
