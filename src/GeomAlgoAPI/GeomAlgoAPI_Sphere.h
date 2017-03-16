// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Sphere.h
// Created:     16 Mar 2017
// Author:      Clarisse Genrault (CEA)

#ifndef GEOMALGOAPI_SPHERE_H_
#define GEOMALGOAPI_SPHERE_H_

#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Pnt.h>

/**\class GeomAlgoAPI_Sphere
 * \ingroup DataAlgo
 * \brief Allows to create Sphere Primitives
 */
class GeomAlgoAPI_Sphere : public GeomAlgoAPI_MakeShape
{
 public:
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Sphere();

  /// Creates a sphere.
  /// \param theCenterPoint The center point of the sphere
  /// \param theRadius The radius of the sphere
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Sphere(std::shared_ptr<GeomAPI_Pnt> theCenterPoint,
                                        const double theRadius);

  /// Checks if data for the sphere construction is OK.
  GEOMALGOAPI_EXPORT bool check();

  /// Builds the sphere.
  GEOMALGOAPI_EXPORT void build();

 private:
  std::shared_ptr<GeomAPI_Pnt> myCenterPoint; /// Center of the sphere.
  double myRadius;
};

#endif // GEOMALGOAPI_SPHERE_H_
