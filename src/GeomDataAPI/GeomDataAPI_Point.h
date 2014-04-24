// File:        GeomData_AttributeDouble.h
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomData_AttributeDouble_HeaderFile
#define GeomData_AttributeDouble_HeaderFile

#include "GeomDataAPI.h"
#include <ModelAPI_Attribute.h>

/**\class GeomData_AttributeDouble
 * \ingroup DataModel
 * \brief Attribute that contains real value with double precision.
 */

class GeomDataAPI_Point : public ModelAPI_Attribute
{
public:
  /// Defines the double value
  virtual void setValue(const double theX, const double theY, const double theZ) = 0;

  /// Returns the X double value
  virtual double x() = 0;
  /// Returns the Y double value
  virtual double y() = 0;
  /// Returns the Z double value
  virtual double z() = 0;

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
