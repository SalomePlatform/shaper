// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Boolean.h
// Created:     02 Sept 2014
// Author:      Vitaly SMETANNIKOV

#ifndef FeaturesPlugin_Cut_H_
#define FeaturesPlugin_Cut_H_

#include "FeaturesPlugin.h"
#include <ModelAPI_Feature.h>
#include <GeomAPI_Shape.h>
#include <GeomAlgoAPI_Boolean.h>

/**\class FeaturesPlugin_Boolean
 * \ingroup Plugins
 * \brief Feature for applying of Boolean operations on Solids.
 *
 * Supports three kinds of Boolean operations: Cut, Fuse and Common.
 * For all of them requires two Solids: object and tool.
 */
class FeaturesPlugin_Boolean : public ModelAPI_Feature
{
public:
  /// Extrusion kind
  inline static const std::string& ID()
  {
    static const std::string MY_ID("Boolean");
    return MY_ID;
  }
  /// attribute name of referenced object
  inline static const std::string& OBJECT_LIST_ID()
  {
    static const std::string MY_OBJECT_LIST_ID("main_objects");
    return MY_OBJECT_LIST_ID;
  }
  /// attribute name of referenced object
  inline static const std::string& OBJECT_ID()
  {
    static const std::string MY_OBJECT_ID("main_object");
    return MY_OBJECT_ID;
  }
  /// attribute name of tool object
  inline static const std::string& TOOL_ID()
  {
    static const std::string MY_TOOL_ID("tool_object");
    return MY_TOOL_ID;
  }
  /// attribute name of tool object
  inline static const std::string& TOOL_LIST_ID()
  {
    static const std::string MY_TOOL_LIST_ID("tool_objects");
    return MY_TOOL_LIST_ID;
  }
  /// attribute name of operation type
  inline static const std::string& TYPE_ID()
  {
    static const std::string MY_TYPE_ID("bool_type");
    return MY_TYPE_ID;
  }

  enum {
	BOOL_CUT,
	BOOL_FUSE,
    BOOL_COMMON
  };

  /// Returns the kind of a feature
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_Boolean::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  FeaturesPlugin_Boolean();

private:
  std::shared_ptr<GeomAPI_Shape> getShape(const std::string& theAttrName);
  
  /// Load Naming data structure of the feature to the document
  void LoadNamingDS(GeomAlgoAPI_Boolean* theFeature, 
					std::shared_ptr<ModelAPI_ResultBody> theResultBody,
	                std::shared_ptr<GeomAPI_Shape> theObject,
	                std::shared_ptr<GeomAPI_Shape> theTool,
					int theType);
};

#endif
