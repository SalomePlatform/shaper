// File:        SketchPlugin_Sketch.h
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef SketchPlugin_Sketch_HeaderFile
#define SketchPlugin_Sketch_HeaderFile

#include "SketchPlugin.h"
#include <SketchPlugin_Feature.h>

/// part reference attribute
const std::string PART_ATTR_DOC_REF = "SketchDocument";

/**\class SketchPlugin_Sketch
 * \ingroup DataModel
 * \brief Feature for creation of the new part in PartSet.
 */
class SketchPlugin_Sketch: public SketchPlugin_Feature
{
public:
  /// Returns the kind of a feature
 SKETCHPLUGIN_EXPORT virtual const std::string& getKind() 
  {static std::string MY_KIND = "Sketch"; return MY_KIND;}

  /// Returns to which group in the document must be added feature
 SKETCHPLUGIN_EXPORT virtual const std::string& getGroup() 
  {static std::string MY_GROUP = "Construction"; return MY_GROUP;}

  /// Creates a new part document if needed
 SKETCHPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
 SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Returns the sketch preview
  SKETCHPLUGIN_EXPORT virtual const std::shared_ptr<GeomAPI_Shape>& preview();

  /// Use plugin manager for features creation
  SketchPlugin_Sketch();
};

#endif
