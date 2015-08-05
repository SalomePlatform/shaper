// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeDouble.h
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_AttributeDouble_H_
#define Model_AttributeDouble_H_

#include "Model.h"
#include "ModelAPI_AttributeDouble.h"

#include <TDF_Label.hxx>

class ModelAPI_Expression;

/**\class Model_AttributeDouble
 * \ingroup DataModel
 * \brief Attribute that contains real value with double precision.
 */

class Model_AttributeDouble : public ModelAPI_AttributeDouble
{
  std::shared_ptr<ModelAPI_Expression> myExpression;

 public:
  /// Defines the double value
  MODEL_EXPORT virtual void setValue(const double theValue);

  /// Returns the double value
  MODEL_EXPORT virtual double value();

  /// Defines the calculated double value
  MODEL_EXPORT virtual void setCalculatedValue(const double theValue);

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
  Model_AttributeDouble(TDF_Label& theLabel);

  friend class Model_Data;
};

#endif
