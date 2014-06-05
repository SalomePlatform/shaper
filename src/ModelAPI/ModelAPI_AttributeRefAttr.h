// File:        ModelAPI_AttributeRefAttr.h
// Created:     8 May 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_AttributeRefAttr_HeaderFile
#define ModelAPI_AttributeRefAttr_HeaderFile

#include "ModelAPI_Attribute.h"
#include "ModelAPI_Feature.h"

/**\class ModelAPI_AttributeRefAttr
 * \ingroup DataModel
 * \brief Attribute that contains reference to an attribute of a feature or reference to 
 * a feature (switchable)
 */

class ModelAPI_AttributeRefAttr : public ModelAPI_Attribute
{
public:
  /// Returns true if this attribute references to a feature (not to the attribute)
  MODELAPI_EXPORT virtual bool isFeature() = 0;

  /// Defines the reference to the attribute
  MODELAPI_EXPORT virtual void setAttr(boost::shared_ptr<ModelAPI_Attribute> theAttr) = 0;

  /// Returns attribute referenced from this attribute
  MODELAPI_EXPORT virtual boost::shared_ptr<ModelAPI_Attribute> attr() = 0;

  /// Defines the reference to the feature
  MODELAPI_EXPORT virtual void setFeature(FeaturePtr theFeature) = 0;

  /// Returns feature referenced from this attribute
  MODELAPI_EXPORT virtual FeaturePtr feature() = 0;

  /// Returns the type of this class of attributes
  MODELAPI_EXPORT static std::string type() {return "RefAttr";}

  /// Returns the type of this class of attributes, not static method
  MODELAPI_EXPORT virtual std::string attributeType() {return type();}

  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_AttributeRefAttr() {}

protected:
  /// Objects are created for features automatically
  MODELAPI_EXPORT ModelAPI_AttributeRefAttr()
  {}
};

#endif
