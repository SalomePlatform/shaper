// File:        GeomData_Point.h
// Created:     24 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomData_Point_HeaderFile
#define GeomData_Point_HeaderFile

#include "GeomData.h"
#include "GeomDataAPI_Point.h"
#include <TDataStd_RealArray.hxx>
#include <TDF_Label.hxx>

/**\class GeomData_Point
 * \ingroup DataModel
 * \brief Attribute that contains 3D point.
 */

class GeomData_Point : public GeomDataAPI_Point
{
  Handle_TDataStd_RealArray myCoords; ///< X, Y and Z doubles as real array attribute [0; 2]
public:
  /// Defines the double value
  GEOMDATA_EXPORT virtual void setValue(const double theX, const double theY, const double theZ);

  /// Returns the X double value
  GEOMDATA_EXPORT virtual double x() const;
  /// Returns the Y double value
  GEOMDATA_EXPORT virtual double y() const;
  /// Returns the Z double value
  GEOMDATA_EXPORT virtual double z() const;

protected:
  /// Initializes attributes
  GEOMDATA_EXPORT GeomData_Point(TDF_Label& theLabel);

  friend class Model_Data;
};

#endif
