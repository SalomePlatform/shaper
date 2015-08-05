// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_Expression.h
// Created:     5 Aug 2015
// Author:      Sergey POKHODENKO

#ifndef ModelAPI_Expression_H_
#define ModelAPI_Expression_H_

#include "ModelAPI.h"
#include <memory>
#include <set>
#include <string>

/**\class ModelAPI_Expression
 * \ingroup DataModel
 * \brief Expression for calculated values.
 */
class ModelAPI_Expression
{
 protected:
  bool myIsInitialized; ///< is some value assigned to this attribute

 public:
  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_Expression();

  /// Returns true if attribute was initialized by some value
  MODELAPI_EXPORT virtual bool isInitialized();

  /// Makes attribute initialized
  MODELAPI_EXPORT virtual void setInitialized();

  /// Defines the double value
  MODELAPI_EXPORT virtual void setValue(const double theValue) = 0;

  /// Returns the double value
  MODELAPI_EXPORT virtual double value() = 0;

  /// Sets the text of this Expression
  MODELAPI_EXPORT virtual void setText(const std::string& theText) = 0;

  /// Returns the text of this Expression
  MODELAPI_EXPORT virtual std::string text() const = 0;

  /// Allows to set expression (text) as invalid (by the parameters listener)
  MODELAPI_EXPORT virtual void setInvalid(const bool theFlag) = 0;

  /// Returns true if text is invalid
  MODELAPI_EXPORT virtual bool isInvalid() = 0;

  /// Allows to set expression (text) error (by the parameters listener)
  MODELAPI_EXPORT virtual void setError(const std::string& theError) = 0;

  /// Returns an expression error
  MODELAPI_EXPORT virtual std::string error() = 0;

  /// Defines the used parameters (by the parameters listener)
  MODELAPI_EXPORT virtual void setUsedParameters(const std::set<std::string>& theUsedParameters) = 0;

  /// Returns the used parameters
  MODELAPI_EXPORT virtual std::set<std::string> usedParameters() const = 0;

 protected:
  /// Objects are created for features automatically
  MODELAPI_EXPORT ModelAPI_Expression();

  friend class Model_Data;
};

//! Pointer on Expression object
typedef std::shared_ptr<ModelAPI_Expression> ExpressionPtr;

#endif
