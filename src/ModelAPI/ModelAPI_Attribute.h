// File:        ModelAPI_Attribute.h
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Attribute_HeaderFile
#define ModelAPI_Attribute_HeaderFile

#include "ModelAPI.h"
#include <string>

/**\class ModelAPI_Attribute
 * \ingroup DataModel
 * \brief Generic attribute of the Object.
 */

class MODELAPI_EXPORT ModelAPI_Attribute
{
public:
  
  /// Returns the type of this class of attributes, not static method
  virtual std::string attributeType() = 0;

protected:
  /// Objects are created for features automatically
  ModelAPI_Attribute()
  {}
};

#endif
