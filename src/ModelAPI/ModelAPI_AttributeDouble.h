// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeDouble.h
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_AttributeDouble_H_
#define ModelAPI_AttributeDouble_H_

#include "ModelAPI_Attribute.h"

#include <set>

class ModelAPI_Expression;

/**\class ModelAPI_AttributeDouble
 * \ingroup DataModel
 * \brief Attribute that contains real value with double precision.
 */

class ModelAPI_AttributeDouble : public ModelAPI_Attribute
{
 public:
  /// Defines the double value
  MODELAPI_EXPORT virtual void setValue(const double theValue) = 0;

  /// Returns the double value
  MODELAPI_EXPORT virtual double value() = 0;

  /// Defines the calculated double value
  MODELAPI_EXPORT virtual void setCalculatedValue(const double theValue) = 0;

  /// Defines the text value
  MODELAPI_EXPORT virtual void setText(const std::string& theText) = 0;

  /// Returns the text value
  MODELAPI_EXPORT virtual std::string text() = 0;

  /// Allows to set expression (text) as invalid (by the parameters listener)
  MODELAPI_EXPORT virtual void setExpressionInvalid(const bool theFlag) = 0;

  /// Returns true if text is invalid
  MODELAPI_EXPORT virtual bool expressionInvalid() = 0;

  /// Allows to set expression (text) error (by the parameters listener)
  MODELAPI_EXPORT virtual void setExpressionError(const std::string& theError) = 0;

  /// Returns an expression error
  MODELAPI_EXPORT virtual std::string expressionError() = 0;

  /// Defines the used parameters
  MODELAPI_EXPORT virtual void setUsedParameters(const std::set<std::string>& theUsedParameters) = 0;

  /// Returns the used parameters
  MODELAPI_EXPORT virtual std::set<std::string> usedParameters() const = 0;

  /// Returns the type of this class of attributes
  MODELAPI_EXPORT static std::string typeId()
  {
    return "Double";
  }

  /// Returns the type of this class of attributes, not static method
  MODELAPI_EXPORT virtual std::string attributeType();

  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_AttributeDouble();

 protected:
  /// Objects are created for features automatically
  MODELAPI_EXPORT ModelAPI_AttributeDouble();
};

//! Pointer on double attribute
typedef std::shared_ptr<ModelAPI_AttributeDouble> AttributeDoublePtr;

#endif
