// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef SketchAPI_MacroEllipse_H_
#define SketchAPI_MacroEllipse_H_

#include "SketchAPI.h"
#include "SketchAPI_SketchEntity.h"

#include <SketchPlugin_MacroEllipse.h>

class ModelHighAPI_RefAttr;

/// \class SketchAPI_MacroEllipse
/// \ingroup CPPHighAPI
/// \brief Interface for Ellipse feature.
class SketchAPI_MacroEllipse: public SketchAPI_SketchEntity
{
public:
  /// Constructor without values.
  SKETCHAPI_EXPORT
  explicit SketchAPI_MacroEllipse(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_MacroEllipse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                         double theX1, double theY1,
                         double theX2, double theY2,
                         double theX3, double theY3,
                         bool byCenter = true);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_MacroEllipse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                         const std::shared_ptr<GeomAPI_Pnt2d>& thePoint1,
                         const std::shared_ptr<GeomAPI_Pnt2d>& thePoint2,
                         const std::shared_ptr<GeomAPI_Pnt2d>& thePoint3,
                         bool byCenter = true);

  /// Constructor with values and/or references.
  SKETCHAPI_EXPORT
  SketchAPI_MacroEllipse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                         const std::shared_ptr<GeomAPI_Pnt2d>& thePoint1,
                         const ModelHighAPI_RefAttr&           thePoint1Ref,
                         const std::shared_ptr<GeomAPI_Pnt2d>& thePoint2,
                         const ModelHighAPI_RefAttr&           thePoint2Ref,
                         const std::shared_ptr<GeomAPI_Pnt2d>& thePoint3,
                         const ModelHighAPI_RefAttr&           thePoint3Ref,
                         bool byCenter = true);

  /// Destructor.
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_MacroEllipse();

  INTERFACE_9(SketchPlugin_MacroEllipse::ID(),
              ellipseType, SketchPlugin_MacroEllipse::ELLIPSE_TYPE(),
              ModelAPI_AttributeString, /** Ellipse type */,
              centerPoint, SketchPlugin_MacroEllipse::FIRST_POINT_ID(),
              GeomDataAPI_Point2D, /** Center point */,
              centerPointRef, SketchPlugin_MacroEllipse::FIRST_POINT_REF_ID(),
              ModelAPI_AttributeRefAttr, /** Reference to a center point */,
              majorAxisNegativePoint, SketchPlugin_MacroEllipse::FIRST_POINT_ID(),
              GeomDataAPI_Point2D, /** Major axis negative point point */,
              majorAxisNegativePointRef, SketchPlugin_MacroEllipse::FIRST_POINT_REF_ID(),
              ModelAPI_AttributeRefAttr, /** Reference to the negative point on a major axis */,
              majorAxisPositivePoint, SketchPlugin_MacroEllipse::SECOND_POINT_ID(),
              GeomDataAPI_Point2D, /** Major axis positive point point */,
              majorAxisPositivePointRef, SketchPlugin_MacroEllipse::SECOND_POINT_REF_ID(),
              ModelAPI_AttributeRefAttr, /** Reference to the positive point on a major axis */,
              passedPoint, SketchPlugin_MacroEllipse::PASSED_POINT_ID(),
              GeomDataAPI_Point2D, /** Passed point */,
              passedPointRef, SketchPlugin_MacroEllipse::PASSED_POINT_REF_ID(),
              ModelAPI_AttributeRefAttr, /** Reference to a passed point */)

private:
  /// Set flag of creation by center, major semi-axis and passed point.
  void setByCenterAndPassedPoints();
  /// Set flag of creation by major axis and passed point.
  void setByMajorAxisAndPassedPoint();

  /// Set points of ellipse.
  void initializePoints(double theMajorAxisX1, double theMajorAxisY1,
                        double theMajorAxisX2, double theMajorAxisY2,
                        double thePassedX, double thePassedY);

  /// Set points of ellipse.
  void initializePoints(const std::shared_ptr<GeomAPI_Pnt2d>& theMajorAxisPoint1,
                        const std::shared_ptr<GeomAPI_Pnt2d>& theMajorAxisPoint2,
                        const std::shared_ptr<GeomAPI_Pnt2d>& thePassedPoint);

  /// Set points of ellipse.
  void initializePoints(const std::shared_ptr<GeomAPI_Pnt2d>& theMajorAxisPoint1,
                        const ModelHighAPI_RefAttr&           theMajorAxisPoint1Ref,
                        const std::shared_ptr<GeomAPI_Pnt2d>& theMajorAxisPoint2,
                        const ModelHighAPI_RefAttr&           theMajorAxisPoint2Ref,
                        const std::shared_ptr<GeomAPI_Pnt2d>& thePassedPoint,
                        const ModelHighAPI_RefAttr&           thePassedPointRef);
};

/// Pointer on Circle object.
typedef std::shared_ptr<SketchAPI_MacroEllipse> MacroEllipsePtr;

#endif // SketchAPI_MacroEllipse_H_
