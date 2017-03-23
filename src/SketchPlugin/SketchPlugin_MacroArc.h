// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_MacroArc.h
// Created:     26 May 2014
// Author:      Artem ZHIDKOV

#ifndef SketchPlugin_MacroArc_H_
#define SketchPlugin_MacroArc_H_

#include "SketchPlugin.h"
#include <SketchPlugin_SketchEntity.h>
#include <SketchPlugin_Sketch.h>
#include <GeomAPI_IPresentable.h>

/**\class SketchPlugin_MacroArc
 * \ingroup Plugins
 * \brief Feature for creation of the new arc of circle in PartSet.
 * The visualization of this object is separated in two parts. The first one is an AIS object
 * calculated when there is non-initialized attributes of the arc. The second is a result and
 * it is calculated if all attributes are initialized.
 */
class SketchPlugin_MacroArc: public SketchPlugin_SketchEntity,
                             public GeomAPI_IPresentable
{
 public:
  /// Arc feature kind
  inline static const std::string& ID()
  {
    static const std::string ID("SketchMacroArc");
    return ID;
  }

  inline static const std::string& ARC_TYPE()
  {
    static const std::string ID("arc_type");
    return ID;
  }

  static const std::string& ARC_TYPE_BY_CENTER_AND_POINTS()
  {
    static const std::string ID("by_center_and_points");
    return ID;
  }
  static const std::string& ARC_TYPE_BY_THREE_POINTS()
  {
    static const std::string ID("by_three_points");
    return ID;
  }

  inline static const std::string& ARC_TYPE_BY_TANGENT_EDGE()
  {
    static const std::string ID("by_tangent_edge");
    return ID;
  }

  /// Central 2D point of the circle which contains the arc
  inline static const std::string& CENTER_POINT_ID()
  {
    static const std::string ID = "center_point";
    return ID;
  }

  inline static const std::string& CENTER_POINT_REF_ID()
  {
    static const std::string ID = "center_point_ref";
    return ID;
  }

  /// Start 2D point of the arc
  inline static const std::string& START_POINT_1_ID()
  {
    static const std::string ID = "start_point_1";
    return ID;
  }

  /// Start 2D point of the arc
  inline static const std::string& START_POINT_2_ID()
  {
    static const std::string ID = "start_point_2";
    return ID;
  }

  inline static const std::string& START_POINT_REF_ID()
  {
    static const std::string ID = "start_point_ref";
    return ID;
  }

  /// End 2D point of the arc
  inline static const std::string& END_POINT_1_ID()
  {
    static const std::string ID = "end_point_1";
    return ID;
  }

  /// End 2D point of the arc
  inline static const std::string& END_POINT_2_ID()
  {
    static const std::string ID = "end_point_2";
    return ID;
  }

  /// End 2D point of the arc
  inline static const std::string& END_POINT_3_ID()
  {
    static const std::string ID = "end_point_3";
    return ID;
  }

  inline static const std::string& END_POINT_REF_ID()
  {
    static const std::string ID = "end_point_ref";
    return ID;
  }

  /// Passed point of the arc.
  static const std::string& PASSED_POINT_ID()
  {
    static const std::string ID("passed_point");
    return ID;
  }

  static const std::string& PASSED_POINT_REF_ID()
  {
    static const std::string ID("passed_point_ref");
    return ID;
  }

  static const std::string& TANGENT_POINT_ID()
  {
    static const std::string ID("tangent_point");
    return ID;
  }

  /// Arc radius.
  static const std::string& RADIUS_ID()
  {
    static const std::string ID("radius");
    return ID;
  }

  /// Arc angle.
  static const std::string& ANGLE_ID()
  {
    static const std::string ID("angle");
    return ID;
  }

  /// Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_MacroArc::ID();
    return MY_KIND;
  }

  /// \brief Request for initialization of data model of the feature: adding all attributes.
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Called on change of any argument-attribute of this object
  /// \param theID identifier of changed attribute
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Returns the AIS preview
  virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// Creates an arc-shape
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Moves the feature
  /// \param theDeltaX the delta for X coordinate is moved
  /// \param theDeltaY the delta for Y coordinate is moved
  SKETCHPLUGIN_EXPORT virtual void move(const double theDeltaX, const double theDeltaY)
  {
  };

  /// Reimplemented from ModelAPI_Feature::isMacro().
  SKETCHPLUGIN_EXPORT virtual bool isMacro() const {return true;};

  SKETCHPLUGIN_EXPORT virtual bool isPreviewNeeded() const {return false;};

  /// Use plugin manager for features creation.
  SketchPlugin_MacroArc();

private:
  std::shared_ptr<GeomAPI_Pnt2d> myCenter;
  std::shared_ptr<GeomAPI_Pnt2d> myStart;
  std::shared_ptr<GeomAPI_Pnt2d> myEnd;

  /// To define in which direction draw arc.
  bool myIsInversed;
  double myParamBefore;
};

#endif
