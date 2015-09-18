// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_ValidatorTransform.h
// Created:     16 Sep 2015
// Author:      Natalia ERMOLAEVA

#ifndef FeaturesPlugin_ValidatorTransform_H
#define FeaturesPlugin_ValidatorTransform_H

#include "ModelAPI_AttributeValidator.h"

/** \class FeaturesPlugin_ValidatorTransform
 *  \ingroup Validators
 *  \brief A validator of selection
 */
class FeaturesPlugin_ValidatorTransform : public ModelAPI_AttributeValidator
{
 public:
  /** \return true if attribute is valid
   *  \param theAttribute the checked attribute
   *  \param theArguments arguments of the attribute
   *  \param theError error message
   */
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       std::string& theError) const;
};

#endif
