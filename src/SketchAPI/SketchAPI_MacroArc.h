// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchAPI_MacroArc.h
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#ifndef SketchAPI_MacroArc_H_
#define SketchAPI_MacroArc_H_

#include "SketchAPI.h"
#include "SketchAPI_SketchEntity.h"

#include <GeomDataAPI_Point2D.h>

#include <SketchPlugin_MacroArc.h>

class ModelHighAPI_RefAttr;
class ModelHighAPI_Selection;

/// \class SketchAPI_MacroArc
/// \ingroup CPPHighAPI
/// \brief Interface for Arc feature.
class SketchAPI_MacroArc: public SketchAPI_SketchEntity
{
public:
  /// Constructor without values.
  SKETCHAPI_EXPORT
  explicit SketchAPI_MacroArc(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_MacroArc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                double theCenterX, double theCenterY,
                double theStartX, double theStartY,
                double theEndX, double theEndY,
                bool theInversed);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_MacroArc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                bool theInversed);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_MacroArc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                double theStartX, double theStartY,
                double theEndX, double theEndY,
                double thePassedX, double thePassedY);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_MacroArc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                const std::shared_ptr<GeomAPI_Pnt2d>& thePassed);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_MacroArc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                const ModelHighAPI_RefAttr& theTangentPoint,
                double theEndX, double theEndY,
                bool theInversed);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_MacroArc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                const ModelHighAPI_RefAttr& theTangentPoint,
                const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                bool theInversed);

  /// Destructor.
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_MacroArc();

  void test() {
    SketchPlugin_MacroArc::ID();
  }

  INTERFACE_12(SketchPlugin_MacroArc::ID(),
               arcType, SketchPlugin_MacroArc::ARC_TYPE(),
               ModelAPI_AttributeString, /** Arc type */,
               center, SketchPlugin_MacroArc::CENTER_POINT_ID(),
               GeomDataAPI_Point2D, /** Center point */,
               startPoint1, SketchPlugin_MacroArc::START_POINT_1_ID(),
               GeomDataAPI_Point2D, /** Start point */,
               endPoint1, SketchPlugin_MacroArc::END_POINT_1_ID(),
               GeomDataAPI_Point2D, /** End point */,
               startPoint2, SketchPlugin_MacroArc::START_POINT_2_ID(),
               GeomDataAPI_Point2D, /** Start point */,
               endPoint2, SketchPlugin_MacroArc::END_POINT_2_ID(),
               GeomDataAPI_Point2D, /** End point */,
               passedPoint, SketchPlugin_MacroArc::PASSED_POINT_ID(),
               GeomDataAPI_Point2D, /** Passed point */,
               tangentPoint, SketchPlugin_MacroArc::TANGENT_POINT_ID(),
               ModelAPI_AttributeRefAttr, /** Tangent point */,
               endPoint3, SketchPlugin_MacroArc::END_POINT_3_ID(),
               GeomDataAPI_Point2D, /** End point */,
               reversed, SketchPlugin_MacroArc::REVERSED_ID(),
               ModelAPI_AttributeBoolean, /** Reversed flag */,
               radius, SketchPlugin_MacroArc::RADIUS_ID(),
               ModelAPI_AttributeDouble, /** Radius */,
               angle, SketchPlugin_MacroArc::ANGLE_ID(),
               ModelAPI_AttributeDouble, /** Angle */)

private:

  /// Set by center and start, end point.
  SKETCHAPI_EXPORT
  void setByCenterStartEnd(double theCenterX, double theCenterY,
                           double theStartX, double theStartY,
                           double theEndX, double theEndY,
                           bool theInversed);

  /// Set by center and start, end point.
  SKETCHAPI_EXPORT
  void setByCenterStartEnd(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                           const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                           const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                           bool theInversed);

  /// Set by start, end and passed points.
  SKETCHAPI_EXPORT
  void setByStartEndPassed(double theStartX, double theStartY,
                           double theEndX, double theEndY,
                           double thePassedX, double thePassedY);

  /// Set by start, end and passed points.
  SKETCHAPI_EXPORT
  void setByStartEndPassed(const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                           const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                           const std::shared_ptr<GeomAPI_Pnt2d>& thePassed);

  /// Set by tangent and end point.
  SKETCHAPI_EXPORT
  void setByTangent(const ModelHighAPI_RefAttr& theTangentPoint,
                    double theEndX, double theEndY,
                    bool theInversed);

  /// Set by tangent and end point.
  SKETCHAPI_EXPORT
  void setByTangent(const ModelHighAPI_RefAttr& theTangentPoint,
                    const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                    bool theInversed);
};

/// Pointer on Arc object.
typedef std::shared_ptr<SketchAPI_MacroArc> MacroArcPtr;

#endif // SketchAPI_MacroArc_H_
