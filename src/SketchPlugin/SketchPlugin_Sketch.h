// File:        SketchPlugin_Sketch.h
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef SketchPlugin_Sketch_HeaderFile
#define SketchPlugin_Sketch_HeaderFile

#include "SketchPlugin.h"
#include <SketchPlugin_Feature.h>
#include <GeomAPI_Pnt.h>
#include <list>

/// Origin point of the sketcher in 3D space
const std::string SKETCH_ATTR_ORIGIN("Origin");
/// Vector X inside of the sketch plane
const std::string SKETCH_ATTR_DIRX("DirX");
/// Vector Y inside of the sketch plane
const std::string SKETCH_ATTR_DIRY("DirY");
/// Vector Z, normal to the sketch plane
const std::string SKETCH_ATTR_NORM("Norm");
/// All features of this sketch (list of references)
const std::string SKETCH_ATTR_FEATURES("Features");

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

  /// Adds sub-feature of the higher level feature (sub-element of the sketch)
  /// \param theFeature sub-feature
  SKETCHPLUGIN_EXPORT virtual const void addSub(
    const boost::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Converts a 2D sketch space point into point in 3D space
  SKETCHPLUGIN_EXPORT boost::shared_ptr<GeomAPI_Pnt> to3D(
    const double theX, const double theY);

  /// Returns true if this feature must be displayed in the history (top level of Part tree)
  SKETCHPLUGIN_EXPORT virtual bool isInHistory() {return true;}

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

  /// Checks whether the plane is set in the sketch.
  /// \returns the boolean state
  bool isPlaneSet();
};

#endif
