// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchAPI_Circle.h
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#ifndef SketchAPI_Circle_H_
#define SketchAPI_Circle_H_

#include "SketchAPI.h"
#include "SketchAPI_SketchEntity.h"

#include <SketchPlugin_Circle.h>

class ModelHighAPI_Selection;

/// \class SketchAPI_Circle
/// \ingroup CPPHighAPI
/// \brief Interface for Circle feature.
class SketchAPI_Circle: public SketchAPI_SketchEntity
{
public:
  /// Constructor without values.
  SKETCHAPI_EXPORT
  explicit SketchAPI_Circle(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_Circle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                   double theCenterX,
                   double theCenterY,
                   double theRadius);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_Circle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                   const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                   double theRadius);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_Circle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                   const ModelHighAPI_Selection& theExternal);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_Circle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                   const std::string& theExternalName);

  /// Destructor.
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_Circle();

  INTERFACE_3(SketchPlugin_Circle::ID(),
              center, SketchPlugin_Circle::CENTER_ID(),
              GeomDataAPI_Point2D, /** Center point */,
              radius, SketchPlugin_Circle::RADIUS_ID(),
              ModelAPI_AttributeDouble, /** Radius */,
              external, SketchPlugin_Circle::EXTERNAL_ID(),
              ModelAPI_AttributeSelection, /** External */)

  /// Set by center and radius.
  SKETCHAPI_EXPORT
  void setByCenterAndRadius(double theCenterX, double theCenterY, double theRadius);

  /// Set by center and radius.
  SKETCHAPI_EXPORT
  void setByCenterAndRadius(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter, double theRadius);

  /// Set by external.
  SKETCHAPI_EXPORT
  void setByExternal(const ModelHighAPI_Selection& theExternal);

  /// Set by external name.
  SKETCHAPI_EXPORT
  void setByExternalName(const std::string& theExternalName);

  /// Set center.
  SKETCHAPI_EXPORT
  void setCenter(double theX, double theY);

  /// Set center.
  SKETCHAPI_EXPORT
  void setCenter(const std::shared_ptr<GeomAPI_Pnt2d> & theCenter);

  /// Set radius.
  SKETCHAPI_EXPORT
  void setRadius(double theRadius);

  /// Dump wrapped feature
  SKETCHAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Circle object.
typedef std::shared_ptr<SketchAPI_Circle> CirclePtr;

#endif // SketchAPI_Circle_H_
