// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Pln.hxx
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomAPI_Pln_H_
#define GeomAPI_Pln_H_

#include <memory>
#include <GeomAPI_Interface.h>

class GeomAPI_Ax3;
class GeomAPI_Pnt;
class GeomAPI_Dir;

/**\class GeomAPI_Pln
 * \ingroup DataModel
 * \brief 3D point defined by three coordinates
 */

class GeomAPI_Pln : public GeomAPI_Interface
{
 public:
  /// Creation of plane by the axis placement
  GEOMAPI_EXPORT 
  GeomAPI_Pln(const std::shared_ptr<GeomAPI_Ax3>& theAxis);

  /// Creation of plane by the point and normal
  GEOMAPI_EXPORT 
  GeomAPI_Pln(const std::shared_ptr<GeomAPI_Pnt>& thePoint,
              const std::shared_ptr<GeomAPI_Dir>& theNormal);

  /// Creation of plane by coefficients (Ax+By+Cz+D=0)
  GEOMAPI_EXPORT 
  GeomAPI_Pln(const double theA, const double theB, const double theC, const double theD);

  /// Returns a point of this plane
  GEOMAPI_EXPORT 
  std::shared_ptr<GeomAPI_Pnt> location();

  /// Returns a plane normal
  GEOMAPI_EXPORT 
  std::shared_ptr<GeomAPI_Dir> direction();

  /// Returns the plane coefficients (Ax+By+Cz+D=0)
  GEOMAPI_EXPORT 
  void coefficients(double& theA, double& theB, double& theC, double& theD);
};

#endif

