// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_Expression.h
// Created:     5 Aug 2015
// Author:      Sergey POKHODENKO

#ifndef Model_Expression_H_
#define Model_Expression_H_

#include "Model.h"
#include "ModelAPI_Expression.h"

#include <TDataStd_Comment.hxx>
#include <TDataStd_ExtStringList.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Real.hxx>

#include <TDF_Label.hxx>

/**\class Model_Expression
 * \ingroup DataModel
 * \brief Implementation of ModelAPI_Expression.
 */
class Model_Expression : public ModelAPI_Expression
{
  Handle_TDataStd_Real myReal; ///< double is Real attribute
  Handle_TDataStd_Name myText; ///< Text representation of the attribute (may differ for parameters)
  Handle_TDataStd_Comment myError; ///< Error of expression of the text attribute
  Handle_TDataStd_ExtStringList myUsedParameters; ///< Parameters used in expression
 public:
  /// Defines the double value
  MODEL_EXPORT virtual void setValue(const double theValue);

  /// Returns the double value
  MODEL_EXPORT virtual double value();

  /// Sets the text of this Expression
  MODEL_EXPORT virtual void setText(const std::string& theText);

  /// Returns the text of this Expression
  MODEL_EXPORT virtual std::string text() const;

  /// Allows to set expression (text) as invalid (by the parameters listener)
  MODEL_EXPORT virtual void setInvalid(const bool theFlag);

  /// Returns true if text is invalid
  MODEL_EXPORT virtual bool isInvalid();

  /// Allows to set expression (text) error (by the parameters listener)
  MODEL_EXPORT virtual void setError(const std::string& theError);

  /// Returns an expression error
  MODEL_EXPORT virtual std::string error();

  /// Defines the used parameters (by the parameters listener)
  MODEL_EXPORT virtual void setUsedParameters(const std::set<std::string>& theUsedParameters);

  /// Returns the used parameters
  MODEL_EXPORT virtual std::set<std::string> usedParameters() const;

 protected:
  /// Initializes attributes
  Model_Expression(TDF_Label& theLabel);

  friend class Model_Data;
};

#endif
