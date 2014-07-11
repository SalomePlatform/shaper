// File:        SketchPlugin_Circle.h
// Created:     26 May 2014
// Author:      Artem ZHIDKOV

#ifndef SketchPlugin_Circle_HeaderFile
#define SketchPlugin_Circle_HeaderFile

#include "SketchPlugin.h"
#include <SketchPlugin_Feature.h>
#include <SketchPlugin_Sketch.h>

/// Circle feature kind
const std::string SKETCH_CIRCLE_KIND("SketchCircle");

/// 2D point - center of the circle
const std::string CIRCLE_ATTR_CENTER("CircleCenter");
/// Radius of the circle
const std::string CIRCLE_ATTR_RADIUS("CircleRadius");

/**\class SketchPlugin_Circle
 * \ingroup DataModel
 * \brief Feature for creation of the new circle in PartSet.
 */
class SketchPlugin_Circle: public SketchPlugin_Feature
{
public:
  /// Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind() 
  {static std::string MY_KIND = SKETCH_CIRCLE_KIND; return MY_KIND;}

  /// Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Returns the AIS preview
  virtual boost::shared_ptr<GeomAPI_AISObject> getAISObject(
                                boost::shared_ptr<GeomAPI_AISObject> thePrevious)
  {return simpleAISObject(firstResult(), thePrevious);}

  /// Adds sub-feature of the higher level feature (sub-element of the sketch)
  /// \param theFeature sub-feature
  SKETCHPLUGIN_EXPORT virtual const void addSub(
    const FeaturePtr& theFeature) {};

  /// Moves the feature
  /// \param theDeltaX the delta for X coordinate is moved
  /// \param theDeltaY the delta for Y coordinate is moved
  SKETCHPLUGIN_EXPORT virtual void move(const double theDeltaX, const double theDeltaY);

  /// Return the distance between the feature and the point
  /// \param thePoint the point
  virtual double distanceToPoint(const boost::shared_ptr<GeomAPI_Pnt2d>& thePoint);

  /// Use plugin manager for features creation
  SketchPlugin_Circle();
};

#endif
