// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeValidator.h
// Created:     29 July 2015
// Author:      Sergey POKHODENKO

#ifndef Model_AttributeValidator_H
#define Model_AttributeValidator_H

#include "Model.h"
#include <ModelAPI_AttributeValidator.h>

/**\class Model_AttributeValidator
 * \ingroup DataModel
 * \brief The geneneric validator for the attribute.
 *
 * Can be redefined for some specific attributes, but by default for each attribute this validator is
 * used: it checks the argument and if it contains invalid expression the attribute is invalid.
 */
class Model_AttributeValidator : public ModelAPI_AttributeValidator
{
public:
  /// \param theAttribute the checked attribute
  /// \param theArguments arguments of the attribute
  /// \param theError erros message produced by validator to the user if it fails
  /// \returns true if attribute is valid
  MODEL_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                    const std::list<std::string>& theArguments,
                                    std::string& theError) const;
};

#endif // Model_AttributeValidator_H
