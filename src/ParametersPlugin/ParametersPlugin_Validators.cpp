/*
 * Parameters_VariableValidator.cpp
 *
 *  Created on: Apr 9, 2015
 *      Author: sbh
 */

#include <ParametersPlugin_Validators.h>

#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultParameter.h>

ParametersPlugin_VariableValidator::ParametersPlugin_VariableValidator()
{
}

ParametersPlugin_VariableValidator::~ParametersPlugin_VariableValidator()
{
}

bool ParametersPlugin_VariableValidator::isValid(const AttributePtr& theAttribute,
                                                 const std::list<std::string>& theArguments) const
{
  AttributeStringPtr aStrAttr = std::dynamic_pointer_cast<ModelAPI_AttributeString>(theAttribute);
  bool result = isVariable(aStrAttr->value());
  return result;
}

bool ParametersPlugin_VariableValidator::isVariable(const std::string& theString) const
{
  if (theString.empty())
    return false;
  std::string::const_iterator it = theString.begin();
  if (!(isalpha(*it) || (*it) == '_') || it == theString.end())
    return false;
  it++;
  for ( ; it != theString.end(); ++it ) {
    if(!(isalnum(*it) || (*it) == '_')) {
      return false;
    }
  }
  return true;
}

ParametersPlugin_ExpressionValidator::ParametersPlugin_ExpressionValidator()
{

}

ParametersPlugin_ExpressionValidator::~ParametersPlugin_ExpressionValidator()
{

}

bool ParametersPlugin_ExpressionValidator::isValid(const AttributePtr& theAttribute,
                                                  const std::list<std::string>& theArguments) const
{
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
  ResultParameterPtr aParam =
      std::dynamic_pointer_cast<ModelAPI_ResultParameter>(aFeature->firstResult());

  AttributeStringPtr aStrAttr =
    std::dynamic_pointer_cast<ModelAPI_AttributeString>(theAttribute);
  bool isEmptyExpr = aStrAttr->value().empty();
  if(isEmptyExpr)
    return false;

  if(!aParam.get())
    return false;

  return aFeature->error().empty();
}
