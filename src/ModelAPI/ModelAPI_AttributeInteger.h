// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeInteger.h
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef MODELAPI_ATTRIBUTEINTEGER_H_
#define MODELAPI_ATTRIBUTEINTEGER_H_

#include <ModelAPI.h>
#include <ModelAPI_Attribute.h>

#include <string>


/**\class ModelAPI_AttributeInteger
 * \ingroup DataModel
 * \brief API for the attribute that contains integer (int).
 */

class ModelAPI_AttributeInteger : public ModelAPI_Attribute
{
 public:
  /// Defines the integer value
  MODELAPI_EXPORT virtual void setValue(const int theValue) = 0;

  /// Returns the inhteger value
  MODELAPI_EXPORT virtual int value() = 0;

  /// Returns the type of this class of attributes
  MODELAPI_EXPORT static std::string type()
  {
    return "Integer";
  }

  /// Returns the type of this class of attributes, not static method
  MODELAPI_EXPORT virtual std::string attributeType();

  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_AttributeInteger();

 protected:
  /// Objects are created for features automatically
  MODELAPI_EXPORT ModelAPI_AttributeInteger();
};

//! Pointer on double attribute
typedef std::shared_ptr<ModelAPI_AttributeInteger> AttributeIntegerPtr;

#endif
