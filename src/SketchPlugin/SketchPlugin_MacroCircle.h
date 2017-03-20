// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_MacroCircle.h
// Created:     26 May 2014
// Author:      Artem ZHIDKOV

#ifndef SketchPlugin_MacroCircle_H_
#define SketchPlugin_MacroCircle_H_

#include "SketchPlugin.h"

#include "SketchPlugin_Sketch.h"
#include "SketchPlugin_SketchEntity.h"

#include <GeomAPI_IPresentable.h>
#include <GeomAPI_Pnt2d.h>

/**\class SketchPlugin_MacroCircle
 * \ingroup Plugins
 * \brief Feature for creation of the new circle in Sketch.
 */
class SketchPlugin_MacroCircle: public SketchPlugin_SketchEntity,
                                public GeomAPI_IPresentable
{
 public:
  /// Circle feature kind
  inline static const std::string& ID()
  {
    static const std::string ID("SketchMacroCircle");
    return ID;
  }

  inline static const std::string& CIRCLE_TYPE()
  {
    static const std::string ID("circle_type");
    return ID;
  }

  /// Creation method by center and passed point.
  inline static const std::string& CIRCLE_TYPE_BY_CENTER_AND_PASSED_POINTS()
  {
    static const std::string ID("circle_type_by_center_and_passed_points");
    return ID;
  }

  /// Creation method by three points.
  inline static const std::string& CIRCLE_TYPE_BY_THREE_POINTS()
  {
    static const std::string ID("circle_type_by_three_points");
    return ID;
  }

  /// 2D point - center of the circle.
  inline static const std::string& CENTER_POINT_ID()
  {
    static const std::string ID("center_point");
    return ID;
  }

  /// Reference for center point selection.
  inline static const std::string& CENTER_POINT_REF_ID()
  {
    static const std::string ID("center_point_ref");
    return ID;
  }

  /// 2D point - passed point of the circle
  inline static const std::string& PASSED_POINT_ID()
  {
    static const std::string ID("passed_point");
    return ID;
  }

  /// Reference for passed point selection.
  inline static const std::string& PASSED_POINT_REF_ID()
  {
    static const std::string ID("passed_point_ref");
    return ID;
  }

  /// First point id.
  inline static const std::string& FIRST_POINT_ID()
  {
    static const std::string ID("first_point");
    return ID;
  }

  /// Second point id.
  inline static const std::string& SECOND_POINT_ID()
  {
    static const std::string ID("second_point");
    return ID;
  }

  /// Third point id.
  inline static const std::string& THIRD_POINT_ID()
  {
    static const std::string ID("third_point");
    return ID;
  }

  /// Radius of the circle
  inline static const std::string& CIRCLE_RADIUS_ID()
  {
    static const std::string ID("circle_radius");
    return ID;
  }

  /// Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_MacroCircle::ID();
    return MY_KIND;
  }

  /// \brief Request for initialization of data model of the feature: adding all attributes.
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Called on change of any argument-attribute of this object
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Returns the AIS preview
  virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Moves the feature
  /// \param theDeltaX the delta for X coordinate is moved
  /// \param theDeltaY the delta for Y coordinate is moved
  SKETCHPLUGIN_EXPORT virtual void move(const double theDeltaX, const double theDeltaY)
  {};

  /// Reimplemented from ModelAPI_Feature::isMacro().
  /// \returns true
  SKETCHPLUGIN_EXPORT virtual bool isMacro() const {return true;};

  SKETCHPLUGIN_EXPORT virtual bool isPreviewNeeded() const {return false;};

  /// Use plugin manager for features creation
  SketchPlugin_MacroCircle();

private:
  void resetAttribute(const std::string& theId);

private:
  std::shared_ptr<GeomAPI_Pnt2d> myCenter;
  double myRadius;
};

#endif
