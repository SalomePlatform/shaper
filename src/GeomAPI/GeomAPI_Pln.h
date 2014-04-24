// File:        GeomAPI_Pln.hxx
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomAPI_Pln_HeaderFile
#define GeomAPI_Pln_HeaderFile

#include <memory>
#include <GeomAPI_Interface.h>

class GeomAPI_Pnt;
class GeomAPI_Dir;

/**\class GeomAPI_Pln
 * \ingroup DataModel
 * \brief 3D point defined by three coordinates
 */

class GEOMAPI_EXPORT GeomAPI_Pln: public GeomAPI_Interface
{
public:
  /// Creation of plane by the point and normal
  GeomAPI_Pln(const std::shared_ptr<GeomAPI_Pnt>& thePoint,
              const std::shared_ptr<GeomAPI_Dir>& theNormal);

  /// Creation of plane by coefficients A * X + B * Y + C * Z + D = 0.0 
  GeomAPI_Pln(const double theA, const double theB, const double theC, const double theD);

  /// Returns a point of this plane
  std::shared_ptr<GeomAPI_Pnt> location();

  /// Returns a plane normal
  std::shared_ptr<GeomAPI_Dir> direction();
};

#endif

