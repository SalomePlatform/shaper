// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Placement.h
// Created:     2 Dec 2014
// Author:      Artem ZHIDKOV

#ifndef FeaturesPlugin_Placement_H_
#define FeaturesPlugin_Placement_H_

#include "FeaturesPlugin.h"
#include <ModelAPI_Feature.h>
#include <GeomAlgoAPI_Placement.h>

class ModelAPI_ResultBody;
class GeomAPI_Shape;

/**\class FeaturesPlugin_Placement
 * \ingroup Plugins
 * \brief Feature for applying of placement operation: relative movement of Solid.
 *
 * Locates the selected placement_attractable_object (face, edge, vertex) of the solid into 
 * the selected placement_base_object. Faces must be planar, edges must be linear.
 * Orientation of the placed solid depends on the underlied planes of both faces.
 */
class FeaturesPlugin_Placement : public ModelAPI_Feature
{
 public:
  /// Placement kind
  inline static const std::string& ID()
  {
    static const std::string MY_PLACEMENT_ID("Placement");
    return MY_PLACEMENT_ID;
  }
  /// attribute name of referenced object
  inline static const std::string& BASE_OBJECT_ID()
  {
    static const std::string MY_BASE_OBJECT_ID("placement_base_object");
    return MY_BASE_OBJECT_ID;
  }
  /// attribute name of attractable face
  inline static const std::string& ATTRACT_OBJECT_ID()
  {
    static const std::string MY_ATTRACT_OBJECT_ID("placement_attractable_object");
    return MY_ATTRACT_OBJECT_ID;
  }
  /// attribute name of flag of reverse direction
  inline static const std::string& REVERSE_ID()
  {
    static const std::string MY_REVERSE_ID("placement_reverse_direction");
    return MY_REVERSE_ID;
  }
  /// attribute name of flag of centering position
  inline static const std::string& CENTERING_ID()
  {
    static const std::string MY_CENTERING_ID("placement_centering");
    return MY_CENTERING_ID;
  }

  /// Returns the kind of a feature
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Placement::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  FeaturesPlugin_Placement();
private:
  /// Load Naming data structure of the feature to the document
  void LoadNamingDS(GeomAlgoAPI_Placement& theFeature,
                    std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                    std::shared_ptr<GeomAPI_Shape> theSlaveObject);
};

#endif
