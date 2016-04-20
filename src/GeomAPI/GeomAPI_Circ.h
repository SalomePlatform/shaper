// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Circ.h
// Created:     24 Jun 2014
// Author:      Artem ZHIDKOV

#ifndef GeomAPI_Circ_H_
#define GeomAPI_Circ_H_

#include <GeomAPI_Interface.h>
#include <memory>

class GeomAPI_Ax2;
class GeomAPI_Pnt;
class GeomAPI_Dir;

/**\class GeomAPI_Circ
 * \ingroup DataModel
 * \brief Circle in 3D
 */

class GeomAPI_Circ : public GeomAPI_Interface
{
 public:

  /** \brief Constructs a circle of radius Radius, where theAx2 locates the circle and defines its orientation in 3D space such that:\n
   *  - the center of the circle is the origin of theAx2;\n
   *  - the origin, "X Direction" and "Y Direction" of theAx2 define the plane of the circle;\n
   *  - theAx2 is the local coordinate system of the circle.\n
   *    Note: It is possible to create a circle where Radius is equal to 0.0. raised if Radius < 0.
   */
  GEOMAPI_EXPORT GeomAPI_Circ(const std::shared_ptr<GeomAPI_Ax2> theAx2,
                              const double theRadius);

  /// Creation of circle defined by center point, direction and circle radius
  GEOMAPI_EXPORT GeomAPI_Circ(const std::shared_ptr<GeomAPI_Pnt>& theCenter,
               const std::shared_ptr<GeomAPI_Dir>& theDir, double theRadius);

  /// Return center of the circle
  GEOMAPI_EXPORT const std::shared_ptr<GeomAPI_Pnt> center() const;

  /// Return radius of the circle
  GEOMAPI_EXPORT double radius() const;

  /// Return orthogonal direction to the circle's plane
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Dir> normal() const;

  /// Project point on circle
  GEOMAPI_EXPORT const std::shared_ptr<GeomAPI_Pnt> project(
      const std::shared_ptr<GeomAPI_Pnt>& thePoint) const;

  /** \brief Computes the parameter of a given point on a circle. The point must be
   *         located either on the circle itself or relatively to the latter
   *         at a distance less than the tolerance value. Return FALSE if the point
   *         is beyond the tolerance limit or if computation fails.
   *         Max Tolerance value is currently limited to 1.e-4
   *  \param[in] thePoint point of origin.
   *  \param[in] theTolerance tolerance of computation.
   *  \param[out] theParameter resulting parameter.
   */
  GEOMAPI_EXPORT const bool parameter(const std::shared_ptr<GeomAPI_Pnt> thePoint,
                                      const double theTolerance,
                                      double& theParameter) const;
};

#endif

