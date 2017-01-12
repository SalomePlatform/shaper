// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Cylinder.h
// Created:     05 Jan 2016
// Author:      Clarisse Genrault (CEA)

#ifndef GEOMALGOAPI_CYLINDER_H_
#define GEOMALGOAPI_CYLINDER_H_

#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Ax2.h>
#include <GeomAPI_Pnt.h>

/**\class GeomAlgoAPI_Cylinder
 * \ingroup DataAlgo
 * \brief Allows to create Cylinder Primitives
 */
class GeomAlgoAPI_Cylinder : public GeomAlgoAPI_MakeShape
{
 public:
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Cylinder();

  /// Creates a cylinder
  /// \param theAxis The axis of the cylinder
  /// \param theRadius The radius of the cylinder
  /// \param theHeight The height of the cylinder
  /// \param theAngle The covering angle of the cylinder
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Cylinder(std::shared_ptr<GeomAPI_Ax2> theAxis,
                                          const double theRadius,
                                          const double theHeight);

  /// Creates a cylinder
  /// \param theAxis The axis of the cylinder
  /// \param theRadius The radius of the cylinder
  /// \param theHeight The height of the cylinder
  /// \param theAngle The covering angle of the cylinder
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Cylinder(std::shared_ptr<GeomAPI_Ax2> theAxis,
                                          const double theRadius,
                                          const double theHeight,
                                          const double theAngle);

  /// Checks if data for the cyminder construction is OK.
  GEOMALGOAPI_EXPORT bool check();

  /// Builds the cylinder.
  GEOMALGOAPI_EXPORT void build();

 private:
  bool withAngle;
  std::shared_ptr<GeomAPI_Pnt> myBasePoint;
  std::shared_ptr<GeomAPI_Ax2> myAxis; /// Axis of the cylinder.
  double myRadius; /// Radius of the cylinder.
  double myHeight; /// Height of the cylinder.
  double myAngle; /// Covering polar angle of the cylinder;
};


#endif // GEOMALGOAPI_CYLINDER_H_
