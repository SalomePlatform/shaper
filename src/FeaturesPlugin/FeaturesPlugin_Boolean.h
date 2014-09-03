// File:        FeaturesPlugin_Boolean.h
// Created:     02 Sept 2014
// Author:      Vitaly SMETANNIKOV

#ifndef FeaturesPlugin_Cut_H_
#define FeaturesPlugin_Cut_H_

#include "FeaturesPlugin.h"
#include <ModelAPI_Feature.h>
#include <GeomAPI_Shape.h>

class FeaturesPlugin_Boolean : public ModelAPI_Feature
{
 public:
  /// Extrusion kind
  inline static const std::string& ID()
  {
    static const std::string MY_CUT_ID("Boolean");
    return MY_CUT_ID;
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
  /// attribute name of operation type
  inline static const std::string& TYPE_ID()
  {
    static const std::string MY_TOOL_ID("bool_type");
    return MY_TOOL_ID;
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
  boost::shared_ptr<GeomAPI_Shape> getShape(const std::string& theAttrName);

};

#endif
