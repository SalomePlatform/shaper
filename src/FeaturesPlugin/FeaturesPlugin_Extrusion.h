// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

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

/**\class FeaturesPlugin_Extrusion
 * \ingroup Plugins
 * \brief Feature for creation of extrusion from the planar face.
 *
 * Extrusion creates the lateral faces based on edges of the base face and
 * the top face equal to the base face. Direction of extrusion is taken from the face
 * plane, but can be corrected by the "reverse" flag.
 */
class FeaturesPlugin_Extrusion : public ModelAPI_Feature
{
 public:
  /// Extrusion kind
  inline static const std::string& ID()
  {
    static const std::string MY_EXTRUSION_ID("Extrusion");
    return MY_EXTRUSION_ID;
  }
  /// attribute name of references sketch entities list, it should contain a sketch result or
  /// a pair a sketch result to sketch face
  inline static const std::string& LIST_ID()
  {
    static const std::string MY_GROUP_LIST_ID("base");
    return MY_GROUP_LIST_ID;
  }

  /// attribute name of an object to which the extrusion grows
  inline static const std::string& AXIS_OBJECT_ID()
  {
    static const std::string MY_TO_OBJECT_ID("axis_object");
    return MY_TO_OBJECT_ID;
  }

  /// attribute name of extrusion size
  inline static const std::string& TO_SIZE_ID()
  {
    static const std::string MY_TO_SIZE_ID("to_size");
    return MY_TO_SIZE_ID;
  }

  /// attribute name of extrusion size
  inline static const std::string& FROM_SIZE_ID()
  {
    static const std::string MY_FROM_SIZE_ID("from_size");
    return MY_FROM_SIZE_ID;
  }

  /// attribute name of an object to which the extrusion grows
  inline static const std::string& TO_OBJECT_ID()
  {
    static const std::string MY_TO_OBJECT_ID("to_object");
    return MY_TO_OBJECT_ID;
  }

  /// attribute name of tool object
  inline static const std::string& FROM_OBJECT_ID()
  {
    static const std::string MY_FROM_OBJECT_ID("from_object");
    return MY_FROM_OBJECT_ID;
  }
  /// attribute name of reverse direction
 inline static const std::string& REVERSE_ID()
 {
   static const std::string MY_REVERSE_ID("reverse");
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
private:
  /// Load Naming data structure of the feature to the document
  void LoadNamingDS(GeomAlgoAPI_Extrusion& theFeature, std::shared_ptr<ModelAPI_ResultBody> theResultBody,
	                std::shared_ptr<GeomAPI_Shape> theBasis,
	                std::shared_ptr<GeomAPI_Shape> theContext);

  /// Set an empty shape to the result of extrusion
  void clearResult();
};

#endif
