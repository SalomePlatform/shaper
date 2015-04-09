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
  myPyVariableRegex = std::regex("[_a-zA-Z][a-zA-Z0-9_]*");
}

ParametersPlugin_VariableValidator::~ParametersPlugin_VariableValidator()
{
}

bool ParametersPlugin_VariableValidator::isValid(const AttributePtr& theAttribute,
                                                 const std::list<std::string>& theArguments) const
{
  AttributeStringPtr aStrAttr = std::dynamic_pointer_cast<ModelAPI_AttributeString>(theAttribute);
  bool result = std::regex_match(aStrAttr->value(), myPyVariableRegex);
  return result;
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
