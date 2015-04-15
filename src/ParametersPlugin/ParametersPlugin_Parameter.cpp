// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        ParametersPlugin_Parameter.cpp
// Created:     23 MArch 2015
// Author:      sbh

#include "ParametersPlugin_Parameter.h"
#include <ParametersPlugin_PyInterp.h>

#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_AttributeDouble.h>

#include <string>
#include <sstream>

ParametersPlugin_Parameter::ParametersPlugin_Parameter()
{
  myInterp = new ParametersPlugin_PyInterp();
  myInterp->initialize();
}

ParametersPlugin_Parameter::~ParametersPlugin_Parameter()
{
  myInterp->destroy();
  delete myInterp;
}

void ParametersPlugin_Parameter::initAttributes()
{
  data()->addAttribute(ParametersPlugin_Parameter::VARIABLE_ID(),
                       ModelAPI_AttributeString::typeId());
  data()->addAttribute(ParametersPlugin_Parameter::EXPRESSION_ID(),
                       ModelAPI_AttributeString::typeId());
}

bool ParametersPlugin_Parameter::isInHistory()
{
  return false;
}

void ParametersPlugin_Parameter::attributeChanged(const std::string&)
{
  ResultParameterPtr aParam = document()->createParameter(data());

  std::string anExpression = string(ParametersPlugin_Parameter::EXPRESSION_ID())->value();
  if(anExpression.empty()) {
    // clear error/result if the expression is empty
    setError("", false);
    return;
  }
  std::string outErrorMessage;
  double aValue = evaluate(anExpression, outErrorMessage);
  // Name
  std::string aName = string(ParametersPlugin_Parameter::VARIABLE_ID())->value();
  std::ostringstream sstream;
  sstream << aValue;
  std::string aParamValue = sstream.str();
  data()->setName(aName);
  aParam->data()->setName(aName);
  // Error
  std::string aStateMsg;
  if (!outErrorMessage.empty()) {
    aStateMsg = "Error: " + outErrorMessage;
    data()->execState(ModelAPI_StateExecFailed);
  } 
  setError(aStateMsg, false);
  // Value
  AttributeDoublePtr aValueAttribute = aParam->data()->real(ModelAPI_ResultParameter::VALUE());
  aValueAttribute->setValue(aValue);
  setResult(aParam);
}

void ParametersPlugin_Parameter::execute()
{
}

double ParametersPlugin_Parameter::evaluate(const std::string& theExpression, std::string& theError)
{
  std::list<std::string> anExprParams = myInterp->compile(theExpression);
  // find expression's params in the model
  std::list<std::string> aContext;
  std::list<std::string>::iterator it = anExprParams.begin();
  for ( ; it != anExprParams.end(); it++) {
    std::string aParamName = *it;
    ObjectPtr aParamObj = document()->objectByName(ModelAPI_ResultParameter::group(), aParamName);
    ResultParameterPtr aParam = std::dynamic_pointer_cast<ModelAPI_ResultParameter>(aParamObj);
    if(!aParam.get()) continue;
    AttributeDoublePtr aValueAttribute = aParam->data()->real(ModelAPI_ResultParameter::VALUE());
    std::ostringstream sstream;
    sstream << aValueAttribute->value();
    std::string aParamValue = sstream.str();
    aContext.push_back(aParamName + "=" + aParamValue);
  }
  myInterp->extendLocalContext(aContext);
  return myInterp->evaluate(theExpression, theError);
}
