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

#ifndef GEOMALGOAPI_SHAPEAPI_H
#define GEOMALGOAPI_SHAPEAPI_H

#include <GeomAPI_Ax1.h>
#include <GeomAPI_Ax2.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Shape.h>

#include <GeomAlgoAPI_Exception.h>

#ifdef WIN32
#pragma warning(disable : 4290)
#endif

namespace GeomAlgoAPI_ShapeAPI
{
/**\class GeomAlgoAPI_ShapeAPI
 * \ingroup DataAlgo
 * \brief Allows to access the direct API
 */
class GEOMALGOAPI_EXPORT GeomAlgoAPI_ShapeAPI
{
public:
  /// Creates a box using the dimensions.
  /// \param theDx The dimension on X
  /// \param theDy The dimension on Y
  /// \param theDz The dimension on Z
  /// \return a shape
  static std::shared_ptr<GeomAPI_Shape> makeBox(const double theDx, const double theDy,
                                                const double theDz);

  /// Creates a box using the two points that defined a diagonal.
  /// \param theFirstPoint One extermity of the diagonal
  /// \param theSecondPoint The other extremity of the diagonal
  /// \return a shape
  static std::shared_ptr<GeomAPI_Shape> makeBox(std::shared_ptr<GeomAPI_Pnt> theFirstPoint,
                     std::shared_ptr<GeomAPI_Pnt> theSecondPoint);

  /// Creates a cylinder using a center, an axis, a radius and a height.
  /// \param theBasePoint The center of the lower base of the cylinder
  /// \param theEdge The axis of the cylinder
  /// \param theRadius The radius of the cylinder
  /// \param theHeight The heigth of the cylinder
  static std::shared_ptr<GeomAPI_Shape> makeCylinder(std::shared_ptr<GeomAPI_Pnt> theBasePoint,
                     std::shared_ptr<GeomAPI_Edge> theEdge, double theRadius, double theHeight);

  /// Creates a portion of cylinder using a center, an axis, a radius, a height and an angle.
  /// \param theBasePoint The center of the lower base of the cylinder
  /// \param theEdge The axis of the cylinder
  /// \param theRadius The radius of the cylinder
  /// \param theHeight The heigth of the cylinder
  /// \param theAngle The angle defining the portion
  static std::shared_ptr<GeomAPI_Shape> makeCylinder(std::shared_ptr<GeomAPI_Pnt> theBasePoint,
                     std::shared_ptr<GeomAPI_Edge> theEdge, double theRadius, double theHeight,
                     double theAngle);

  /// Creates a cylinder using the origin, the OZ axis, a radius and a height.
  /// \param theRadius The radius of the cylinder
  /// \param theHeight The heigth of the cylinder
  static std::shared_ptr<GeomAPI_Shape> makeCylinder(double theRadius, double theHeight);

  /// Creates a portion of cylinder using the origin, the OZ axis, a radius, a height and an angle.
  /// \param theRadius The radius of the cylinder
  /// \param theHeight The heigth of the cylinder
  /// \param theAngle The angle defining the portion
  static std::shared_ptr<GeomAPI_Shape> makeCylinder(double theRadius, double theHeight,
                     double theAngle);

  /// Creates a sphere using a center and a radius.
  /// \param theCenterPoint The center of the sphere
  /// \param theRadius The radius of the sphere
  static std::shared_ptr<GeomAPI_Shape> makeSphere(std::shared_ptr<GeomAPI_Pnt> theCenterPoint,
                     double theRadius);

  /// Creates a sphere using the origin and a radius.
  /// \param theRadius The radius of the sphere
  static std::shared_ptr<GeomAPI_Shape> makeSphere(double theRadius);

  /// Creates a torus using a base point, an axis, a radius and a ring radius.
  /// \param theBasePoint The center of the torus
  /// \param theEdge The axis of the torus
  /// \param theRadius The radius of the torus
  /// \param theRingRadius The ring radius of the torus
  static std::shared_ptr<GeomAPI_Shape> makeTorus(std::shared_ptr<GeomAPI_Pnt> theBasePoint,
                     std::shared_ptr<GeomAPI_Edge> theEdge, double theRadius, double theRingRadius);

  /// Creates a torus using a radius and a ring radius.
  /// \param theRadius The radius of the torus
  /// \param theRingRadius The ring radius of the torus
  static std::shared_ptr<GeomAPI_Shape> makeTorus(double theRadius, double theRingRadius);

