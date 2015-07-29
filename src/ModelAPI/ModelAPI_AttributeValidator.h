// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeValidator.h
// Created:     4 Sep 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_AttributeValidator_H
#define ModelAPI_AttributeValidator_H

#include <ModelAPI.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_Validator.h>

/**
 * Generic validator for any attribute of a feature.
 */
class ModelAPI_AttributeValidator : public ModelAPI_Validator
{
public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       std::string& theError = std::string()) const = 0;

  MODELAPI_EXPORT ~ModelAPI_AttributeValidator();
};

#endif
