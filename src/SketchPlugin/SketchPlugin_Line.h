// File:        SketchPlugin_Line.h
// Created:     24 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef SketchPlugin_Line_HeaderFile
#define SketchPlugin_Line_HeaderFile

#include "SketchPlugin.h"
#include <SketchPlugin_Feature.h>
#include <SketchPlugin_Sketch.h>
#include <list>

/// Line feature kind
const std::string SKETCH_LINE_KIND("SketchLine");

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
  {static std::string MY_KIND = SKETCH_LINE_KIND; return MY_KIND;}

  /// Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual boost::shared_ptr<GeomAPI_AISObject> getAISObject(
                                boost::shared_ptr<GeomAPI_AISObject> thePrevious)
  {return simpleAISObject(firstResult(), thePrevious);}

  /// Moves the feature
  /// \param theDeltaX the delta for X coordinate is moved
  /// \param theDeltaY the delta for Y coordinate is moved
  SKETCHPLUGIN_EXPORT virtual void move(const double theDeltaX, const double theDeltaY);

  /// Return the distance between the feature and the point
  /// \param thePoint the point
  virtual double distanceToPoint(const boost::shared_ptr<GeomAPI_Pnt2d>& thePoint);

  /// Use plugin manager for features creation
  SketchPlugin_Line();
};

#endif
