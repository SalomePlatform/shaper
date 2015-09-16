// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_ValidatorTransform.h
// Created:     16 Sep 2015
// Author:      Natalia ERMOLAEVA

#ifndef FeaturesPlugin_ValidatorTransform_H
#define FeaturesPlugin_ValidatorTransform_H

#include "ModelAPI_AttributeValidator.h"

/**
* \ingroup Validators
* A validator of selection
*/
class FeaturesPlugin_ValidatorTransform : public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       std::string& theError) const;
};

#endif
