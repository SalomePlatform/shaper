// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Intersection.h
// Created:     15 Feb 2016
// Author:      Dmitry Bobylev

#ifndef FeaturesPlugin_Intersection_H_
#define FeaturesPlugin_Intersection_H_

#include "FeaturesPlugin.h"

#include <ModelAPI_Feature.h>

#include <GeomAPI_Shape.h>

class GeomAlgoAPI_MakeShape;

/// \class FeaturesPlugin_Intersection
/// \ingroup Plugins
/// \brief Intersection feature takes a list of shapes as objects and list of shapes as tools.
/// The types of objects and tools may be different: whole objects, compsoilds, solids, shells, faces or edges.
/// The result is less than the minimal dimension from pair of intersection:
/// for two solids or two faces it is wire, for the edge and face it is vertex, etc.
class FeaturesPlugin_Intersection : public ModelAPI_Feature
{
public:
  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Intersection");
    return MY_ID;
  }

  /// Attribute name of objects.
  inline static const std::string& OBJECT_LIST_ID()
  {
    static const std::string MY_OBJECT_LIST_ID("main_objects");
    return MY_OBJECT_LIST_ID;
  }

  /// Attribute name of tools.
  inline static const std::string& TOOL_LIST_ID()
  {
    static const std::string MY_TOOL_LIST_ID("tool_objects");
    return MY_TOOL_LIST_ID;
  }

  /// Returns the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Intersection::ID();
    return MY_KIND;
  }

  /// Executes feature.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation.
  FeaturesPlugin_Intersection();

private:
  /// Load Naming data structure of the feature to the document.
  void loadNamingDS(std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                    const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                    const ListOfShape& theTools,
                    GeomAlgoAPI_MakeShape& theMakeShape);
};

#endif
