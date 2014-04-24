// File:        SketchPlugin_Sketch.h
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef SketchPlugin_Sketch_HeaderFile
#define SketchPlugin_Sketch_HeaderFile

#include "SketchPlugin.h"
#include <SketchPlugin_Feature.h>
#include <list>

/// Coefficient A of the sketch plane (Ax+By+Cz+D=0)
const std::string SKETCH_ATTR_PLANE_A("PlaneA");
/// Coefficient B of the sketch plane
const std::string SKETCH_ATTR_PLANE_B("PlaneB");
/// Coefficient C of the sketch plane
const std::string SKETCH_ATTR_PLANE_C("PlaneC");
/// Coefficient D of the sketch plane
const std::string SKETCH_ATTR_PLANE_D("PlaneD");

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
  SKETCHPLUGIN_EXPORT virtual const boost::shared_ptr<GeomAPI_Shape>& preview();

  /// Use plugin manager for features creation
  SketchPlugin_Sketch();
protected:
  /// Creates a plane and append it to the list
  /// \param theX the X normal value
  /// \param theY the Y normal value
  /// \param theZ the Z normal value
  /// \param theShapes the list of result shapes
  void addPlane(double theX, double theY, double theZ,
                std::list<boost::shared_ptr<GeomAPI_Shape> >& theShapes) const;
};

#endif
