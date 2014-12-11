// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeString.h
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef MODELAPI_ATTRIBUTESTRING_H_
#define MODELAPI_ATTRIBUTESTRING_H_

#include "ModelAPI_Attribute.h"

#include <string>

/**\class ModelAPI_AttributeString
 * \ingroup DataModel
 * \brief API for the attribute that contains std (null terminated) string.
 */

class ModelAPI_AttributeString : public ModelAPI_Attribute
{
 public:
  /// Defines the double value
  MODELAPI_EXPORT virtual void setValue(const std::string& theValue) = 0;

  /// Returns the double value
  MODELAPI_EXPORT virtual std::string value() = 0;

  /// Returns the type of this class of attributes
  MODELAPI_EXPORT static std::string type()
  {
    return "String";
  }

  /// Returns the type of this class of attributes, not static method
  MODELAPI_EXPORT virtual std::string attributeType()
  {
    return type();
  }

  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_AttributeString()
  {
  }

 protected:
  /// Objects are created for features automatically
  MODELAPI_EXPORT ModelAPI_AttributeString()
  {
  }
};

//! Pointer on double attribute
typedef std::shared_ptr<ModelAPI_AttributeString> AttributeStringPtr;

#endif
