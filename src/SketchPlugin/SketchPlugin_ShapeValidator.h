// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        Model_ResultValidators.h
// Created:     27 Feb 2015
// Author:      Natalia ERMOLAEVA

#ifndef SketchPlugin_ShapeValidator_H
#define SketchPlugin_ShapeValidator_H

#include <SketchPlugin.h>
#include <ModelAPI_ShapeValidator.h>
#include <ModelAPI_Object.h>

/**\class SketchPlugin_ResultPointValidator
 * \ingroup Validators
 * \brief Validator for the points selection
 *
 * Allows to select points only.
 */
class SketchPlugin_ShapeValidator : public ModelAPI_ShapeValidator
{
public:
  // returns true if there is an empty or non-external shape on the feature.
  // in the given shape or in the list of attribute shapes
  /// \param theFeature a feature to check
  /// \param theArguments a filter parameters
  /// \param theObject an object
  /// \param theAttribute an attribute to check
  /// \param theShape a shape
  SKETCHPLUGIN_EXPORT virtual bool isValid(const FeaturePtr& theFeature,
                               const std::list<std::string>& theArguments,
                               const ObjectPtr& theObject, const AttributePtr& theAttribute,
                               const GeomShapePtr& theShape) const;
};

#endif
