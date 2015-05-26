// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomData_Point2D.h
// Created:     24 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomData_Point2D_H_
#define GeomData_Point2D_H_

#include "GeomData.h"
#include "GeomDataAPI_Point2D.h"
#include <TDataStd_RealArray.hxx>
#include <TDataStd_ExtStringArray.hxx>
#include <TDataStd_BooleanArray.hxx>
#include <TDF_Label.hxx>

/**\class GeomData_Point2D
 * \ingroup DataModel
 * \brief Attribute that contains 2D point.
 */

class GeomData_Point2D : public GeomDataAPI_Point2D
{
  Handle_TDataStd_RealArray myCoords;  ///< X and Y doubles as real array attribute [0; 1]
  Handle_TDataStd_ExtStringArray myTextArray;  ///< Text representation of the X, Y and Z attributes [0; 2]
  Handle_TDataStd_BooleanArray myExpressionInvalidArray;  ///< Flag of invalid expression of the X, Y and Z attributes [0; 2]
 public:
  /// Defines the double value
  GEOMDATA_EXPORT virtual void setValue(const double theX, const double theY);
  /// Defines the point
  GEOMDATA_EXPORT virtual void setValue(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint);

  /// Returns the X double value
  GEOMDATA_EXPORT virtual double x() const;
  /// Returns the Y double value
  GEOMDATA_EXPORT virtual double y() const;
  /// Returns the 2D point
  GEOMDATA_EXPORT virtual std::shared_ptr<GeomAPI_Pnt2d> pnt();

  /// Defines the double values
  GEOMDATA_EXPORT virtual void setText(const std::string& theX,
                                       const std::string& theY);

  /// Returns the double values
  GEOMDATA_EXPORT virtual std::string textX();
  GEOMDATA_EXPORT virtual std::string textY();

  /// Allows to set expression (text) as invalid (by the parameters listener)
  GEOMDATA_EXPORT virtual void setExpressionInvalid(int, const bool theFlag);

  /// Returns true if text is invalid
  GEOMDATA_EXPORT virtual bool expressionInvalid(int);

 protected:
  /// Initializes attributes
  GEOMDATA_EXPORT GeomData_Point2D(TDF_Label& theLabel);

  friend class Model_Data;
};

#endif
