// File:    SketchPlugin_Point.h
// Created: 07 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_Point_HeaderFile
#define SketchPlugin_Point_HeaderFile

#include "SketchPlugin.h"
#include <SketchPlugin_Sketch.h>
#include "SketchPlugin_Feature.h"
#include <list>

/**\class SketchPlugin_Point
 * \ingroup DataModel
 * \brief Feature for creation of a new point.
 */
class SketchPlugin_Point: public SketchPlugin_Feature
{
public:
  /// Point feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_POINT_ID("SketchPoint");
    return MY_POINT_ID;
  }
  /// Coordinates of the point
  inline static const std::string& COORD_ID()
  {
    static const std::string MY_COORD_ID("PointCoordindates");
    return MY_COORD_ID;
  }
  /// Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind() 
  {static std::string MY_KIND = SketchPlugin_Point::ID(); return MY_KIND;}

  /// Returns to which group in the document must be added feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getGroup() 
  {static std::string MY_GROUP = SketchPlugin_Sketch::ID(); return MY_GROUP;}

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
  SketchPlugin_Point();
};

#endif
