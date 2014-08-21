// File:        GeomDataAPI_Point2D.h
// Created:     24 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomDataAPI_Point2D_H_
#define GeomDataAPI_Point2D_H_

#include "GeomDataAPI.h"
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
  virtual void setValue(const double theX, const double theY) = 0;
  /// Defines the point
  virtual void setValue(const boost::shared_ptr<GeomAPI_Pnt2d>& thePoint) = 0;

  /// Returns the X double value
  virtual double x() const = 0;
  /// Returns the Y double value
  virtual double y() const = 0;
  /// Returns the 2D point
  virtual boost::shared_ptr<GeomAPI_Pnt2d> pnt() = 0;

  /// Returns the type of this class of attributes
  static inline std::string type()
  {
    return std::string("Point2D");
  }

  /// Returns the type of this class of attributes, not static method
  virtual std::string attributeType()
  {
    return type();
  }

 protected:
  /// Objects are created for features automatically
  GeomDataAPI_Point2D()
  {
  }
};

#endif
