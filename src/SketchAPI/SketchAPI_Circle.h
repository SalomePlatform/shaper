// Copyright (C) 2014-2024  CEA, EDF
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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
                   double theRadius,
                   bool theIsAddPoint = false,
                   double theAngle = 0.0);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_Circle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                   const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                   double theRadius,
                   bool theIsAddPoint = false,
                   double theAngle = 0.0);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_Circle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                   const ModelHighAPI_Selection& theExternal);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_Circle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                   const std::wstring& theExternalName);

  /// Destructor.
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_Circle();

  INTERFACE_5(SketchPlugin_Circle::ID(),
              center, SketchPlugin_Circle::CENTER_ID(),
              GeomDataAPI_Point2D, /** Center point */,
              radius, SketchPlugin_Circle::RADIUS_ID(),
              ModelAPI_AttributeDouble, /** Radius */,
              external, SketchPlugin_Circle::EXTERNAL_ID(),
              ModelAPI_AttributeSelection, /** External */,
              angle, SketchPlugin_Circle::ANGLE_ID(),
              ModelAPI_AttributeDouble, /** Angle */,
              addpoint, SketchPlugin_Circle::ADD_CONSTRUCTION_POINT_ID(),
              ModelAPI_AttributeBoolean, /** Is to add point */)

  /// Set by center and radius.
  SKETCHAPI_EXPORT
  void setByCenterAndRadius(double theCenterX, double theCenterY,
                            double theRadius, 
                            bool theIsAddPoint = false,
                            double theAngle = 0.0);

  /// Set by center and radius.
  SKETCHAPI_EXPORT
  void setByCenterAndRadius(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                            double theRadius, 
                            bool theIsAddPoint = false,
                            double theAngle = 0.0);

  /// Set by external.
  SKETCHAPI_EXPORT
  void setByExternal(const ModelHighAPI_Selection& theExternal);

  /// Set by external name.
  SKETCHAPI_EXPORT
  void setByExternalName(const std::wstring& theExternalName);

  /// Set center.
  SKETCHAPI_EXPORT
  void setCenter(double theX, double theY);

  /// Set center.
  SKETCHAPI_EXPORT
  void setCenter(const std::shared_ptr<GeomAPI_Pnt2d> & theCenter);

  /// Set radius.
  SKETCHAPI_EXPORT
  void setRadius(double theRadius);

  /// Set angle.
  SKETCHAPI_EXPORT
  void setAngle(double theAngle);

  /// Set is to add point.
  SKETCHAPI_EXPORT
  void setIsToAddPoint(bool theIsToAddPoint);

  /// Returns created points on circle
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_SketchEntity> createdPoint() const;

  /// Dump wrapped feature
  SKETCHAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;

private:
  void createPoint();
};

/// Pointer on Circle object.
typedef std::shared_ptr<SketchAPI_Circle> CirclePtr;

#endif // SketchAPI_Circle_H_
