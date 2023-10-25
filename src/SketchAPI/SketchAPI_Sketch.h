// Copyright (C) 2014-2023  CEA, EDF
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

#ifndef SRC_SKETCHAPI_SKETCHAPI_SKETCH_H_
#define SRC_SKETCHAPI_SKETCHAPI_SKETCH_H_

//--------------------------------------------------------------------------------------
#include "SketchAPI.h"

#include <list>

#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_SketchEntity.h>
#include <SketchPlugin_Offset.h>

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>
#include <ModelHighAPI_Selection.h>
//--------------------------------------------------------------------------------------
class ModelAPI_CompositeFeature;
class ModelAPI_Object;
class ModelHighAPI_Integer;
class ModelHighAPI_RefAttr;
class ModelHighAPI_Reference;
class SketchAPI_Arc;
class SketchAPI_MacroArc;
class SketchAPI_Circle;
class SketchAPI_MacroCircle;
class SketchAPI_Ellipse;
class SketchAPI_MacroEllipse;
class SketchAPI_EllipticArc;
class SketchAPI_MacroEllipticArc;
class SketchAPI_BSpline;
class SketchAPI_IntersectionPoint;
class SketchAPI_Line;
class SketchAPI_Mirror;
class SketchAPI_Offset;
class SketchAPI_Point;
class SketchAPI_Projection;
class SketchAPI_Rectangle;
class SketchAPI_Rotation;
class SketchAPI_Translation;
class SketchAPI_MacroMiddlePoint;

//--------------------------------------------------------------------------------------
typedef std::pair<std::shared_ptr<GeomAPI_Pnt2d>, ModelHighAPI_RefAttr> PointOrReference;
//--------------------------------------------------------------------------------------
/**\class SketchAPI_Sketch
 * \ingroup CPPHighAPI
 * \brief Interface for Sketch feature
 */
