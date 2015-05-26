// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomData_Point.h
// Created:     24 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomData_Point_H_
#define GeomData_Point_H_

#include "GeomData.h"
#include "GeomDataAPI_Point.h"
#include <TDataStd_RealArray.hxx>
#include <TDataStd_ExtStringArray.hxx>
#include <TDataStd_BooleanArray.hxx>
#include <TDF_Label.hxx>

/**\class GeomData_Point
 * \ingroup DataModel
 * \brief Attribute that contains 3D point.
 */

class GeomData_Point : public GeomDataAPI_Point
{
  Handle_TDataStd_RealArray myCoords;  ///< X, Y and Z doubles as real array attribute [0; 2]
  Handle_TDataStd_ExtStringArray myTextArray;  ///< Text representation of the X, Y and Z attributes [0; 2]
  Handle_TDataStd_BooleanArray myExpressionInvalidArray;  ///< Flag of invalid expression of the X, Y and Z attributes [0; 2]
 public:
  /// Defines the double value
  GEOMDATA_EXPORT virtual void setValue(const double theX, const double theY, const double theZ);
  /// Defines the point
  GEOMDATA_EXPORT virtual void setValue(const std::shared_ptr<GeomAPI_Pnt>& thePoint);

  /// Returns the X double value
  GEOMDATA_EXPORT virtual double x() const;
  /// Returns the Y double value
  GEOMDATA_EXPORT virtual double y() const;
  /// Returns the Z double value
  GEOMDATA_EXPORT virtual double z() const;
  /// Returns the 3D point
  GEOMDATA_EXPORT virtual std::shared_ptr<GeomAPI_Pnt> pnt();

  /// Defines the text values
  GEOMDATA_EXPORT virtual void setText(const std::string& theX,
                                       const std::string& theY,
                                       const std::string& theZ);

  /// Returns the X text value
  GEOMDATA_EXPORT virtual std::string textX();
  /// Returns the Y text value
  GEOMDATA_EXPORT virtual std::string textY();
  /// Returns the Z text value
  GEOMDATA_EXPORT virtual std::string textZ();

  /// Allows to set expression (text) as invalid (by the parameters listener)
  GEOMDATA_EXPORT virtual void setExpressionInvalid(int, bool theFlag);

  /// Returns true if text is invalid
  GEOMDATA_EXPORT virtual bool expressionInvalid(int);

 protected:
  /// Initializes attributes
  GEOMDATA_EXPORT GeomData_Point(TDF_Label& theLabel);

  friend class Model_Data;
};

#endif
