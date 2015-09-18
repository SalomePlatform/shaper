/*
 * Parameters_VariableValidator.h
 *
 *  Created on: Apr 9, 2015
 *      Author: sbh
 */

#ifndef PARAMETERSPLUGIN_VARIABLEVALIDATOR_H_
#define PARAMETERSPLUGIN_VARIABLEVALIDATOR_H_

#include <ParametersPlugin.h>
#include <ParametersPlugin_Validators.h>

#include <ModelAPI_AttributeValidator.h>

#include <memory>

/**
 * \class ParametersPlugin_VariableValidator
 * \ingroup Validators
 * \brief Validator for the variable name of parameter.
 */
class ParametersPlugin_VariableValidator : public ModelAPI_AttributeValidator
{
 public:
  PARAMETERSPLUGIN_EXPORT ParametersPlugin_VariableValidator();
  PARAMETERSPLUGIN_EXPORT virtual ~ParametersPlugin_VariableValidator();

  /**
   * \brief Returns true if attribute has a valid parameter name.
   * \param theAttribute the checked attribute
   * \param theArguments arguments of the attribute
   * \param theError the error string message if validation fails
   */
  PARAMETERSPLUGIN_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                               const std::list<std::string>& theArguments,
                                               std::string& theError) const;

 protected:
  /// Returns true if theString is a valid variable name.
  PARAMETERSPLUGIN_EXPORT bool isVariable(const std::string& theString) const;
  /// Returns true if theString is unique parameter name in the document of theAttribute.
  PARAMETERSPLUGIN_EXPORT bool isUnique(const AttributePtr& theAttribute,
                                        const std::string& theString) const;
};

/**
 * \class ParametersPlugin_ExpressionValidator
 * \ingroup Validators
 * \brief Validator for the expression of parameter.
 */
class ParametersPlugin_ExpressionValidator: public ModelAPI_AttributeValidator
{
 public:
  PARAMETERSPLUGIN_EXPORT ParametersPlugin_ExpressionValidator();
  PARAMETERSPLUGIN_EXPORT virtual ~ParametersPlugin_ExpressionValidator();

  /**
   * \brief Returns true if attribute has a valid parameter expression.
   * \param theAttribute the checked attribute
   * \param theArguments arguments of the attribute
   * \param theError the error string message if validation fails
   */
  PARAMETERSPLUGIN_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                               const std::list<std::string>& theArguments,
                                               std::string& theError) const;
};


#endif /* PARAMETERSPLUGIN_VARIABLEVALIDATOR_H_ */
