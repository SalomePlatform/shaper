// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_Validators.h
// Created:     01 Aug 2014
// Author:      Vitaly SMETANNIKOV

#ifndef SketchPlugin_Validators_H
#define SketchPlugin_Validators_H

#include "SketchPlugin.h"
#include <ModelAPI_AttributeValidator.h>

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
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       std::string& theError) const;
};


/**\class SketchPlugin_NotFixed
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
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       std::string& theError) const;
};

#endif
