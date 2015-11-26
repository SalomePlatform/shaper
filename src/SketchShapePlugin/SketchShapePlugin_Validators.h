// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchShapePlugin_Validators.h
// Created:     25 Nov 2015
// Author:      Natalia ERMOLAEVA

#ifndef SKETCHSHAPEPLUGIN_VALIDATORS_H
#define SKETCHSHAPEPLUGIN_VALIDATORS_H

#include "SketchShapePlugin.h"
#include <ModelAPI_AttributeValidator.h>

/**\class SketchShapePlugin_FeatureValidator
 * \ingroup Validators
 * \brief Validator for an attribute of sketch shape feature.
 *
 * It is empty.
 */
class SketchShapePlugin_FeatureValidator : public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       std::string& theError) const;
};

#endif