  /// Creates a cone using a base point, an axis, a base radius, a top radius and a height.
  /// \param theBasePoint The center of the lower base of the cone
  /// \param theEdge The axis of the cone
  /// \param theBaseRadius The base radius of the cone
  /// \param theTopRadius The top radius of the cone
  /// \param theHeight The height of the cone
  static std::shared_ptr<GeomAPI_Shape> makeCone(std::shared_ptr<GeomAPI_Pnt> theBasePoint,
                     std::shared_ptr<GeomAPI_Edge> theEdge, double theBaseRadius,
                     double theTopRadius, double theHeight);

  /// Creates a cone using a base radius, a top radius and a height.
  /// \param theBaseRadius The base radius of the cone
  /// \param theTopRadius The top radius of the cone
  /// \param theHeight The height of the cone
  static std::shared_ptr<GeomAPI_Shape> makeCone(double theBaseRadius, double theTopRadius,
                     double theHeight);

  /// Performs a translation from an axis and a distance.
  /// \param theSourceShape Shape to be moved
  /// \param theAxis Movement axis
  /// \param theDistance Movement distance
  /// \return a shape
  static std::shared_ptr<GeomAPI_Shape> makeTranslation(
                     std::shared_ptr<GeomAPI_Shape> theSourceShape,
                     std::shared_ptr<GeomAPI_Ax1> theAxis,
                     const double theDistance);

  /// Performs a translation from dimensions.
  /// \param theSourceShape Shape to be moved
  /// \param theDx Movement dimension on X
  /// \param theDy Movement dimension on Y
  /// \param theDz Movement dimension on Z
  /// \return a shape
  static std::shared_ptr<GeomAPI_Shape> makeTranslation(
                     std::shared_ptr<GeomAPI_Shape> theSourceShape,
                     const double theDx,
                     const double theDy,
                     const double theDz);

  /// Performs a translation from two points.
  /// \param theSourceShape Shape to be moved
  /// \param theStartPoint Movement start point
  /// \param theEndPoint Movement end point
  /// \return a shape
  static std::shared_ptr<GeomAPI_Shape> makeTranslation(
                     std::shared_ptr<GeomAPI_Shape> theSourceShape,
                     std::shared_ptr<GeomAPI_Pnt> theStartPoint,
                     std::shared_ptr<GeomAPI_Pnt> theEndPoint);

  /// Performs a rotation from an axis and an angle.
  /// \param theSourceShape Shape to be rotated
  /// \param theAxis Movement axis
  /// \param theAngle Movement angle
  /// \return a shape
  static std::shared_ptr<GeomAPI_Shape> makeRotation(
                     std::shared_ptr<GeomAPI_Shape> theSourceShape,
                     std::shared_ptr<GeomAPI_Ax1> theAxis,
                     const double theAngle);

  /// Performs a rotation from three points.
  /// \param theSourceShape Shape to be rotated
  /// \param theCenterPoint Movement center point
  /// \param theStartPoint Movement start point
  /// \param theEndPoint Movement end point
  /// \return a shape
  static std::shared_ptr<GeomAPI_Shape> makeRotation(
                     std::shared_ptr<GeomAPI_Shape> theSourceShape,
                     std::shared_ptr<GeomAPI_Pnt> theCenterPoint,
                     std::shared_ptr<GeomAPI_Pnt> theStartPoint,
                     std::shared_ptr<GeomAPI_Pnt> theEndPoint);

  /// Performs a symmetry by a point.
  /// \param theSourceShape Shape be symmetrized
  /// \param thePoint Point of symmetry
  static std::shared_ptr<GeomAPI_Shape> makeSymmetry(
                     std::shared_ptr<GeomAPI_Shape> theSourceShape,
                     std::shared_ptr<GeomAPI_Pnt> thePoint);

  /// Performs a symmetry by an axis.
  /// \param theSourceShape Shape be symmetrized
  /// \param theAxis Axis of symmetry
  static std::shared_ptr<GeomAPI_Shape> makeSymmetry(
                     std::shared_ptr<GeomAPI_Shape> theSourceShape,
                     std::shared_ptr<GeomAPI_Ax1> theAxis);

