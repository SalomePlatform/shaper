/*
 * Parameters_VariableValidator.cpp
 *
 *  Created on: Apr 9, 2015
 *      Author: sbh
 */

#include <ParametersPlugin_Validators.h>

#include <ParametersPlugin_Parameter.h>

#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_Tools.h>

ParametersPlugin_VariableValidator::ParametersPlugin_VariableValidator()
{
}

ParametersPlugin_VariableValidator::~ParametersPlugin_VariableValidator()
{
}

bool ParametersPlugin_VariableValidator::isValid(const AttributePtr& theAttribute,
                                                 const std::list<std::string>& theArguments,
                                                 std::string& theError) const
{
  AttributeStringPtr aStrAttr = std::dynamic_pointer_cast<ModelAPI_AttributeString>(theAttribute);
  if (!aStrAttr->isInitialized()) {
    theError = "Attribute \"" + aStrAttr->id() + "\" is not initialized.";
    return false;
  }
  bool isEmptyExpr = aStrAttr->value().empty();
  if (isEmptyExpr) {
    theError = "Attribute \"" + aStrAttr->id() + "\" value is empty.";
    return false;
  }
  if (!isVariable(aStrAttr->value())) {
    theError = "Incorrect variable name.";
    return false;
  } 
  if (!isUnique(theAttribute, aStrAttr->value())) {
    theError = "Variable name is not unique.";
    return false;
  }
  return true;
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

bool ParametersPlugin_VariableValidator::isUnique(const AttributePtr& theAttribute,
                                                  const std::string& theString) const
{
  DocumentPtr aDocument = theAttribute->owner()->document();
  for (int anIndex = 0, aSize = aDocument->size(ModelAPI_ResultParameter::group());
       anIndex < aSize; ++anIndex) {
    ObjectPtr aParamObj = aDocument->object(ModelAPI_ResultParameter::group(), anIndex);
    if (aParamObj->data()->name() != theString)
      continue;
    ResultParameterPtr aParam = std::dynamic_pointer_cast<ModelAPI_ResultParameter>(aParamObj);
    if (!aParam.get())
      continue;
    FeaturePtr aFeature = ModelAPI_Feature::feature(aParam);
    if (aFeature == theAttribute->owner())
      continue;
    return false;
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
                                                   const std::list<std::string>& theArguments,
                                                   std::string& theError) const
{
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
  ResultParameterPtr aParam =
      std::dynamic_pointer_cast<ModelAPI_ResultParameter>(aFeature->firstResult());

  AttributeStringPtr aStrAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeString>(theAttribute);
  if (!aStrAttr->isInitialized()) {
    theError = "Attribute \"" + aStrAttr->id() + "\" is not initialized.";
    return false;
  }
  bool isEmptyExpr = aStrAttr->value().empty();
  if (isEmptyExpr) {
    theError = "Expression is empty.";
    return false;
  }

  if (!aParam.get()) {
    theError = "Result is empty.";
    return false;
  }

  theError = aFeature->string(ParametersPlugin_Parameter::EXPRESSION_ERROR_ID())->value();
  return theError.empty();
}
