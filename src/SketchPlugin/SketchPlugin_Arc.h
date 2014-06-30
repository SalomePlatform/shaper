// File:        SketchPlugin_Arc.h
// Created:     26 May 2014
// Author:      Artem ZHIDKOV

#ifndef SketchPlugin_Arc_HeaderFile
#define SketchPlugin_Arc_HeaderFile

#include "SketchPlugin.h"
#include <SketchPlugin_Feature.h>
#include <SketchPlugin_Sketch.h>

/// Arc feature kind
const std::string SKETCH_ARC_KIND("SketchArc");

/// Central 2D point of the circle which contains the arc
const std::string ARC_ATTR_CENTER("ArcCenter");
/// Start 2D point of the arc
const std::string ARC_ATTR_START("ArcStartPoint");
/// End 2D point of the arc
const std::string ARC_ATTR_END("ArcEndPoint");

/**\class SketchPlugin_Arc
 * \ingroup DataModel
 * \brief Feature for creation of the new arc of circle in PartSet.
 */
class SketchPlugin_Arc: public SketchPlugin_Feature
{
public:
  /// Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {static std::string MY_KIND = SKETCH_ARC_KIND; return MY_KIND;}

  /// Returns to which group in the document must be added feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getGroup()
  {static std::string MY_GROUP = SKETCH_KIND; return MY_GROUP;}

  /// Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Returns the sketch preview
  SKETCHPLUGIN_EXPORT virtual const boost::shared_ptr<GeomAPI_Shape>& preview();

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual boost::shared_ptr<GeomAPI_AISObject> getAISObject(
                                boost::shared_ptr<GeomAPI_AISObject> thePrevious);

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
  SketchPlugin_Arc();
};

#endif
