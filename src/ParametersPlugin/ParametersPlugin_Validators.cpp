// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <ParametersPlugin_Validators.h>

#include <ParametersPlugin_Parameter.h>

#include <Events_InfoMessage.h>

#include <Locale_Convert.h>

#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Expression.h>

ParametersPlugin_VariableValidator::ParametersPlugin_VariableValidator()
{
}

ParametersPlugin_VariableValidator::~ParametersPlugin_VariableValidator()
{
}

bool ParametersPlugin_VariableValidator::isValid(const AttributePtr& theAttribute,
                                                 const std::list<std::string>& /*theArguments*/,
                                                 Events_InfoMessage& theError) const
{
  AttributeStringPtr aStrAttr = std::dynamic_pointer_cast<ModelAPI_AttributeString>(theAttribute);
  if (!aStrAttr->isInitialized()) {
    theError = "Attribute \"%1\" is not initialized.";
    theError.arg(aStrAttr->id());
    return false;
  }
  bool isEmptyExpr = aStrAttr->value().empty();
  if (isEmptyExpr) {
    theError = "Attribute \"%1\" value is empty.";
    theError.arg(aStrAttr->id());
    return false;
  }
  if (!ModelAPI_Expression::isVariable(aStrAttr->value())) {
    theError = "Incorrect variable name.";
    return false;
  }
  if (!isUnique(theAttribute, aStrAttr->value())) {
    theError = "Variable name is not unique.";
    return false;
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
    if (Locale::Convert::toString(aParamObj->data()->name()) != theString)
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
                                                   const std::list<std::string>& /*theArguments*/,
                                                   Events_InfoMessage& theError) const
{
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());

  AttributeStringPtr aStrAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeString>(theAttribute);
  if (!aStrAttr->isInitialized()) {
    theError = "Attribute \"%1\" is not initialized.";
    theError.arg(aStrAttr->id());
    return false;
  }
  bool isEmptyExpr = aStrAttr->value().empty();
  if (isEmptyExpr) {
    theError = "Expression is empty.";
    return false;
  }

  theError = aFeature->string(ParametersPlugin_Parameter::EXPRESSION_ERROR_ID())->value();
  return theError.empty();
}