class SketchAPI_Sketch : public ModelHighAPI_Interface
{
public:
  /// Constructor without values
  SKETCHAPI_EXPORT
  explicit SketchAPI_Sketch(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_Sketch(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                   const std::shared_ptr<GeomAPI_Ax3> & thePlane);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_Sketch(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                   const ModelHighAPI_Selection & theExternal);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_Sketch(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                   std::shared_ptr<ModelAPI_Object> thePlaneObject);
  /// Destructor
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_Sketch();

  INTERFACE_7(SketchPlugin_Sketch::ID(),
              origin, SketchPlugin_Sketch::ORIGIN_ID(),
              GeomDataAPI_Point, /** Origin point */,
              dirX, SketchPlugin_Sketch::DIRX_ID(),
              GeomDataAPI_Dir, /** Direction of X */,
              normal, SketchPlugin_Sketch::NORM_ID(),
              GeomDataAPI_Dir, /** Normal */,
              features, SketchPlugin_Sketch::FEATURES_ID(),
              ModelAPI_AttributeRefList, /** Features */,
              external, SketchPlugin_SketchEntity::EXTERNAL_ID(),
              ModelAPI_AttributeSelection, /** External */,
              solverError, SketchPlugin_Sketch::SOLVER_ERROR(),
              ModelAPI_AttributeString, /** Solver error */,
              solverDOF, SketchPlugin_Sketch::SOLVER_DOF(),
              ModelAPI_AttributeString, /** Solver DOF */
  )

  /// Set plane
  SKETCHAPI_EXPORT
  void setPlane(const std::shared_ptr<GeomAPI_Ax3> & thePlane);

  /// Change sketch plane
  SKETCHAPI_EXPORT
  void setPlane(const ModelHighAPI_Selection & thePlane,
                bool theRemoveExternalDependency = false);

  /// Set external
  SKETCHAPI_EXPORT
  void setExternal(const ModelHighAPI_Selection & theExternal);

  /// Set external
  SKETCHAPI_EXPORT
  void setExternal(std::shared_ptr<ModelAPI_Object> thePlaneObject);

  /// Change order of sketch results (faces)
  SKETCHAPI_EXPORT
  void changeFacesOrder(const std::list<std::list<ModelHighAPI_Selection> >& theFaces);

  /// List points not connected by constraints with other sketch entitites
  SKETCHAPI_EXPORT
  std::list< std::shared_ptr<SketchAPI_Point> > getFreePoints();

  /// Add point
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Point> addPoint(
      double theX, double theY);
  /// Add point
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Point> addPoint(
      const std::shared_ptr<GeomAPI_Pnt2d> & thePoint);
  /// Add point
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Point> addPoint(const ModelHighAPI_Selection & theExternal);
  /// Add point
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Point> addPoint(const std::wstring & theExternalName);

  /// Add intersection point
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_IntersectionPoint>
    addIntersectionPoint(const ModelHighAPI_Selection & theExternal,
                         bool theKeepResult = false);
  /// Add point
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_IntersectionPoint>
    addIntersectionPoint(const std::wstring & theExternalName,
                         bool theKeepResult = false);

  /// Add line
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Line> addLine(
      double theX1, double theY1, double theX2, double theY2);
  /// Add line
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Line> addLine(
      const std::shared_ptr<GeomAPI_Pnt2d> & theStartPoint,
      const std::shared_ptr<GeomAPI_Pnt2d> & theEndPoint);
  /// Add line
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Line> addLine(const ModelHighAPI_Selection & theExternal);
  /// Add line
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Line> addLine(const std::wstring & theExternalName);

  /// Add rectangle
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Rectangle> addRectangle(
      double theX1, double theY1, double theX2, double theY2);
  /// Add rectangle
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Rectangle> addRectangle(
      const std::shared_ptr<GeomAPI_Pnt2d> & theStartPoint,
      const std::shared_ptr<GeomAPI_Pnt2d> & theEndPoint);
  /// Add rectangle
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Rectangle> addRectangleCentered(
      const std::pair<std::shared_ptr<GeomAPI_Pnt2d>, ModelHighAPI_RefAttr> & theCenter,
      const std::pair<std::shared_ptr<GeomAPI_Pnt2d>, ModelHighAPI_RefAttr> & theCorner);

  /// Add circle.
  /// [DEPRECATED]: use addCircleWithPoint() instead.
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Circle> addCircle(
      double theCenterX, double theCenterY,
      double theRadius);
  /// Add circle.
  /// [DEPRECATED]: use addCircleWithPoint() instead.
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Circle> addCircle(
      const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
      double theRadius);
  /// Add circle.
  /// [DEPRECATED]: use addCircleWithPoint() instead.
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_MacroCircle> addCircle(
      double theCenterX, double theCenterY,
      double thePassedX, double thePassedY);
  /// Add circle.
  /// [DEPRECATED]: use addCircleWithPoint() instead.
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_MacroCircle> addCircle(
      const std::shared_ptr<GeomAPI_Pnt2d>& theCenterPoint,
      const std::shared_ptr<GeomAPI_Pnt2d>& thePassedPoint);
  /// Add circle.
  /// [DEPRECATED]: use addCircleWithPoint() instead.
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_MacroCircle> addCircle(
      double theX1, double theY1,
      double theX2, double theY2,
      double theX3, double theY3);
  /// Add circle.
  /// [DEPRECATED]: use addCircleWithPoint() instead.
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_MacroCircle> addCircle(
      const std::shared_ptr<GeomAPI_Pnt2d>& thePoint1,
      const std::shared_ptr<GeomAPI_Pnt2d>& thePoint2,
      const std::shared_ptr<GeomAPI_Pnt2d>& thePoint3);
  /// Add circle.
  /// [DEPRECATED]: use addCircleWithPoint() instead.
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Circle> addCircle(const ModelHighAPI_Selection & theExternal);
  /// Add circle.
  /// [DEPRECATED]: use addCircleWithPoint() instead.
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Circle> addCircle(const std::wstring & theExternalName);

  /// Add circle with point
  SKETCHAPI_EXPORT
    std::shared_ptr<SketchAPI_Circle> addCircleWithPoint(
      double theCenterX, double theCenterY,
      double theRadius, double theAngle);
  /// Add circle with point
  SKETCHAPI_EXPORT
    std::shared_ptr<SketchAPI_Circle> addCircleWithPoint(
      const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
      double theRadius, double theAngle);
  /// Add circle with point
  SKETCHAPI_EXPORT
    std::shared_ptr<SketchAPI_MacroCircle> addCircleWithPoint(
      double theCenterX, double theCenterY,
      double thePassedX, double thePassedY,
      double theAngle);
  /// Add circle with point
  SKETCHAPI_EXPORT
    std::shared_ptr<SketchAPI_MacroCircle> addCircleWithPoint(
      const std::shared_ptr<GeomAPI_Pnt2d>& theCenterPoint,
      const std::shared_ptr<GeomAPI_Pnt2d>& thePassedPoint,
      double theAngle);
  /// Add circle with point
  SKETCHAPI_EXPORT
    std::shared_ptr<SketchAPI_MacroCircle> addCircleWithPoint(
      double theX1, double theY1,
      double theX2, double theY2,
      double theX3, double theY3,
      double theAngle);
  /// Add circle with point
  SKETCHAPI_EXPORT
    std::shared_ptr<SketchAPI_MacroCircle> addCircleWithPoint(
      const std::shared_ptr<GeomAPI_Pnt2d>& thePoint1,
      const std::shared_ptr<GeomAPI_Pnt2d>& thePoint2,
      const std::shared_ptr<GeomAPI_Pnt2d>& thePoint3,
      double theAngle);
  /// Add circle with point
  SKETCHAPI_EXPORT
    std::shared_ptr<SketchAPI_Circle> addCircleWithPoint(const ModelHighAPI_Selection& theExternal);
  /// Add circle with point
  SKETCHAPI_EXPORT
    std::shared_ptr<SketchAPI_Circle> addCircleWithPoint(const std::wstring& theExternalName);

  /// Add arc
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Arc> addArc(
      double theCenterX, double theCenterY,
      double theStartX, double theStartY,
      double theEndX, double theEndY,
      bool theInversed);

  /// Add arc
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Arc> addArc(
      const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
      const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
      const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
      bool theInversed);

  /// Add arc
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_MacroArc> addArc(
      double theStartX, double theStartY,
      double theEndX, double theEndY,
      double thePassedX, double thePassedY);

  /// Add arc
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_MacroArc> addArc(
      const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
      const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
      const std::shared_ptr<GeomAPI_Pnt2d>& thePassed);

  /// Add transversal/tangent arc
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_MacroArc> addArc(
      const ModelHighAPI_RefAttr& theConnectedPoint,
      double theEndX, double theEndY,
      bool theInversed,
      bool theTransversal = false);

  /// Add transversal/tangent arc
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_MacroArc> addArc(
      const ModelHighAPI_RefAttr& theConnectedPoint,
      const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
      bool theInversed,
      bool theTransversal = false);

  /// Add arc
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Arc> addArc(const ModelHighAPI_Selection & theExternal);

  /// Add arc
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Arc> addArc(const std::wstring & theExternalName);

  /// Add ellipse
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Ellipse> addEllipse(
      double theCenterX, double theCenterY,
      double theFocusX, double theFocusY,
      double theMinorRadius);
  /// Add ellipse
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Ellipse> addEllipse(
      const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
      const std::shared_ptr<GeomAPI_Pnt2d>& theFocus,
      double theRadius);
  /// Add ellipse
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_MacroEllipse> addEllipse(
      double thePoint1X, double thePoint1Y,
      double thePoint2X, double thePoint2Y,
      double thePassedX, double thePassedY,
      bool isPoint1Center = true);
  /// Add ellipse
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_MacroEllipse> addEllipse(
      const PointOrReference& thePoint1,
      const PointOrReference& thePoint2,
      const PointOrReference& thePassedPoint,
      bool isPoint1Center = true);
  /// Add ellipse
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Ellipse> addEllipse(const ModelHighAPI_Selection & theExternal);
  /// Add ellipse
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Ellipse> addEllipse(const std::wstring & theExternalName);

  /// Add elliptic arc
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_EllipticArc> addEllipticArc(
      double theCenterX, double theCenterY,
      double theFocusX, double theFocusY,
      double theStartX, double theStartY,
      double theEndX, double theEndY,
      bool theInversed = false);
  /// Add elliptic arc
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_MacroEllipticArc> addEllipticArc(
      const PointOrReference& theCenter,
      const PointOrReference& theMajorAxisPoint,
      const PointOrReference& theStartPoint,
      const PointOrReference& theEndPoint,
      bool theInversed = false);
  /// Add elliptic arc
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_EllipticArc> addEllipticArc(const ModelHighAPI_Selection & theExternal);
  /// Add elliptic arc
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_EllipticArc> addEllipticArc(const std::wstring & theExternalName);

  /// Add B-spline
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_BSpline> addSpline(
      const ModelHighAPI_Selection & external = ModelHighAPI_Selection(),
      const int degree = -1,
      const std::list<PointOrReference>& poles = std::list<PointOrReference>(),
      const std::list<ModelHighAPI_Double>& weights = std::list<ModelHighAPI_Double>(),
      const std::list<ModelHighAPI_Double>& knots = std::list<ModelHighAPI_Double>(),
      const std::list<ModelHighAPI_Integer>& multiplicities = std::list<ModelHighAPI_Integer>(),
      const bool periodic = false);

  /// Add interpolation feature
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_BSpline> addInterpolation(
      const std::list<ModelHighAPI_RefAttr>& points,
      const bool periodic = false,
      const bool closed = false);

  /// Add approximation feature
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_BSpline> addApproximation(
      const std::list<ModelHighAPI_RefAttr>& points,
      const ModelHighAPI_Double& precision = ModelHighAPI_Double(1.e-3),
      const bool periodic = false,
      const bool closed = false);

  /// Add projection
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Projection> addProjection(
      const ModelHighAPI_Selection & theExternalFeature,
      bool keepResult = false,
      bool keepRefToOriginal = true);

  /// Add mirror
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Mirror> addMirror(
      const ModelHighAPI_RefAttr & theMirrorLine,
      const std::list<std::shared_ptr<ModelAPI_Object> > & theObjects);

  /// Add offset
  SKETCHAPI_EXPORT
    std::shared_ptr<SketchAPI_Offset> addOffset(
      const std::list<std::shared_ptr<ModelAPI_Object> > & theObjects,
      const ModelHighAPI_Double & theValue,
      const bool theReversed = false,
      const std::string & theJointType = SketchPlugin_Offset::JOINT_KEEP_DISTANCE(),
      const bool theApprox = false);

  /// Add translation
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Translation> addTranslation(
      const std::list<std::shared_ptr<ModelAPI_Object> > & theObjects,
      const ModelHighAPI_RefAttr & thePoint1,
      const ModelHighAPI_RefAttr & thePoint2,
      const ModelHighAPI_Integer & theNumberOfObjects,
      bool theFullValue = false);

  /// Add rotation
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Rotation> addRotation(
      const std::list<std::shared_ptr<ModelAPI_Object> > & theObjects,
      const ModelHighAPI_RefAttr & theCenter,
      const ModelHighAPI_Double & theAngle,
      const ModelHighAPI_Integer & theNumberOfObjects,
      bool theFullValue = false,
      bool theReversed  = false);

  /// Add split
  SKETCHAPI_EXPORT
  std::shared_ptr<ModelHighAPI_Interface> addSplit(
      const ModelHighAPI_Reference& theFeature,
      const std::shared_ptr<GeomAPI_Pnt2d>& thePositionPoint);

  /// Add trim
  SKETCHAPI_EXPORT
  std::shared_ptr<ModelHighAPI_Interface> addTrim(
      const ModelHighAPI_Reference& theFeature,
      const std::shared_ptr<GeomAPI_Pnt2d>& thePositionPoint);

  /// Set angle
  SKETCHAPI_EXPORT
  std::shared_ptr<ModelHighAPI_Interface> setAngle(
      const ModelHighAPI_RefAttr & theLine1,
      const ModelHighAPI_RefAttr & theLine2,
      const ModelHighAPI_Double & theValue,
      const std::string& type = std::string());

  /// Set complementary angle
  SKETCHAPI_EXPORT
  std::shared_ptr<ModelHighAPI_Interface> setAngleComplementary(
      const ModelHighAPI_RefAttr & theLine1,
      const ModelHighAPI_RefAttr & theLine2,
      const ModelHighAPI_Double & theValue);

  /// Set backward angle (= 360 - angle)
  SKETCHAPI_EXPORT
  std::shared_ptr<ModelHighAPI_Interface> setAngleBackward(
      const ModelHighAPI_RefAttr & theLine1,
      const ModelHighAPI_RefAttr & theLine2,
      const ModelHighAPI_Double & theValue);

  /// Set coincident
  SKETCHAPI_EXPORT
  std::shared_ptr<ModelHighAPI_Interface> setCoincident(
      const ModelHighAPI_RefAttr & thePoint1,
      const ModelHighAPI_RefAttr & thePoint2);

  /// Set collinear
  SKETCHAPI_EXPORT
  std::shared_ptr<ModelHighAPI_Interface> setCollinear(
      const ModelHighAPI_RefAttr & theLine1,
      const ModelHighAPI_RefAttr & theLine2);

  /// Set distance
  SKETCHAPI_EXPORT
  std::shared_ptr<ModelHighAPI_Interface> setDistance(
      const ModelHighAPI_RefAttr & thePoint,
      const ModelHighAPI_RefAttr & thePointOrLine,
      const ModelHighAPI_Double & theValue,
      bool isSigned = false);

  /// Set signed distance
  SKETCHAPI_EXPORT
  std::shared_ptr<ModelHighAPI_Interface> setSignedDistance(
      const ModelHighAPI_RefAttr & thePoint,
      const ModelHighAPI_RefAttr & thePointOrLine,
      const ModelHighAPI_Double & theValue);

  /// Set unsigned distance
  SKETCHAPI_EXPORT
  std::shared_ptr<ModelHighAPI_Interface> setUnsignedDistance(
      const ModelHighAPI_RefAttr & thePoint,
      const ModelHighAPI_RefAttr & thePointOrLine,
      const ModelHighAPI_Double & theValue);

  /// Set horizontal distance
  SKETCHAPI_EXPORT
  std::shared_ptr<ModelHighAPI_Interface> setHorizontalDistance(
      const ModelHighAPI_RefAttr & thePoint1,
      const ModelHighAPI_RefAttr & thePoint2,
      const ModelHighAPI_Double & theValue);

  /// Set vertical distance
  SKETCHAPI_EXPORT
  std::shared_ptr<ModelHighAPI_Interface> setVerticalDistance(
      const ModelHighAPI_RefAttr & thePoint1,
      const ModelHighAPI_RefAttr & thePoint2,
      const ModelHighAPI_Double & theValue);

  /// Set equal
  SKETCHAPI_EXPORT
  std::shared_ptr<ModelHighAPI_Interface> setEqual(
      const ModelHighAPI_RefAttr & theObject1,
      const ModelHighAPI_RefAttr & theObject2);

  /// Set fillet
  SKETCHAPI_EXPORT
  std::shared_ptr<ModelHighAPI_Interface> setFillet(
      const ModelHighAPI_RefAttr & thePoint);

  /// Set fillet with additional radius constraint
  SKETCHAPI_EXPORT
  std::shared_ptr<ModelHighAPI_Interface> setFilletWithRadius(
      const ModelHighAPI_RefAttr & thePoint,
      const ModelHighAPI_Double & theRadius);

  /// Set fixed
  SKETCHAPI_EXPORT
  std::shared_ptr<ModelHighAPI_Interface> setFixed(
      const ModelHighAPI_RefAttr & theObject);

  /// Set horizontal
  SKETCHAPI_EXPORT
  std::shared_ptr<ModelHighAPI_Interface> setHorizontal(
      const ModelHighAPI_RefAttr & theLine);

  /// Set length
  SKETCHAPI_EXPORT
  std::shared_ptr<ModelHighAPI_Interface> setLength(
      const ModelHighAPI_RefAttr & theLine,
      const ModelHighAPI_Double & theValue);

  /// Set middle
  SKETCHAPI_EXPORT
  std::shared_ptr<ModelHighAPI_Interface> setMiddlePoint(
      const ModelHighAPI_RefAttr & thePoint,
      const ModelHighAPI_RefAttr & theLine);

  /// Set middle
  SKETCHAPI_EXPORT
    std::shared_ptr<SketchAPI_MacroMiddlePoint> setMiddlePoint(
      const ModelHighAPI_RefAttr& theLine);

  /// Set parallel
  SKETCHAPI_EXPORT
  std::shared_ptr<ModelHighAPI_Interface> setParallel(
      const ModelHighAPI_RefAttr & theLine1,
      const ModelHighAPI_RefAttr & theLine2);

  /// Set perpendicular
  SKETCHAPI_EXPORT
  std::shared_ptr<ModelHighAPI_Interface> setPerpendicular(
      const ModelHighAPI_RefAttr & theLine1,
      const ModelHighAPI_RefAttr & theLine2);

  /// Set radius
  SKETCHAPI_EXPORT
  std::shared_ptr<ModelHighAPI_Interface> setRadius(
      const ModelHighAPI_RefAttr & theCircleOrArc,
      const ModelHighAPI_Double & theValue);

  /// Set tangent
  SKETCHAPI_EXPORT
  std::shared_ptr<ModelHighAPI_Interface> setTangent(
      const ModelHighAPI_RefAttr & theLine,
      const ModelHighAPI_RefAttr & theCircle);

  /// Set vertical
  SKETCHAPI_EXPORT
  std::shared_ptr<ModelHighAPI_Interface> setVertical(
      const ModelHighAPI_RefAttr & theLine);

  /// Set constraint value
  SKETCHAPI_EXPORT
  void setValue(
      const std::shared_ptr<ModelHighAPI_Interface> & theConstraint,
      const ModelHighAPI_Double & theValue);

  /// Move point or sketch feature
  SKETCHAPI_EXPORT
  void move(const ModelHighAPI_RefAttr& theMovedEntity,
            const std::shared_ptr<GeomAPI_Pnt2d>& theTargetPoint);

  /// Move point or sketch feature
  SKETCHAPI_EXPORT
  void move(const ModelHighAPI_RefAttr& theMovedEntity,
            double theTargetX, double theTargetY);

  SKETCHAPI_EXPORT
  std::shared_ptr<GeomAPI_Pnt2d> to2D(const std::shared_ptr<GeomAPI_Pnt>& thePoint);

  // TODO(spo): rename to selectFaces() or faces() (or add faces() -> list to SWIG)
  /// Select face
  SKETCHAPI_EXPORT
  std::list<ModelHighAPI_Selection> selectFace() const;

  /// Dump wrapped feature
  SKETCHAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;

protected:
  std::shared_ptr<ModelAPI_CompositeFeature> compositeFeature() const;

};

//! Pointer on Sketch object
typedef std::shared_ptr<SketchAPI_Sketch> SketchPtr;

//--------------------------------------------------------------------------------------

/**\ingroup CPPHighAPI
 * \brief Create Sketch feature
 */
SKETCHAPI_EXPORT
SketchPtr addSketch(const std::shared_ptr<ModelAPI_Document> & thePart,
                    const std::shared_ptr<GeomAPI_Ax3> & thePlane);

/**\ingroup CPPHighAPI
 * \brief Create Sketch feature
 */
SKETCHAPI_EXPORT
SketchPtr addSketch(const std::shared_ptr<ModelAPI_Document> & thePart,
                    const ModelHighAPI_Selection & theExternal);

/**\ingroup CPPHighAPI
 * \brief Create Sketch feature
 */
SKETCHAPI_EXPORT
SketchPtr addSketch(const std::shared_ptr<ModelAPI_Document> & thePart,
                    const std::wstring & theExternalName);

/**\ingroup CPPHighAPI
 * \brief Create Sketch feature
 */
SKETCHAPI_EXPORT
SketchPtr addSketch(const std::shared_ptr<ModelAPI_Document> & thePart,
                    std::shared_ptr<ModelAPI_Object> thePlaneObject);

//--------------------------------------------------------------------------------------

/** \ingroup CPPHighAPI
 *  \brief Copy sketch with all its sub-features
 */
SKETCHAPI_EXPORT
SketchPtr copySketch(const std::shared_ptr<ModelAPI_Document> & thePart,
                     const SketchPtr & theSketch);

//--------------------------------------------------------------------------------------
#endif /* SRC_SKETCHAPI_SKETCHAPI_SKETCH_H_ */
