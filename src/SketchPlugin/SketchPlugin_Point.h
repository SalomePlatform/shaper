// File:    SketchPlugin_Point.h
// Created: 07 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_Point_HeaderFile
#define SketchPlugin_Point_HeaderFile

#include "SketchPlugin.h"
#include "SketchPlugin_Feature.h"
#include <list>

/// Coordinates of the point
const std::string POINT_ATTR_COORD("PointCoordindates");

/**\class SketchPlugin_Point
 * \ingroup DataModel
 * \brief Feature for creation of a new point.
 */
class SketchPlugin_Point: public SketchPlugin_Feature
{
public:
  /// Returns the kind of a feature
 SKETCHPLUGIN_EXPORT virtual const std::string& getKind() 
  {static std::string MY_KIND = "SketchPoint"; return MY_KIND;}

  /// Returns to which group in the document must be added feature
 SKETCHPLUGIN_EXPORT virtual const std::string& getGroup() 
  {static std::string MY_GROUP = "Sketch"; return MY_GROUP;}

  /// Creates a new part document if needed
 SKETCHPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
 SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Returns the sketch preview
  SKETCHPLUGIN_EXPORT virtual const boost::shared_ptr<GeomAPI_Shape>& preview();

  /// Adds sub-feature of the higher level feature (sub-element of the sketch)
  /// \param theFeature sub-feature
  SKETCHPLUGIN_EXPORT virtual const void addSub(
    const boost::shared_ptr<ModelAPI_Feature>& theFeature) {};

  /// Use plugin manager for features creation
  SketchPlugin_Point();
};

#endif
