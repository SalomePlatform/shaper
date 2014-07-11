// File:        SketchPlugin_Feature.h
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef SketchPlugin_Feature_HeaderFile
#define SketchPlugin_Feature_HeaderFile

#include "SketchPlugin.h"
#include <ModelAPI_Feature.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_AISObject.h>
#include <ModelAPI_Document.h>

class SketchPlugin_Sketch;
class GeomAPI_Pnt2d;
class Handle_AIS_InteractiveObject;

/**\class SketchPlugin_Feature
 * \ingroup DataModel
 * \brief Feature for creation of the new feature in PartSet. This is an abstract class to give
 * an interface to create the sketch feature preview.
 */
class SketchPlugin_Feature: public ModelAPI_Feature
{
public:
  /// Returns the type of the feature result: it is allways construction element
  virtual const std::string& getResultType() {return CONSTRUCTIONS_GROUP;}

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual boost::shared_ptr<GeomAPI_AISObject> getAISObject(
                                boost::shared_ptr<GeomAPI_AISObject> thePrevious) = 0;

  /// Adds sub-feature of the higher level feature (sub-element of the sketch)
  /// \param theFeature sub-feature
  SKETCHPLUGIN_EXPORT virtual const void addSub(
    const FeaturePtr& theFeature) = 0;

  /// Returns true if this feature must be displayed in the history (top level of Part tree)
  SKETCHPLUGIN_EXPORT virtual bool isInHistory() {return false;}

  /// Moves the feature
  /// \param theDeltaX the delta for X coordinate is moved
  /// \param theDeltaY the delta for Y coordinate is moved
  SKETCHPLUGIN_EXPORT virtual void move(const double theDeltaX, const double theDeltaY) = 0;

  /// Return the distance between the feature and the point
  /// \param thePoint the point
  virtual double distanceToPoint(const boost::shared_ptr<GeomAPI_Pnt2d>& thePoint) = 0;

protected:
  /// Sets the higher-level feature for the sub-feature (sketch for line)
  void setSketch(SketchPlugin_Sketch* theSketch) {mySketch = theSketch;}
  /// Returns the sketch of this feature
  SketchPlugin_Sketch* sketch();
  /// initializes mySketch
  SketchPlugin_Feature();

  friend class SketchPlugin_Sketch;

private:
  boost::shared_ptr<GeomAPI_Shape> myPreview; ///< the preview shape
  SketchPlugin_Sketch* mySketch; /// sketch that contains this feature
};

#endif
