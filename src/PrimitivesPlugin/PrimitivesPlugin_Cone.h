// Copyright (C) 2014-201x CEA/DEN, EDF R&D

// File:        PrimitivesPlugin_Cone.h
// Created:     17 Mar 2017
// Author:      Clarisse Genrault (CEA)

#ifndef PRIMITIVESPLUGIN_CONE_H_
#define PRIMITIVESPLUGIN_CONE_H_

#include <PrimitivesPlugin.h>
#include <ModelAPI_Feature.h>
#include <GeomAlgoAPI_Cone.h>

/**\class PrimitivesPlugin_Cone
 * \ingroup Plugins
 * \brief Feature for creation of a cone.
 *
 * Creates a cone from a 
 */
class PrimitivesPlugin_Cone : public ModelAPI_Feature
{
 public:
  /// Cone kind
  inline static const std::string& ID()
  {
    static const std::string MY_CONE_ID("Cone");
    return MY_CONE_ID;
  }

  /// Attribute name of the base point
  inline static const std::string& BASE_POINT_ID()
  {
    static const std::string MY_BASE_POINT_ID("base_point");
    return MY_BASE_POINT_ID;
  }

  /// Attribute name of the axis
  inline static const std::string& AXIS_ID()
  {
    static const std::string MY_AXIS_ID("axis");
    return MY_AXIS_ID;
  }

  /// Attribute name of the base radius
  inline static const std::string& BASE_RADIUS_ID()
  {
    static const std::string MY_BASE_RADIUS_ID("base_radius");
    return MY_BASE_RADIUS_ID;
  }

  /// Attribute name of the radius
  inline static const std::string& TOP_RADIUS_ID()
  {
    static const std::string MY_TOP_RADIUS_ID("top_radius");
    return MY_TOP_RADIUS_ID;
  }

  /// Attribute name of the radius
  inline static const std::string& HEIGHT_ID()
  {
    static const std::string MY_HEIGHT_ID("height");
    return MY_HEIGHT_ID;
  }

  /// Returns the kind of a feature
  PRIMITIVESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = PrimitivesPlugin_Cone::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed
  PRIMITIVESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  PRIMITIVESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  PrimitivesPlugin_Cone();

 private:
  /// Load Naming data structure of the feature to the document
  void loadNamingDS(std::shared_ptr<GeomAlgoAPI_Cone> theConeAlgo,
                    std::shared_ptr<ModelAPI_ResultBody> theResultCone);

};

#endif // PRIMITIVESPLUGIN_CONE_H_
