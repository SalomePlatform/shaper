// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Partition.h
// Created:     31 Jul 2015
// Author:      Natalia ERMOLAEVA

#ifndef FeaturesPlugin_Partition_H_
#define FeaturesPlugin_Partition_H_

#include "FeaturesPlugin.h"
#include <ModelAPI_Feature.h>

#include <GeomAlgoAPI_Partition.h>

/**\class FeaturesPlugin_Partition
 * \ingroup Plugins
 * \brief Feature for applying of Partition operations on Solids. Partition makes conjunctional
 * faces of solids as shared. The result of partitions is a compsolid.
 * Main objects are solids, tool objects are solids or faces
 */
class FeaturesPlugin_Partition : public ModelAPI_Feature
{
public:
  /// Extrusion kind
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Partition");
    return MY_ID;
  }
  /// attribute name of referenced object
  inline static const std::string& OBJECT_LIST_ID()
  {
    static const std::string MY_OBJECT_LIST_ID("main_objects");
    return MY_OBJECT_LIST_ID;
  }
  /// attribute name of tool object
  inline static const std::string& TOOL_LIST_ID()
  {
    static const std::string MY_TOOL_LIST_ID("tool_objects");
    return MY_TOOL_LIST_ID;
  }

  /// Returns the kind of a feature
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Partition::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  FeaturesPlugin_Partition();

private:
  std::shared_ptr<GeomAPI_Shape> getShape(const std::string& theAttrName);

  /// Load Naming data structure of the feature to the document
  void loadNamingDS(std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                    const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                    const ListOfShape& theTools,
                    const std::shared_ptr<GeomAPI_Shape> theResultShape,
                    GeomAlgoAPI_MakeShape& theMakeShape,
                    GeomAPI_DataMapOfShapeShape& theMapOfShapes);

};

#endif
