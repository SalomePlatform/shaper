// File:        SketchPlugin_Line.h
// Created:     24 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef SketchPlugin_Line_HeaderFile
#define SketchPlugin_Line_HeaderFile

#include "SketchPlugin.h"
#include <SketchPlugin_Feature.h>
#include <list>

/// Start 2D point of the line
const std::string LINE_ATTR_START("StartPoint");
/// End 2D point of the line
const std::string LINE_ATTR_END("EndPoint");

/**\class SketchPlugin_Line
 * \ingroup DataModel
 * \brief Feature for creation of the new part in PartSet.
 */
class SketchPlugin_Line: public SketchPlugin_Feature
{
public:
  /// Returns the kind of a feature
 SKETCHPLUGIN_EXPORT virtual const std::string& getKind() 
  {static std::string MY_KIND = "SketchLine"; return MY_KIND;}

  /// Returns to which group in the document must be added feature
 SKETCHPLUGIN_EXPORT virtual const std::string& getGroup() 
  {static std::string MY_GROUP = "Sketch"; return MY_GROUP;}

  /// Creates a new part document if needed
 SKETCHPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
 SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Returns the sketch preview
  SKETCHPLUGIN_EXPORT virtual const boost::shared_ptr<GeomAPI_Shape>& preview();

  /// Use plugin manager for features creation
  SketchPlugin_Line();
};

#endif
