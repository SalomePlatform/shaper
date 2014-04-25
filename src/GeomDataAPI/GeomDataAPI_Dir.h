// File:        GeomDataAPI_Dir.h
// Created:     24 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomDataAPI_Dir_HeaderFile
#define GeomDataAPI_Dir_HeaderFile

#include "GeomDataAPI.h"
#include <ModelAPI_Attribute.h>

/**\class GeomDataAPI_Dir
 * \ingroup DataModel
 * \brief Attribute that contains 3D direction coordinates. 
 */

class GeomDataAPI_Dir : public ModelAPI_Attribute
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

  /// Returns the type of this class of attributes
  static inline std::string type() {return std::string("Point");}

  /// Returns the type of this class of attributes, not static method
  virtual std::string attributeType() {return type();}

protected:
  /// Objects are created for features automatically
  GeomDataAPI_Dir()
  {}
};

#endif
