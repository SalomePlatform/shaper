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

class SketchPlugin_Sketch;
class GeomAPI_Pnt2d;
class Handle_AIS_InteractiveObject;

/**\class SketchPlugin_Feature
 * \ingroup DataModel
 * \brief Feature for creation of the new feature in PartSet. This is an abstract class to give
 * an interface to create the sketch feature preview.
 */
class SketchPlugin_Feature : public ModelAPI_Feature
{
 public:
  /// Simple creation of interactive object by the result of the object
  static AISObjectPtr simpleAISObject(boost::shared_ptr<ModelAPI_Result> theRes,
                                      AISObjectPtr thePrevious);

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
  virtual double distanceToPoint(const boost::shared_ptr<GeomAPI_Pnt2d>& thePoint) = 0;

  /// Construction result is allways recomuted on the fly
  SKETCHPLUGIN_EXPORT virtual bool isPersistentResult() {return false;}

  /// Returns true is sketch element is under the rigid constraint
  SKETCHPLUGIN_EXPORT virtual bool isFixed() {return false;}

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
  boost::shared_ptr<GeomAPI_Shape> myPreview;  ///< the preview shape
  SketchPlugin_Sketch* mySketch;  /// sketch that contains this feature
};

#endif
