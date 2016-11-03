// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchAPI_Arc.h
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#ifndef SketchAPI_Arc_H_
#define SketchAPI_Arc_H_

#include "SketchAPI.h"
#include "SketchAPI_SketchEntity.h"

#include <GeomDataAPI_Point2D.h>

#include <SketchPlugin_Arc.h>

class ModelHighAPI_RefAttr;
class ModelHighAPI_Selection;

/// \class SketchAPI_Arc
/// \ingroup CPPHighAPI
/// \brief Interface for Arc feature.
class SketchAPI_Arc: public SketchAPI_SketchEntity
{
public:
  /// Constructor without values.
  SKETCHAPI_EXPORT
  explicit SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                double theCenterX, double theCenterY,
                double theStartX, double theStartY,
                double theEndX, double theEndY,
                bool theInversed);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                bool theInversed);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                double theStartX, double theStartY,
                double theEndX, double theEndY,
                double thePassedX, double thePassedY);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                const std::shared_ptr<GeomAPI_Pnt2d>& thePassed);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                const ModelHighAPI_RefAttr& theTangentPoint,
                double theEndX, double theEndY,
                bool theInversed);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                const ModelHighAPI_RefAttr& theTangentPoint,
                const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                bool theInversed);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                const ModelHighAPI_Selection& theExternal);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                const std::string& theExternalName);

  /// Destructor.
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_Arc();

  INTERFACE_10(SketchPlugin_Arc::ID(),
               arcType, SketchPlugin_Arc::ARC_TYPE(),
               ModelAPI_AttributeString, /** Arc type */,
               center, SketchPlugin_Arc::CENTER_ID(),
               GeomDataAPI_Point2D, /** Center point */,
               startPoint, SketchPlugin_Arc::START_ID(),
               GeomDataAPI_Point2D, /** Start point */,
               endPoint, SketchPlugin_Arc::END_ID(),
               GeomDataAPI_Point2D, /** End point */,
               inversed, SketchPlugin_Arc::INVERSED_ID(),
               ModelAPI_AttributeBoolean, /** Inversed flag */,
               passedPoint, SketchPlugin_Arc::PASSED_POINT_ID(),
               GeomDataAPI_Point2D, /** Passed point */,
               tangentPoint, SketchPlugin_Arc::TANGENT_POINT_ID(),
               ModelAPI_AttributeRefAttr, /** Tangent point */,
               radius, SketchPlugin_Arc::RADIUS_ID(),
               ModelAPI_AttributeDouble, /** Radius */,
               angle, SketchPlugin_Arc::ANGLE_ID(),
               ModelAPI_AttributeDouble, /** Angle */,
               external, SketchPlugin_Arc::EXTERNAL_ID(),
               ModelAPI_AttributeSelection, /** External */)

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

  /// Set by external.
  SKETCHAPI_EXPORT
  void setByExternal(const ModelHighAPI_Selection& theExternal);

  /// Set by external name.
  SKETCHAPI_EXPORT
  void setByExternalName(const std::string& theExternalName);

  /// Set radius.
  SKETCHAPI_EXPORT
  void setRadius(double theRadius);

  /// Set angle.
  SKETCHAPI_EXPORT
  void setAngle(double theAngle);

  /// Dump wrapped feature
  SKETCHAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Arc object.
typedef std::shared_ptr<SketchAPI_Arc> ArcPtr;

#endif // SketchAPI_Arc_H_
