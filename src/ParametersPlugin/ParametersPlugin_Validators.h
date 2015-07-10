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

class ParametersPlugin_VariableValidator : public ModelAPI_AttributeValidator
{
 public:
  PARAMETERSPLUGIN_EXPORT ParametersPlugin_VariableValidator();
  PARAMETERSPLUGIN_EXPORT virtual ~ParametersPlugin_VariableValidator();

  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  PARAMETERSPLUGIN_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                               const std::list<std::string>& theArguments) const;

 protected:
  PARAMETERSPLUGIN_EXPORT bool isVariable(const std::string& theString) const;
  PARAMETERSPLUGIN_EXPORT bool isUnique(const AttributePtr& theAttribute, 
                                        const std::string& theString) const;
};

class ParametersPlugin_ExpressionValidator: public ModelAPI_AttributeValidator
{
 public:
  PARAMETERSPLUGIN_EXPORT ParametersPlugin_ExpressionValidator();
  PARAMETERSPLUGIN_EXPORT virtual ~ParametersPlugin_ExpressionValidator();

  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  PARAMETERSPLUGIN_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                               const std::list<std::string>& theArguments) const;
};


#endif /* PARAMETERSPLUGIN_VARIABLEVALIDATOR_H_ */
