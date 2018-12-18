// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

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

private:
  /// Set by center and passed points.
  void setByCenterAndPassedPoints(double theCenterX, double theCenterY,
                                  double thePassedX, double thePassedY);

  /// Set by center and passed points.
  void setByCenterAndPassedPoints(const std::shared_ptr<GeomAPI_Pnt2d>& theCenterPoint,
                                  const std::shared_ptr<GeomAPI_Pnt2d>& thePassedPoint);

  /// Set by three points.
  void setByThreePoints(double theX1, double theY1,
                        double theX2, double theY2,
                        double theX3, double theY3);

  /// Set by three points.
  void setByThreePoints(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint1,
                        const std::shared_ptr<GeomAPI_Pnt2d>& thePoint2,
                        const std::shared_ptr<GeomAPI_Pnt2d>& thePoint3);
};

/// Pointer on Circle object.
typedef std::shared_ptr<SketchAPI_MacroCircle> MacroCirclePtr;

#endif // SketchAPI_MacroCircle_H_
