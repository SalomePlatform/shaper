// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeBoolean.h
// Created:     2 june 2014
// Author:      Vitaly Smetannikov

#ifndef MODELAPI_ATTRIBUTEBOOLEAN_H_
#define MODELAPI_ATTRIBUTEBOOLEAN_H_

#include "ModelAPI_Attribute.h"

/**\class ModelAPI_AttributeBoolean
 * \ingroup DataModel
 * \brief Attribute that contains boolean value.
 */

class ModelAPI_AttributeBoolean : public ModelAPI_Attribute
{
 public:
  /// Defines the double value
  MODELAPI_EXPORT virtual void setValue(bool theValue) = 0;

  /// Returns the double value
  MODELAPI_EXPORT virtual bool value() = 0;

  /// Returns the type of this class of attributes
  MODELAPI_EXPORT static std::string type()
  {
    return "Boolean";
  }

  /// Returns the type of this class of attributes, not static method
  MODELAPI_EXPORT virtual std::string attributeType()
  {
    return type();
  }

  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_AttributeBoolean()
  {
  }

 protected:
  /// Objects are created for features automatically
  MODELAPI_EXPORT ModelAPI_AttributeBoolean()
  {
  }
};

typedef std::shared_ptr<ModelAPI_AttributeBoolean> AttributeBooleanPtr;

#endif
