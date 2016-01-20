// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_Validators.h
// Created:     01 Aug 2014
// Author:      Vitaly SMETANNIKOV

#ifndef SketchPlugin_Validators_H
#define SketchPlugin_Validators_H

#include "SketchPlugin.h"
#include <ModelAPI_AttributeValidator.h>
#include <ModelAPI_FeatureValidator.h>

/**\class SketchPlugin_DistanceAttrValidator
 * \ingroup Validators
 * \brief Validator for the distance input.
 *
 * It just checks that distance is greater than zero.
 */
class SketchPlugin_DistanceAttrValidator : public ModelAPI_AttributeValidator
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

/**\class SketchPlugin_TangentAttrValidator
 * \ingroup Validators
 * \brief Validator for the tangent constraint input.
 *
 * It just checks that distance is greater than zero.
 */
class SketchPlugin_TangentAttrValidator : public ModelAPI_AttributeValidator
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


/**\class SketchPlugin_NotFixedValidator
 * \ingroup Validators
 * \brief Validator for the rigid constraint input.
 *
 * It just checks there is no rigid constraint for the current feature.
 */
class SketchPlugin_NotFixedValidator : public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is not used in another rigid constraint
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       std::string& theError) const;
};

/**\class SketchPlugin_EqualAttrValidator
 * \ingroup Validators
 * \brief Validator for the equal constraint input.
 *
 * It checks that attributes of the Equal constraint are correct.
 */
class SketchPlugin_EqualAttrValidator : public ModelAPI_AttributeValidator
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

/**\class SketchPlugin_MirrorAttrValidator
 * \ingroup Validators
 * \brief Validator for the mirror constraint input.
 *
 * It checks that attributes of the Mirror constraint are correct.
 */
class SketchPlugin_MirrorAttrValidator : public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute (not used)
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       std::string& theError) const;
};


/**\class SketchPlugin_CoincidenceAttrValidator
 * \ingroup Validators
 * \brief Validator for the coincidence constraint input.
 *
 * It checks that attributes of the Coincidence constraint are correct.
 */
class SketchPlugin_CoincidenceAttrValidator : public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute (not used)
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       std::string& theError) const;
};


/**\class SketchPlugin_CopyValidator
 * \ingroup Validators
 * \brief Validator for the constraints which create features.
 *
 * Applicable only for features, which creates another features. It verifies the produced
 * features of current constraint don't become into the list of initial features
 */
class SketchPlugin_CopyValidator : public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute (not used)
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       std::string& theError) const;
};

/**\class SketchPlugin_SolverErrorValidator
 * \ingroup Validators
 * \brief Validator for the solver error.
 *
 * Simply checks that solver error attribute is empty. Returns the attribute value as an error.
 */
class SketchPlugin_SolverErrorValidator : public ModelAPI_FeatureValidator
{
 public:
  //! returns true if there are no solver errors
  //! \param theFeature the checked feature
  //! \param theArguments arguments of the feature (not used)
  //! \param theError error message
  virtual bool isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                       const std::list<std::string>& theArguments,
                       std::string& theError) const;

  /// Returns true if the attribute in feature is not obligatory for the feature execution
  virtual bool isNotObligatory(std::string theFeature, std::string theAttribute);
};

/**\class SketchPlugin_FilletVertexValidator
 * \ingroup Validators
 * \brief Validator for the point for fillet creation.
 *
 * Checks that selected point have exactly two coincident lines.
 */
class SketchPlugin_FilletVertexValidator : public ModelAPI_AttributeValidator
{
public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute (not used)
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       std::string& theError) const;
};


/**\class SketchPlugin_MiddlePointAttrValidator
 * \ingroup Validators
 * \brief Validator for the middle point constraint input.
 *
 * It checks that attributes of the Middle point constraint are correct.
 */
class SketchPlugin_MiddlePointAttrValidator : public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute (not used)
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       std::string& theError) const;
};

#endif
