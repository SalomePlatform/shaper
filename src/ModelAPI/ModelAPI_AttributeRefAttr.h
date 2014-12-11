// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeRefAttr.h
// Created:     8 May 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_AttributeRefAttr_H_
#define ModelAPI_AttributeRefAttr_H_

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
  /// Returns true if this attribute references to a object (not to the attribute)
  MODELAPI_EXPORT virtual bool isObject() = 0;

  /// Defines the reference to the attribute
  MODELAPI_EXPORT virtual void setAttr(std::shared_ptr<ModelAPI_Attribute> theAttr) = 0;

  /// Returns attribute referenced from this attribute
  MODELAPI_EXPORT virtual std::shared_ptr<ModelAPI_Attribute> attr() = 0;

  /// Defines the reference to the object
  MODELAPI_EXPORT virtual void setObject(ObjectPtr theFeature) = 0;

  /// Returns object referenced from this attribute
  MODELAPI_EXPORT virtual ObjectPtr object() = 0;

  /// Returns the type of this class of attributes
  MODELAPI_EXPORT static std::string type()
  {
    return "RefAttr";
  }

  /// Returns the type of this class of attributes, not static method
  MODELAPI_EXPORT virtual std::string attributeType()
  {
    return type();
  }

  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_AttributeRefAttr()
  {
  }

 protected:
  /// Objects are created for features automatically
  MODELAPI_EXPORT ModelAPI_AttributeRefAttr()
  {
  }
};

typedef std::shared_ptr<ModelAPI_AttributeRefAttr> AttributeRefAttrPtr;

#endif
