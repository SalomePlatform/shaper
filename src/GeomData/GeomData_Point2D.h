// File:        GeomData_Point2D.h
// Created:     24 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomData_Point2D_HeaderFile
#define GeomData_Point2D_HeaderFile

#include "GeomData.h"
#include "GeomDataAPI_Point2D.h"
#include <TDataStd_RealArray.hxx>
#include <TDF_Label.hxx>

/**\class GeomData_Point2D
 * \ingroup DataModel
 * \brief Attribute that contains 2D point.
 */

class GeomData_Point2D : public GeomDataAPI_Point2D
{
  Handle_TDataStd_RealArray myCoords; ///< X and Y doubles as real array attribute [0; 1]
public:
  /// Defines the double value
  GEOMDATA_EXPORT virtual void setValue(const double theX, const double theY);

  /// Returns the X double value
  GEOMDATA_EXPORT virtual double x() const;
  /// Returns the Y double value
  GEOMDATA_EXPORT virtual double y() const;

protected:
  /// Initializes attributes
  GEOMDATA_EXPORT GeomData_Point2D(TDF_Label& theLabel);

  friend class Model_Data;
};

#endif
