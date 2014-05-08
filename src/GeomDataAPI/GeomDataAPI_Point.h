// File:        GeomDataAPI_Point.h
// Created:     24 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomDataAPI_Point_HeaderFile
#define GeomDataAPI_Point_HeaderFile

#include "GeomDataAPI.h"
#include <ModelAPI_Attribute.h>

class GeomAPI_Pnt;

/**\class GeomDataAPI_Point
 * \ingroup DataModel
 * \brief Attribute that contains 3D point coordinates. 
 */

class GeomDataAPI_Point : public ModelAPI_Attribute
{
public:
  /// Defines the double value
  virtual void setValue(const double theX, const double theY, const double theZ) = 0;

  /// Returns the X double value
  virtual double x() const = 0;
  /// Returns the Y double value
  virtual double y() const = 0;
  /// Returns the Z double value
  virtual double z() const = 0;
  /// Returns the 3D point
  virtual boost::shared_ptr<GeomAPI_Pnt> pnt() = 0;

  /// Returns the type of this class of attributes
  static inline std::string type() {return std::string("Point");}

  /// Returns the type of this class of attributes, not static method
  virtual std::string attributeType() {return type();}

protected:
  /// Objects are created for features automatically
  GeomDataAPI_Point()
  {}
};

#endif