  /// Performs a symmetry by a plane.
  /// \param theSourceShape Shape be symmetrized
  /// \param thePlane Plane of symmetry
  static std::shared_ptr<GeomAPI_Shape> makeSymmetry(
                     std::shared_ptr<GeomAPI_Shape> theSourceShape,
                     std::shared_ptr<GeomAPI_Ax2> thePlane);

  /// Performs a scale by a scale factor.
  /// \param theSourceShape Shape be scaled
  /// \param theCenterPoint Point of scale
  /// \param theScaleFactor Factor of scale
  static std::shared_ptr<GeomAPI_Shape> makeScale(
                     std::shared_ptr<GeomAPI_Shape> theSourceShape,
                     std::shared_ptr<GeomAPI_Pnt> theCenterPoint,
                     const double theScaleFactor);

  /// Performs a scale by dimensions.
  /// \param theSourceShape Shape be scaled
  /// \param theCenterPoint Point of scale
  /// \param theScaleFactorX Factor of scale in X
  /// \param theScaleFactorY Factor of scale in Y
  /// \param theScaleFactorZ Factor of scale in Z
  static std::shared_ptr<GeomAPI_Shape> makeScale(
                     std::shared_ptr<GeomAPI_Shape> theSourceShape,
                     std::shared_ptr<GeomAPI_Pnt> theCenterPoint,
                     const double theScaleFactorX,
                     const double theScaleFactorY,
                     const double theScaleFactorZ);

  /// Performs a multi translation along one axis, at a distance and a number of times
  /// \param theSourceShape Shape to be moved
  /// \param theAxis Movement axis
  /// \param theStep Movement step
  /// \param theNumber Movement number
  /// \return a shape
  static std::shared_ptr<GeomAPI_Shape> makeMultiTranslation(
                     std::shared_ptr<GeomAPI_Shape> theSourceShape,
                     std::shared_ptr<GeomAPI_Ax1> theAxis,
                     const double theStep,
                     const int theNumber);

  /// Performs a multi translation along two axis : a different distance on each axis
  /// and a different number of times for each axis
  /// \param theSourceShape Shape to be moved
  /// \param theFirstAxis First movement axis
  /// \param theFirstStep First movement step
  /// \param theFirstNumber First movement number
  /// \param theSecondAxis First movement axis
  /// \param theSecondStep First movement step
  /// \param theSecondNumber First movement number
  /// \return a shape
  static std::shared_ptr<GeomAPI_Shape> makeMultiTranslation(
                     std::shared_ptr<GeomAPI_Shape> theSourceShape,
                     std::shared_ptr<GeomAPI_Ax1> theFirstAxis,
                     const double theFirstStep,
                     const int theFirstNumber,
                     std::shared_ptr<GeomAPI_Ax1> theSecondAxis,
                     const double theSecondStep,
                     const int theSecondNumber);

  /// Performs a multi rotation along one axis and a number of times
  /// \param[in] theSourceShape Shape to be rotated
  /// \param[in] theAxis Axis for the rotation
  /// \param[in] theNumber Number of copies
  static std::shared_ptr<GeomAPI_Shape> makeMultiRotation(
                     std::shared_ptr<GeomAPI_Shape> theSourceShape,
                     std::shared_ptr<GeomAPI_Ax1> theAxis,
                     const int theNumber);

  /// Performs a multi rotation along one axis, at a step and a number of times
  /// \param theSourceShape Shape to be moved
  /// \param[in] theAxis Axis for the rotation
  /// \param[in] theStep Angle for each rotation
  /// \param[in] theNumber Number of copies
  static std::shared_ptr<GeomAPI_Shape> makeMultiRotation(
                     std::shared_ptr<GeomAPI_Shape> theSourceShape,
                     std::shared_ptr<GeomAPI_Ax1> theAxis,
                     const double theStep,
                     const int theNumber);

  /// Creates a cone segment using standard GDML parameters.
  /// \param theRMin1 Inner radius at base of cone
  /// \param theRMax1 Outer radius at base of cone
  /// \param theRMin2 Inner radius at top of cone
  /// \param theRMax2 Outer radius at top of cone
  /// \param theZ Height of cone segment
  /// \param theStartPhi Start angle of the segment
  /// \param theDeltaPhi Angle of the segment
  static std::shared_ptr<GeomAPI_Shape> makeConeSegment(
                     const double theRMin1,
                     const double theRMax1,
                     const double theRMin2,
                     const double theRMax2,
                     const double theZ,
                     const double theStartPhi,
                     const double theDeltaPhi);
};
}
#endif
