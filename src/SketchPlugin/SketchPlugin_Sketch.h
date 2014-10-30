// File:        SketchPlugin_Sketch.h
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef SketchPlugin_Sketch_H_
#define SketchPlugin_Sketch_H_

#include "SketchPlugin.h"
#include <SketchPlugin_Feature.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_IPresentable.h>
#include <list>

/// the plane edge color
#define SKETCH_PLANE_COLOR "#700000" 

/// the plane edge width
#define SKETCH_WIDTH        "4"

/// face of the square-face displayed for selection of general plane
#define PLANE_SIZE          "200"     

/**\class SketchPlugin_Sketch
 * \ingroup DataModel
 * \brief Feature for creation of the new part in PartSet.
 */
class SketchPlugin_Sketch : public ModelAPI_CompositeFeature, public GeomAPI_IPresentable
{
 public:
  /// Sketch feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_SKETCH_ID("Sketch");
    return MY_SKETCH_ID;
  }
  /// Origin point of the sketcher in 3D space
  inline static const std::string& ORIGIN_ID()
  {
    static const std::string MY_ORIGIN_ID("Origin");
    return MY_ORIGIN_ID;
  }
  /// Vector X inside of the sketch plane
  inline static const std::string& DIRX_ID()
  {
    static const std::string MY_DIRX_ID("DirX");
    return MY_DIRX_ID;
  }
  /// Vector Y inside of the sketch plane
  inline static const std::string& DIRY_ID()
  {
    static const std::string MY_DIRY_ID("DirY");
    return MY_DIRY_ID;
  }
  /// Vector Z, normal to the sketch plane
  inline static const std::string& NORM_ID()
  {
    static const std::string MY_NORM_ID("Norm");
    return MY_NORM_ID;
  }
  /// All features of this sketch (list of references)
  inline static const std::string& FEATURES_ID()
  {
    static const std::string MY_FEATURES_ID("Features");
    return MY_FEATURES_ID;
  }

  /// Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_Sketch::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Moves the feature
  /// \param theDeltaX the delta for X coordinate is moved
  /// \param theDeltaY the delta for Y coordinate is moved
  SKETCHPLUGIN_EXPORT virtual void move(const double theDeltaX, const double theDeltaY)
  {
  }
  ;

  /// Return the distance between the feature and the point
  /// \param thePoint the point
  virtual double distanceToPoint(const boost::shared_ptr<GeomAPI_Pnt2d>& thePoint)
  {
    return 0;
  }
  ;

  /// Converts a 2D sketch space point into point in 3D space
  SKETCHPLUGIN_EXPORT boost::shared_ptr<GeomAPI_Pnt> to3D(const double theX, const double theY);

  /// Returns true if this feature must be displayed in the history (top level of Part tree)
  SKETCHPLUGIN_EXPORT virtual bool isInHistory()
  {
    return true;
  }

  /// Use plugin manager for features creation
  SketchPlugin_Sketch();

  /// Returns the basis plane for the sketch
  boost::shared_ptr<GeomAPI_Pln> plane();

  virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// removes also all sub-sketch elements
  SKETCHPLUGIN_EXPORT virtual void erase();

  SKETCHPLUGIN_EXPORT virtual boost::shared_ptr<ModelAPI_Feature> addFeature(std::string theID);

  /// Returns the number of sub-elements
  SKETCHPLUGIN_EXPORT virtual int numberOfSubs() const;

  /// Returns the sub-feature by zero-base index
  SKETCHPLUGIN_EXPORT virtual boost::shared_ptr<ModelAPI_Feature> 
    subFeature(const int theIndex) const;

  /// Returns the sub-feature unique identifier in this composite feature by zero-base index
  SKETCHPLUGIN_EXPORT virtual int subFeatureId(const int theIndex) const;

  /// Returns true if feature or reuslt belong to this composite feature as subs
  SKETCHPLUGIN_EXPORT virtual bool isSub(ObjectPtr theObject) const;

  /// Construction result is allways recomuted on the fly
  SKETCHPLUGIN_EXPORT virtual bool isPersistentResult() {return false;}

  /// Returns the point projected into the sketch plane
  boost::shared_ptr<GeomAPI_Pnt2d> to2D(const boost::shared_ptr<GeomAPI_Pnt>& thePnt);

protected:
  /// Creates a plane and append it to the list
  /// \param theX the X normal value
  /// \param theY the Y normal value
  /// \param theZ the Z normal value
  /// \param theShapes the list of result shapes
  //void addPlane(double theX, double theY, double theZ,
  //              std::list<boost::shared_ptr<GeomAPI_Shape> >& theShapes) const;

  /// Checks whether the plane is set in the sketch.
  /// \returns the boolean state
  bool isPlaneSet();
};

#endif
