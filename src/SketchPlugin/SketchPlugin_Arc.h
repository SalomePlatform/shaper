// File:        SketchPlugin_Arc.h
// Created:     26 May 2014
// Author:      Artem ZHIDKOV

#ifndef SketchPlugin_Arc_H_
#define SketchPlugin_Arc_H_

#include "SketchPlugin.h"
#include <SketchPlugin_Feature.h>
#include <SketchPlugin_Sketch.h>
#include <GeomAPI_IPresentable.h>

/**\class SketchPlugin_Arc
 * \ingroup DataModel
 * \brief Feature for creation of the new arc of circle in PartSet.
 * The visualization of this object is separated in two parts. The first one is an AIS object
 * calculated when there is non-initialized attributes of the arc. The second is a result and
 * it is calculated if all attributes are initialized.
 */
class SketchPlugin_Arc : public SketchPlugin_Feature, public GeomAPI_IPresentable
{
  /// to avoid cyclic dependencies in automatic updates: they mean that 
  /// update is performed right now and automatic updates are not needed
  bool myStartUpdate, myEndUpdate;

 public:
  /// Arc feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_SKETCH_ARC_ID("SketchArc");
    return MY_SKETCH_ARC_ID;
  }

  /// Central 2D point of the circle which contains the arc
  inline static const std::string& CENTER_ID()
  {
    static const std::string MY_CENTER_ID = "ArcCenter";
    return MY_CENTER_ID;
  }
  /// Start 2D point of the arc
  inline static const std::string& START_ID()
  {
    static const std::string MY_START_ID = "ArcStartPoint";
    return MY_START_ID;
  }
  /// End 2D point of the arc
  inline static const std::string& END_ID()
  {
    static const std::string MY_END_ID = "ArcEndPoint";
    return MY_END_ID;
  }

  /// Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_Arc::ID();
    return MY_KIND;
  }

  /// Returns true is sketch element is under the rigid constraint
  SKETCHPLUGIN_EXPORT virtual bool isFixed();

  /// Creates an arc-shape
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Called on change of any argument-attribute of this object
  /// \param theID identifier of changed attribute
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Returns the AIS preview
  virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// Moves the feature
  /// \param theDeltaX the delta for X coordinate is moved
  /// \param theDeltaY the delta for Y coordinate is moved
  SKETCHPLUGIN_EXPORT virtual void move(const double theDeltaX, const double theDeltaY);

  /// Return the distance between the feature and the point
  /// \param thePoint the point
  virtual double distanceToPoint(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint);

  /// Use plugin manager for features creation
  SketchPlugin_Arc();

private:
  /// Returns true if all obligatory attributes are initialized
  bool isFeatureValid();
};

#endif
