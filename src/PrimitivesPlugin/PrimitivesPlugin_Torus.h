// Copyright (C) 2014-201x CEA/DEN, EDF R&D

// File:        PrimitivesPlugin_Torus.h
// Created:     17 Mar 2017
// Author:      Clarisse Genrault (CEA)

#ifndef PRIMITIVESPLUGIN_TORUS_H_
#define PRIMITIVESPLUGIN_TORUS_H_

#include <PrimitivesPlugin.h>
#include <ModelAPI_Feature.h>
#include <GeomAlgoAPI_Torus.h>

/**\class PrimitivesPlugin_Torus
 * \ingroup Plugins
 * \brief Feature for creation of a torus.
 *
 * Creates a torus from a 
 */
class PrimitivesPlugin_Torus : public ModelAPI_Feature
{
 public:
  /// Torus kind
  inline static const std::string& ID()
  {
    static const std::string MY_TORUS_ID("Torus");
    return MY_TORUS_ID;
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

  /// Attribute name of the radius
  inline static const std::string& RADIUS_ID()
  {
    static const std::string MY_RADIUS_ID("radius");
    return MY_RADIUS_ID;
  }

  /// Attribute name of the section radius
  inline static const std::string& RING_RADIUS_ID()
  {
    static const std::string MY_RING_RADIUS_ID("ring_radius");
    return MY_RING_RADIUS_ID;
  }

  /// Returns the kind of a feature
  PRIMITIVESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = PrimitivesPlugin_Torus::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed
  PRIMITIVESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  PRIMITIVESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  PrimitivesPlugin_Torus();

 private:
  /// Load Naming data structure of the feature to the document
  void loadNamingDS(std::shared_ptr<GeomAlgoAPI_Torus> theTorusAlgo,
                    std::shared_ptr<ModelAPI_ResultBody> theResultTorus);

};

#endif // PRIMITIVESPLUGIN_TORUS_H_
