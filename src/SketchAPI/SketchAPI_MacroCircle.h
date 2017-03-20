// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchAPI_MacroCircle.h
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#ifndef SketchAPI_MacroCircle_H_
#define SketchAPI_MacroCircle_H_

#include "SketchAPI.h"
#include "SketchAPI_SketchEntity.h"

#include <SketchPlugin_MacroCircle.h>

class ModelHighAPI_Selection;

/// \class SketchAPI_MacroCircle
/// \ingroup CPPHighAPI
/// \brief Interface for Circle feature.
class SketchAPI_MacroCircle: public SketchAPI_SketchEntity
{
public:
  /// Constructor without values.
  SKETCHAPI_EXPORT
  explicit SketchAPI_MacroCircle(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_MacroCircle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                        double theCenterX,
                        double theCenterY,
                        double thePassedX,
                        double thePassedY);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_MacroCircle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                        const std::shared_ptr<GeomAPI_Pnt2d>& theCenterPoint,
                        const std::shared_ptr<GeomAPI_Pnt2d>& thePassedPoint);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_MacroCircle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                        double theX1, double theY1,
                        double theX2, double theY2,
                        double theX3, double theY3);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_MacroCircle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                        const std::shared_ptr<GeomAPI_Pnt2d>& thePoint1,
                        const std::shared_ptr<GeomAPI_Pnt2d>& thePoint2,
                        const std::shared_ptr<GeomAPI_Pnt2d>& thePoint3);

  /// Destructor.
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_MacroCircle();

  INTERFACE_6(SketchPlugin_MacroCircle::ID(),
              circleType, SketchPlugin_MacroCircle::CIRCLE_TYPE(),
              ModelAPI_AttributeString, /** Circle type */,
              centerPoint, SketchPlugin_MacroCircle::CENTER_POINT_ID(),
              GeomDataAPI_Point2D, /** Center point */,
              passedPoint, SketchPlugin_MacroCircle::PASSED_POINT_ID(),
              GeomDataAPI_Point2D, /** Passed point */,
              firstPoint, SketchPlugin_MacroCircle::FIRST_POINT_ID(),
              GeomDataAPI_Point2D, /** First point */,
              secondPoint, SketchPlugin_MacroCircle::SECOND_POINT_ID(),
              GeomDataAPI_Point2D, /** Second point */,
              thirdPoint, SketchPlugin_MacroCircle::THIRD_POINT_ID(),
              GeomDataAPI_Point2D, /** Third point */)

  /// Set by center and passed points.
  SKETCHAPI_EXPORT
  void setByCenterAndPassedPoints(double theCenterX, double theCenterY,
                                  double thePassedX, double thePassedY);

  /// Set by center and passed points.
  SKETCHAPI_EXPORT
  void setByCenterAndPassedPoints(const std::shared_ptr<GeomAPI_Pnt2d>& theCenterPoint,
                                  const std::shared_ptr<GeomAPI_Pnt2d>& thePassedPoint);

  /// Set by three points.
  SKETCHAPI_EXPORT
  void setByThreePoints(double theX1, double theY1,
                        double theX2, double theY2,
                        double theX3, double theY3);

  /// Set by three points.
  SKETCHAPI_EXPORT
  void setByThreePoints(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint1,
                        const std::shared_ptr<GeomAPI_Pnt2d>& thePoint2,
                        const std::shared_ptr<GeomAPI_Pnt2d>& thePoint3);

  /// Set center point.
  SKETCHAPI_EXPORT
  void setCenterPoint(double theX, double theY);

  /// Set center point.
  SKETCHAPI_EXPORT
  void setCenterPoint(const std::shared_ptr<GeomAPI_Pnt2d>& theCenterPoint);

  /// Set passed point.
  SKETCHAPI_EXPORT
  void setPassedPoint(double theX, double theY);

  /// Set passed point.
  SKETCHAPI_EXPORT
  void setPassedPoint(const std::shared_ptr<GeomAPI_Pnt2d>& thePassedPoint);

  /// Set first point.
  SKETCHAPI_EXPORT
  void setFirstPoint(double theX, double theY);

  /// Set first point.
  SKETCHAPI_EXPORT
  void setFirstPoint(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint);

  /// Set second point.
  SKETCHAPI_EXPORT
  void setSecondPoint(double theX, double theY);

  /// Set second point.
  SKETCHAPI_EXPORT
  void setSecondPoint(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint);

  /// Set third point.
  SKETCHAPI_EXPORT
  void setThirdPoint(double theX, double theY);

  /// Set third point.
  SKETCHAPI_EXPORT
  void setThirdPoint(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint);
};

/// Pointer on Circle object.
typedef std::shared_ptr<SketchAPI_MacroCircle> MacroCirclePtr;

#endif // SketchAPI_MacroCircle_H_
