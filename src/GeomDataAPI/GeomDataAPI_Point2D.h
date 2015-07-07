// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomDataAPI_Point2D.h
// Created:     24 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomDataAPI_Point2D_H_
#define GeomDataAPI_Point2D_H_

#include <GeomDataAPI.h>
#include <ModelAPI_Attribute.h>

class GeomAPI_Pnt2d;

/**\class GeomDataAPI_Point2D
 * \ingroup DataModel
 * \brief Attribute that contains 2D point coordinates.
 */

class GeomDataAPI_Point2D : public ModelAPI_Attribute
{
 public:
  /// Defines the double value
  GEOMDATAAPI_EXPORT virtual void setValue(const double theX, const double theY) = 0;
  /// Defines the point
  GEOMDATAAPI_EXPORT virtual void setValue(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint) = 0;

  /// Returns the X double value
  GEOMDATAAPI_EXPORT virtual double x() const = 0;
  /// Returns the Y double value
  GEOMDATAAPI_EXPORT virtual double y() const = 0;
  /// Returns the 2D point
  GEOMDATAAPI_EXPORT virtual std::shared_ptr<GeomAPI_Pnt2d> pnt() = 0;

  /// Defines the calculated double value
  GEOMDATAAPI_EXPORT virtual void setCalculatedValue(const double theX, const double theY) = 0;

  /// Defines the text values
  GEOMDATAAPI_EXPORT virtual void setText(const std::string& theX,
                                          const std::string& theY) = 0;

  /// Returns the text values
  GEOMDATAAPI_EXPORT virtual std::string textX() = 0;
  GEOMDATAAPI_EXPORT virtual std::string textY() = 0;

  enum PointComponent { C_X = 0,
                        C_Y = 1,
  };

  /// Allows to set expression (text) as invalid (by the parameters listener)
  GEOMDATAAPI_EXPORT virtual void setExpressionInvalid(int theComponent, const bool theFlag) = 0;

  /// Returns true if text is invalid
  GEOMDATAAPI_EXPORT virtual bool expressionInvalid(int theComponent) = 0;

  /// Appends the delta values to point
  GEOMDATAAPI_EXPORT void move(const double theDeltaX, const double theDeltaY);

  /// Returns the type of this class of attributes
  static std::string typeId()
  {
    return std::string("Point2D");
  }

  /// Returns the type of this class of attributes, not static method
  GEOMDATAAPI_EXPORT virtual std::string attributeType();

 protected:
  /// Objects are created for features automatically
  GEOMDATAAPI_EXPORT GeomDataAPI_Point2D();
  GEOMDATAAPI_EXPORT virtual ~GeomDataAPI_Point2D();
};

//! Pointer on attribute object
typedef std::shared_ptr<GeomDataAPI_Point2D> AttributePoint2DPtr;

#endif
