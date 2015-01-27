// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        Model_ResultValidators.h
// Created:     23 July 2014
// Author:      Vitaly SMETANNIKOV

#ifndef Model_ResultValidators_H
#define Model_ResultValidators_H

#include <SketchPlugin.h>
#include <ModelAPI_ResultValidator.h>
#include <ModelAPI_Object.h>

/**\class SketchPlugin_ResultPointValidator
 * \ingroup Validators
 * \brief Validator for the points selection
 *
 * Allows to select points only.
 */
class SketchPlugin_ResultPointValidator : public ModelAPI_ResultValidator
{
public:
  /// Returns true if theObject is a point
  SKETCHPLUGIN_EXPORT virtual bool isValid(const ObjectPtr theObject) const;
};

/**\class SketchPlugin_ResultLineValidator
 * \ingroup Validators
 * \brief Validator for the linear segments selection
 *
 * Allows to select linear segments only.
 */
class SketchPlugin_ResultLineValidator : public ModelAPI_ResultValidator
{
public:
  /// Returns true if theObject is a line
  SKETCHPLUGIN_EXPORT virtual bool isValid(const ObjectPtr theObject) const;
};

/**\class SketchPlugin_ResultArcValidator
 * \ingroup Validators
 * \brief Validator for the circular segments selection
 *
 * Allows to select circular segments only.
 */
class SketchPlugin_ResultArcValidator : public ModelAPI_ResultValidator
{
 public:
  /// Returns true if theObject is an arc
  SKETCHPLUGIN_EXPORT virtual bool isValid(const ObjectPtr theObject) const;
};

#endif
