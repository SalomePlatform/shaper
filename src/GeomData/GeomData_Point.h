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
 * \brief Attribute that contains real value with double precision.
 */

class GeomData_Point : public ModelAPI_Attribute
{
  Handle_TDataStd_RealArray myCoords; ///< X, Y and Z doubles as real array attribute [0; 2]
public:
  /// Defines the double value
  virtual void setValue(const double theX, const double theY, const double theZ);

  /// Returns the X double value
  virtual double x() const;
  /// Returns the Y double value
  virtual double y() const;
  /// Returns the Z double value
  virtual double z() const;

protected:
  /// Initializes attributes
  GeomData_Point(TDF_Label& theLabel);
};

#endif
