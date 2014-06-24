// File:        SketchPlugin_Feature.h
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef SketchPlugin_Feature_HeaderFile
#define SketchPlugin_Feature_HeaderFile

#include "SketchPlugin.h"
#include <ModelAPI_Feature.h>
#include <GeomAPI_Shape.h>

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
  /// Returns the sketch preview
  /// \param theSketch the owner of this feature
  /// \return the built preview
  SKETCHPLUGIN_EXPORT virtual const boost::shared_ptr<GeomAPI_Shape>& preview() = 0;

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual Handle_AIS_InteractiveObject getAISShape(Handle_AIS_InteractiveObject thePrevious);

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
  /// Set the shape to the internal preview field
  /// \param theShape a preview shape
  void setPreview(const boost::shared_ptr<GeomAPI_Shape>& theShape); ///< the preview shape
  /// Return the shape from the internal preview field
  /// \return theShape a preview shape
  const boost::shared_ptr<GeomAPI_Shape>& getPreview() const;
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
