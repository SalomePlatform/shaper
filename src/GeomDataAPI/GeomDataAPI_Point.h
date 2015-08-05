// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomDataAPI_Point.h
// Created:     24 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomDataAPI_Point_H_
#define GeomDataAPI_Point_H_

#include "GeomDataAPI.h"
#include <ModelAPI_Attribute.h>

#include <set>

class GeomAPI_Pnt;

/**\class GeomDataAPI_Point
 * \ingroup DataModel
 * \brief Attribute that contains 3D point coordinates. 
 */

class GeomDataAPI_Point : public ModelAPI_Attribute
{
 public:
  /// Defines the double value
  GEOMDATAAPI_EXPORT virtual void setValue(const double theX, const double theY, const double theZ) = 0;
  /// Defines the point
  GEOMDATAAPI_EXPORT virtual void setValue(const std::shared_ptr<GeomAPI_Pnt>& thePoint) = 0;

  /// Returns the X double value
  GEOMDATAAPI_EXPORT virtual double x() const = 0;
  /// Returns the Y double value
  GEOMDATAAPI_EXPORT virtual double y() const = 0;
  /// Returns the Z double value
  GEOMDATAAPI_EXPORT virtual double z() const = 0;
  /// Returns the 3D point
  GEOMDATAAPI_EXPORT virtual std::shared_ptr<GeomAPI_Pnt> pnt() = 0;

  /// Defines the calculated double value
  GEOMDATAAPI_EXPORT virtual void setCalculatedValue(const double theX, const double theY, const double theZ) = 0;
  
  /// Defines the text values
  GEOMDATAAPI_EXPORT virtual void setText(const std::string& theX,
                                          const std::string& theY,
                                          const std::string& theZ) = 0;

  /// Returns the text values
  GEOMDATAAPI_EXPORT virtual std::string textX() = 0;
  GEOMDATAAPI_EXPORT virtual std::string textY() = 0;
  GEOMDATAAPI_EXPORT virtual std::string textZ() = 0;

  enum PointComponent { C_X = 0,
                        C_Y = 1,
                        C_Z = 2,
  };

  /// Allows to set expression (text) as invalid (by the parameters listener)
  GEOMDATAAPI_EXPORT virtual void setExpressionInvalid(int theComponent, const bool theFlag) = 0;

  /// Returns true if text is invalid
  GEOMDATAAPI_EXPORT virtual bool expressionInvalid(int theComponent) = 0;

  /// Allows to set expression (text) error (by the parameters listener)
  GEOMDATAAPI_EXPORT virtual void setExpressionError(int theComponent, const std::string& theError) = 0;

  /// Returns an expression error
  GEOMDATAAPI_EXPORT virtual std::string expressionError(int theComponent) = 0;

  /// Defines the used parameters
  GEOMDATAAPI_EXPORT virtual void setUsedParameters(int theComponent, 
    const std::set<std::string>& theUsedParameters) = 0;

  /// Returns the used parameters
  GEOMDATAAPI_EXPORT virtual std::set<std::string> usedParameters(int theComponent) const = 0;

  /// Returns the type of this class of attributes
  static std::string typeId()
  {
    return std::string("Point");
  }

  /// Returns the type of this class of attributes, not static method
  GEOMDATAAPI_EXPORT virtual std::string attributeType();

 protected:
  /// Objects are created for features automatically
  GEOMDATAAPI_EXPORT GeomDataAPI_Point();
  GEOMDATAAPI_EXPORT virtual ~GeomDataAPI_Point();
};

//! Pointer on attribute object
typedef std::shared_ptr<GeomDataAPI_Point> AttributePointPtr;

#endif
