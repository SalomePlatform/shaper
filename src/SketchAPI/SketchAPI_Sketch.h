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

#ifndef SRC_SKETCHAPI_SKETCHAPI_SKETCH_H_
#define SRC_SKETCHAPI_SKETCHAPI_SKETCH_H_

//--------------------------------------------------------------------------------------
#include "SketchAPI.h"

#include <list>

#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_SketchEntity.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>
//--------------------------------------------------------------------------------------
class ModelAPI_CompositeFeature;
class ModelAPI_Object;
class ModelHighAPI_Double;
class ModelHighAPI_Integer;
class ModelHighAPI_RefAttr;
class ModelHighAPI_Reference;
class ModelHighAPI_Selection;
class SketchAPI_Arc;
class SketchAPI_MacroArc;
class SketchAPI_Circle;
class SketchAPI_MacroCircle;
class SketchAPI_IntersectionPoint;
class SketchAPI_Line;
class SketchAPI_Mirror;
class SketchAPI_Point;
class SketchAPI_Projection;
class SketchAPI_Rectangle;
class SketchAPI_Rotation;
class SketchAPI_Translation;
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
  std::shared_ptr<SketchAPI_Point> addPoint(const std::string & theExternalName);

  /// Add intersection point
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_IntersectionPoint>
    addIntersectionPoint(const ModelHighAPI_Selection & theExternal,
                         bool theKeepResult = false);
  /// Add point
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_IntersectionPoint>
    addIntersectionPoint(const std::string & theExternalName,
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
  std::shared_ptr<SketchAPI_Line> addLine(const std::string & theExternalName);

  /// Add rectangle
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Rectangle> addRectangle(
      double theX1, double theY1, double theX2, double theY2);
  /// Add rectangle
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Rectangle> addRectangle(
      const std::shared_ptr<GeomAPI_Pnt2d> & theStartPoint,
      const std::shared_ptr<GeomAPI_Pnt2d> & theEndPoint);

  /// Add circle
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Circle> addCircle(
      double theCenterX, double theCenterY,
      double theRadius);
  /// Add circle
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Circle> addCircle(
      const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
      double theRadius);
  /// Add circle
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_MacroCircle> addCircle(
      double theCenterX, double theCenterY,
      double thePassedX, double thePassedY);
  /// Add circle
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_MacroCircle> addCircle(
      const std::shared_ptr<GeomAPI_Pnt2d>& theCenterPoint,
      const std::shared_ptr<GeomAPI_Pnt2d>& thePassedPoint);
  /// Add circle
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_MacroCircle> addCircle(
      double theX1, double theY1,
      double theX2, double theY2,
      double theX3, double theY3);
  /// Add circle
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_MacroCircle> addCircle(
      const std::shared_ptr<GeomAPI_Pnt2d>& thePoint1,
      const std::shared_ptr<GeomAPI_Pnt2d>& thePoint2,
      const std::shared_ptr<GeomAPI_Pnt2d>& thePoint3);
  /// Add circle
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Circle> addCircle(const ModelHighAPI_Selection & theExternal);
  /// Add circle
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Circle> addCircle(const std::string & theExternalName);

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
  std::shared_ptr<SketchAPI_Arc> addArc(const std::string & theExternalName);

  /// Add projection
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Projection> addProjection(
      const ModelHighAPI_Selection & theExternalFeature,
      bool theKeepResult = false);

  /// Add projection
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Projection> addProjection(const std::string & theExternalName,
                                                      bool theKeepResult = false);

  /// Add mirror
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_Mirror> addMirror(
      const ModelHighAPI_RefAttr & theMirrorLine,
      const std::list<std::shared_ptr<ModelAPI_Object> > & theObjects);

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
      const ModelHighAPI_Double & theValue);

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
                    const std::string & theExternalName);

/**\ingroup CPPHighAPI
 * \brief Create Sketch feature
 */
SKETCHAPI_EXPORT
SketchPtr addSketch(const std::shared_ptr<ModelAPI_Document> & thePart,
                    std::shared_ptr<ModelAPI_Object> thePlaneObject);

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_SKETCHAPI_SKETCHAPI_SKETCH_H_ */
