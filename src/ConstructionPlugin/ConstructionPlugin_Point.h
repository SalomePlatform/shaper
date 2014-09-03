// File:        ConstructionPlugin_Point.h
// Created:     3 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef ConstructionPlugin_Point_H_
#define ConstructionPlugin_Point_H_

#include "ConstructionPlugin.h"
#include <ModelAPI_Feature.h>

/// Point kind
const std::string CONSTRUCTION_POINT_KIND("Point");

/// attribute name for X coordinate
const std::string POINT_ATTR_X = "x";
/// attribute name for Y coordinate
const std::string POINT_ATTR_Y = "y";
/// attribute name for Z coordinate
const std::string POINT_ATTR_Z = "z";

/**\class ConstructionPlugin_Point
 * \ingroup DataModel
 * \brief Feature for creation of the new part in PartSet.
 */
class ConstructionPlugin_Point : public ModelAPI_Feature
{
 public:
  /// Returns the kind of a feature
  CONSTRUCTIONPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = CONSTRUCTION_POINT_KIND;
    return MY_KIND;
  }

  /// Returns to which group in the document must be added feature
  CONSTRUCTIONPLUGIN_EXPORT virtual const std::string& getGroup()
  {
    static std::string MY_GROUP = "Construction";
    return MY_GROUP;
  }

  /// Creates a new part document if needed
  CONSTRUCTIONPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  CONSTRUCTIONPLUGIN_EXPORT virtual void initAttributes();

  /// Construction result is allways recomuted on the fly
  CONSTRUCTIONPLUGIN_EXPORT virtual bool isPersistentResult() {return false;}

  /// Use plugin manager for features creation
  ConstructionPlugin_Point();
};

#endif
