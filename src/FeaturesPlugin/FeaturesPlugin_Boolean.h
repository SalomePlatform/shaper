// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Boolean.h
// Created:     02 Sept 2014
// Author:      Vitaly SMETANNIKOV

#ifndef FeaturesPlugin_Cut_H_
#define FeaturesPlugin_Cut_H_

#include "FeaturesPlugin.h"

#include <GeomAlgoAPI_MakeShape.h>

#include <ModelAPI_Feature.h>

/// \class FeaturesPlugin_Boolean
/// \ingroup Plugins
/// \brief Feature for applying of Boolean operations on Solids.
/// Supports four kinds of Boolean operations: Cut, Fuse, Common and Smash.
class FeaturesPlugin_Boolean : public ModelAPI_Feature
{
public:
  enum OperationType {
    BOOL_CUT,
    BOOL_FUSE,
    BOOL_COMMON,
    BOOL_SMASH
  };

  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Boolean");
    return MY_ID;
  }

  /// Attribute name of main objects.
  inline static const std::string& OBJECT_LIST_ID()
  {
    static const std::string MY_OBJECT_LIST_ID("main_objects");
    return MY_OBJECT_LIST_ID;
  }

  /// Attribute name of tool objects.
  inline static const std::string& TOOL_LIST_ID()
  {
    static const std::string MY_TOOL_LIST_ID("tool_objects");
    return MY_TOOL_LIST_ID;
  }

  /// Attribute name of operation type.
  inline static const std::string& TYPE_ID()
  {
    static const std::string MY_TYPE_ID("bool_type");
    return MY_TYPE_ID;
  }

  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Boolean::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation.
  FeaturesPlugin_Boolean();

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
