// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeInteger.h
// Created:     03 sep 2014
// Author:      sbh

#ifndef MODEL_ATTRIBUTEINTEGER_H_
#define MODEL_ATTRIBUTEINTEGER_H_

#include <Model.h>
#include <ModelAPI_AttributeInteger.h>

#include <TDF_Label.hxx>

class ModelAPI_ExpressionInteger;

/**\class Model_AttributeInteger
 * \ingroup DataModel
 * \brief Attribute that contains integer.
 */

class Model_AttributeInteger : public ModelAPI_AttributeInteger
{
 public:
  /// Defines the integer value
  MODEL_EXPORT virtual void setValue(const int theValue);

  /// Returns the integer value
  MODEL_EXPORT virtual int value();

  /// Defines the calculated value
  MODEL_EXPORT virtual void setCalculatedValue(const int theValue);

  /// Defines the text value
  MODEL_EXPORT virtual void setText(const std::string& theText);

  /// Returns the text value
  MODEL_EXPORT virtual std::string text();

  /// Allows to set expression (text) as invalid (by the parameters listener)
  MODEL_EXPORT virtual void setExpressionInvalid(const bool theFlag);

  /// Returns true if text is invalid
  MODEL_EXPORT virtual bool expressionInvalid();

  /// Allows to set expression (text) error (by the parameters listener)
  MODEL_EXPORT virtual void setExpressionError(const std::string& theError);

  /// Returns an expression error
  MODEL_EXPORT virtual std::string expressionError();

  /// Defines the used parameters
  MODEL_EXPORT virtual void setUsedParameters(const std::set<std::string>& theUsedParameters);

  /// Returns the used parameters
  MODEL_EXPORT virtual std::set<std::string> usedParameters() const;

 protected:
  /// Initializes attributes
  Model_AttributeInteger();

  friend class Model_Data;

 private:
  std::shared_ptr<ModelAPI_ExpressionInteger> myExpression;
};

#endif
