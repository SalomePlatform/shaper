// File:        ModelAPI_AttributeReference.h
// Created:     8 May 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_AttributeReference_HeaderFile
#define ModelAPI_AttributeReference_HeaderFile

#include "ModelAPI_Attribute.h"
#include <ModelAPI_Feature.h>

/**\class ModelAPI_AttributeReference
 * \ingroup DataModel
 * \brief Attribute that contains reference to feature (located in the same document).
 */

class ModelAPI_AttributeReference : public ModelAPI_Attribute
{
public:
  /// Defines the feature referenced from this attribute
  MODELAPI_EXPORT virtual void setValue(FeaturePtr theFeature) = 0;

  /// Returns feature referenced from this attribute
  MODELAPI_EXPORT virtual FeaturePtr value() = 0;

  /// Returns the type of this class of attributes
  MODELAPI_EXPORT static std::string type() {return "Reference";}

  /// Returns the type of this class of attributes, not static method
  MODELAPI_EXPORT virtual std::string attributeType() {return type();}

  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_AttributeReference() {}

protected:
  /// Objects are created for features automatically
  MODELAPI_EXPORT ModelAPI_AttributeReference()
  {}
};

#endif
