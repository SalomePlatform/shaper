// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_ShapeAPI.h
// Created:     17 Mar 2016
// Author:      Clarisse Genrault (CEA)

#ifndef GEOMALGOAPI_SHAPEAPI_H
#define GEOMALGOAPI_SHAPEAPI_H

#include <GeomAPI_Ax1.h>
#include <GeomAPI_Ax2.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Shape.h>

#include <GeomAlgoAPI_Exception.h>

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
                                                const double theDz) throw (GeomAlgoAPI_Exception);

  /// Creates a box using the two points that defined a diagonal.
  /// \param theFirstPoint One extermity of the diagonal
  /// \param theSecondPoint The other extremity of the diagonal
  /// \return a shape
  static std::shared_ptr<GeomAPI_Shape> makeBox(std::shared_ptr<GeomAPI_Pnt> theFirstPoint,
                     std::shared_ptr<GeomAPI_Pnt> theSecondPoint) throw (GeomAlgoAPI_Exception);

  /// Creates a cylinder using a center, an axis, a radius and a height.
  /// \param theBasePoint The center of the lower base of the cylinder
  /// \param theEdge The axis of the cylinder
  /// \param theRadius The radius of the cylinder
  /// \param theHeight The heigth of the cylinder
  static std::shared_ptr<GeomAPI_Shape> makeCylinder(std::shared_ptr<GeomAPI_Pnt> theBasePoint,
                     std::shared_ptr<GeomAPI_Edge> theEdge, double theRadius, double theHeight)
                     throw (GeomAlgoAPI_Exception);

  /// Creates a portion of cylinder using a center, an axis, a radius, a height and an angle.
  /// \param theBasePoint The center of the lower base of the cylinder
  /// \param theEdge The axis of the cylinder
  /// \param theRadius The radius of the cylinder
  /// \param theHeight The heigth of the cylinder
  /// \param theAngle The angle defining the portion
  static std::shared_ptr<GeomAPI_Shape> makeCylinder(std::shared_ptr<GeomAPI_Pnt> theBasePoint,
                     std::shared_ptr<GeomAPI_Edge> theEdge, double theRadius, double theHeight,
                     double theAngle) throw (GeomAlgoAPI_Exception);

  /// Creates a cylinder using the origin, the OZ axis, a radius and a height.
  /// \param theRadius The radius of the cylinder
  /// \param theHeight The heigth of the cylinder
  static std::shared_ptr<GeomAPI_Shape> makeCylinder(double theRadius, double theHeight)
                     throw (GeomAlgoAPI_Exception);

  /// Creates a portion of cylinder using the origin, the OZ axis, a radius, a height and an angle.
  /// \param theRadius The radius of the cylinder
  /// \param theHeight The heigth of the cylinder
  /// \param theAngle The angle defining the portion
  static std::shared_ptr<GeomAPI_Shape> makeCylinder(double theRadius, double theHeight,
                     double theAngle) throw (GeomAlgoAPI_Exception);

  /// Performs a translation from an axis and a distance.
  /// \param theSourceShape Shape to be moved
  /// \param theAxis Movement axis
  /// \param theDistance Movement distance
  /// \return a shape
  static std::shared_ptr<GeomAPI_Shape> makeTranslation(
                     std::shared_ptr<GeomAPI_Shape> theSourceShape,
                     std::shared_ptr<GeomAPI_Ax1>   theAxis,
                     const double theDistance) throw (GeomAlgoAPI_Exception);

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
                     const double theDz) throw (GeomAlgoAPI_Exception);

  /// Performs a translation from two points.
  /// \param theSourceShape Shape to be moved
  /// \param theStartPoint Movement start point
  /// \param theEndPoint Movement end point
  /// \return a shape
  static std::shared_ptr<GeomAPI_Shape> makeTranslation(
                     std::shared_ptr<GeomAPI_Shape> theSourceShape,
                     std::shared_ptr<GeomAPI_Pnt>   theStartPoint,
                     std::shared_ptr<GeomAPI_Pnt>   theEndPoint) throw (GeomAlgoAPI_Exception);

  /// Performs a symmetry by a point.
  /// \param theSourceShape Shape be symmetrized
  /// \param thePoint Point of symmetry
  static std::shared_ptr<GeomAPI_Shape> makeSymmetry(
                     std::shared_ptr<GeomAPI_Shape> theSourceShape,
                     std::shared_ptr<GeomAPI_Pnt>   thePoint) throw (GeomAlgoAPI_Exception);

  /// Performs a symmetry by an axis.
  /// \param theSourceShape Shape be symmetrized
  /// \param theAxis Axis of symmetry
  static std::shared_ptr<GeomAPI_Shape> makeSymmetry(
                     std::shared_ptr<GeomAPI_Shape> theSourceShape,
                     std::shared_ptr<GeomAPI_Ax1>   theAxis) throw (GeomAlgoAPI_Exception);

  /// Performs a symmetry by a plane.
  /// \param theSourceShape Shape be symmetrized
  /// \param thePlane Plane of symmetry
  static std::shared_ptr<GeomAPI_Shape> makeSymmetry(
                     std::shared_ptr<GeomAPI_Shape> theSourceShape,
                     std::shared_ptr<GeomAPI_Ax2>   thePlane) throw (GeomAlgoAPI_Exception);

  /// Performs a scale by a scale factor.
  /// \param theSourceShape Shape be scaled
  /// \param theCenterPoint Point of scale
  /// \param theScaleFactor Factor of scale
  static std::shared_ptr<GeomAPI_Shape> makeScale(
                     std::shared_ptr<GeomAPI_Shape> theSourceShape,
                     std::shared_ptr<GeomAPI_Pnt>   theCenterPoint,
                     const double                   theScaleFactor) throw (GeomAlgoAPI_Exception);

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
                     const double theDeltaPhi) throw (GeomAlgoAPI_Exception);
};
}
#endif
