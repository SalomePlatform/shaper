// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_ValidatorExtrusionBase.h
// Created:     16 Sep 2015
// Author:      Natalia ERMOLAEVA

#ifndef FeaturesPlugin_ValidatorExtrusionBase_H
#define FeaturesPlugin_ValidatorExtrusionBase_H

#include "ModelAPI_AttributeValidator.h"

/** \class FeaturesPlugin_ValidatorExtrusionBase
 *  \ingroup Validators
 *  \brief A validator of selection
 */
class FeaturesPlugin_ValidatorExtrusionBase : public ModelAPI_AttributeValidator
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
