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
class SketchAPI_Point;
class SketchAPI_Line;

/// \class SketchAPI_MacroEllipse
/// \ingroup CPPHighAPI
/// \brief Interface for Ellipse feature.
class SketchAPI_MacroEllipse: public SketchAPI_SketchEntity
{
public:
  /// Constructor without values.
  SKETCHAPI_EXPORT
  explicit SketchAPI_MacroEllipse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                  bool callInitialize = true);

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

  INTERFACE_1(SketchPlugin_MacroEllipse::ID(),
              ellipseType, SketchPlugin_MacroEllipse::ELLIPSE_TYPE(),
              ModelAPI_AttributeString, /** Ellipse type */)

  /// Return created auxiliary center point
  SKETCHAPI_EXPORT std::shared_ptr<SketchAPI_Point> center();
  /// Return created auxiliary focus in the positive direction of major axis
  SKETCHAPI_EXPORT std::shared_ptr<SketchAPI_Point> focus1();
  /// Return created auxiliary focus in the negative direction of major axis
  SKETCHAPI_EXPORT std::shared_ptr<SketchAPI_Point> focus2();
  /// Return created auxiliary point - start of major axis
  SKETCHAPI_EXPORT std::shared_ptr<SketchAPI_Point> majorAxisStart();
  /// Return created auxiliary point - end of major axis
  SKETCHAPI_EXPORT std::shared_ptr<SketchAPI_Point> majorAxisEnd();
  /// Return created auxiliary point - start of minor axis
  SKETCHAPI_EXPORT std::shared_ptr<SketchAPI_Point> minorAxisStart();
  /// Return created auxiliary point - end of minor axis
  SKETCHAPI_EXPORT std::shared_ptr<SketchAPI_Point> minorAxisEnd();
  /// Return created auxiliary major axis
  SKETCHAPI_EXPORT std::shared_ptr<SketchAPI_Line> majorAxis();
  /// Return created auxiliary minor axis
  SKETCHAPI_EXPORT std::shared_ptr<SketchAPI_Line> minorAxis();

protected:
  // find a parent sketch
  void storeSketch(const std::shared_ptr<ModelAPI_Feature>& theFeature);

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

  /// Collect auxiliary features
  void collectAuxiliary();

private:
  CompositeFeaturePtr mySketch;
  std::shared_ptr<SketchAPI_Point> myCenter;
  std::shared_ptr<SketchAPI_Point> myFocus1;
  std::shared_ptr<SketchAPI_Point> myFocus2;
  std::shared_ptr<SketchAPI_Point> myMajorAxisStart;
  std::shared_ptr<SketchAPI_Point> myMajorAxisEnd;
  std::shared_ptr<SketchAPI_Point> myMinorAxisStart;
  std::shared_ptr<SketchAPI_Point> myMinorAxisEnd;
  std::shared_ptr<SketchAPI_Line>  myMajorAxis;
  std::shared_ptr<SketchAPI_Line>  myMinorAxis;
};

/// Pointer on Circle object.
typedef std::shared_ptr<SketchAPI_MacroEllipse> MacroEllipsePtr;

#endif // SketchAPI_MacroEllipse_H_
