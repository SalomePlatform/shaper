// File:        PartSetPlugin_Point.h
// Created:     3 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef PartSetPlugin_Point_HeaderFile
#define PartSetPlugin_Point_HeaderFile

#include "PartSetPlugin.h"
#include <ModelAPI_Feature.h>

/// attribute name for X coordinate
const std::string POINT_ATTR_X = "x";
/// attribute name for Y coordinate
const std::string POINT_ATTR_Y = "y";
/// attribute name for Z coordinate
const std::string POINT_ATTR_Z = "z";

/**\class PartSetPlugin_Point
 * \ingroup DataModel
 * \brief Feature for creation of the new part in PartSet.
 */
class PartSetPlugin_Point: public ModelAPI_Feature
{
public:
  /// Returns the kind of a feature
  PARTSETPLUGIN_EXPORT virtual const std::string& getKind() 
  {static std::string MY_KIND = "Point"; return MY_KIND;}

  /// Returns to which group in the document must be added feature
  PARTSETPLUGIN_EXPORT virtual const std::string& getGroup() 
  {static std::string MY_GROUP = "Construction"; return MY_GROUP;}

  /// Creates a new part document if needed
  PARTSETPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  PARTSETPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  PartSetPlugin_Point();
};

#endif
