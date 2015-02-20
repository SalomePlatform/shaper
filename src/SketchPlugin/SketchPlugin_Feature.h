// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_Feature.h
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef SketchPlugin_Feature_H_
#define SketchPlugin_Feature_H_

#include "SketchPlugin.h"
#include <ModelAPI_CompositeFeature.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_AISObject.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeSelection.h>
#include <GeomAPI_ICustomPrs.h>

#include <Config_PropManager.h>

#define SKETCH_EDGE_COLOR "#ff0000"
#define SKETCH_POINT_COLOR "#ff0000"
#define SKETCH_EXTERNAL_EDGE_COLOR "#00ff00"

class SketchPlugin_Sketch;
class GeomAPI_Pnt2d;
class Handle_AIS_InteractiveObject;

/**\class SketchPlugin_Feature
 * \ingroup Plugins
 * \brief Feature for creation of the new feature in PartSet. This is an abstract class to give
 * an interface to create the sketch feature preview.
 */
class SketchPlugin_Feature : public ModelAPI_Feature, public GeomAPI_ICustomPrs
{
 public:
  /// Reference to the external edge or vertex as a AttributeSelection
  inline static const std::string& EXTERNAL_ID()
  {
    static const std::string MY_EXTERNAL_ID("External");
    return MY_EXTERNAL_ID;
  }

  /// Returns true if this feature must be displayed in the history (top level of Part tree)
  SKETCHPLUGIN_EXPORT virtual bool isInHistory()
  {
    return false;
  }

  /// Moves the feature
  /// \param theDeltaX the delta for X coordinate is moved
  /// \param theDeltaY the delta for Y coordinate is moved
  SKETCHPLUGIN_EXPORT virtual void move(const double theDeltaX, const double theDeltaY) = 0;

  /// Return the distance between the feature and the point
  /// \param thePoint the point
  virtual double distanceToPoint(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint) = 0;

  /// Construction result is allways recomuted on the fly
  SKETCHPLUGIN_EXPORT virtual bool isPersistentResult() {return false;}

  /// Returns true is sketch element is under the rigid constraint
  SKETCHPLUGIN_EXPORT virtual bool isFixed() {return false;}

  /// Returns true of the feature is created basing on the external shape of not-this-sketch object
  inline bool isExternal() const
  {
    AttributeSelectionPtr aAttr = data()->selection(EXTERNAL_ID());
    if (aAttr)
      return aAttr->context().get() != NULL;
    return false;
  }

  /// Customize presentation of the feature
  virtual void customisePresentation(AISObjectPtr thePrs)
  {
    std::vector<int> aRGB;
    // if this is an edge
    if (thePrs->getShapeType() == 6) {
      thePrs->setWidth(3);
      if (isExternal()) {
        // Set color from preferences
        aRGB = Config_PropManager::color("Visualization", "sketch_external_color",
                                         SKETCH_EXTERNAL_EDGE_COLOR);
      }
      else {
        // Set color from preferences
        aRGB = Config_PropManager::color("Visualization", "sketch_edge_color",
                                         SKETCH_EDGE_COLOR);
      }
    }
    else if (thePrs->getShapeType() == 7) { // otherwise this is a vertex
      // Set color from preferences
      aRGB = Config_PropManager::color("Visualization", "sketch_point_color",
                                       SKETCH_POINT_COLOR);
    }
    // if this is a vertex
    //else if (thePrs->getShapeType() == 7)
    //  thePrs->setPointMarker(6, 2.);
    if (!aRGB.empty())
      thePrs->setColor(aRGB[0], aRGB[1], aRGB[2]);
  }

  /// Returns the sketch of this feature
  SketchPlugin_Sketch* sketch();
protected:
  /// Sets the higher-level feature for the sub-feature (sketch for line)
  void setSketch(SketchPlugin_Sketch* theSketch)
  {
    mySketch = theSketch;
  }
  /// initializes mySketch
  SketchPlugin_Feature();

  friend class SketchPlugin_Sketch;

 private:
  std::shared_ptr<GeomAPI_Shape> myPreview;  ///< the preview shape
  SketchPlugin_Sketch* mySketch;  /// sketch that contains this feature
};

#endif
