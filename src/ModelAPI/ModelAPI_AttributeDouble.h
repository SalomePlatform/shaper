// File:        ModelAPI_AttributeDouble.h
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_AttributeDouble_HeaderFile
#define ModelAPI_AttributeDouble_HeaderFile

#include "ModelAPI_Attribute.h"

/**\class ModelAPI_AttributeDouble
 * \ingroup DataModel
 * \brief Attribute that contains real value with double precision.
 */

class MODELAPI_EXPORT ModelAPI_AttributeDouble : public ModelAPI_Attribute
{
public:
  /// Defines the double value
  virtual void setValue(const double theValue) = 0;

  /// Returns the double value
  virtual double value() = 0;

  /// Returns the type of this class of attributes
  static std::string type() {return "Double";}

  /// Returns the type of this class of attributes, not static method
  virtual std::string attributeType() {return type();}

protected:
  /// Objects are created for features automatically
  ModelAPI_AttributeDouble()
  {}
};

#endif
