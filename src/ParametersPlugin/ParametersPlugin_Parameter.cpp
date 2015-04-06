// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        ParametersPlugin_Parameter.cpp
// Created:     23 MArch 2015
// Author:      sbh

#include "ParametersPlugin_Parameter.h"
#include <ParametersPlugin_PyInterp.h>

#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_AttributeDouble.h>


ParametersPlugin_Parameter::ParametersPlugin_Parameter()
{
  myInterp = new ParametersPlugin_PyInterp();
  myInterp->initialize();
}

ParametersPlugin_Parameter::~ParametersPlugin_Parameter()
{
  delete myInterp;
}

void ParametersPlugin_Parameter::initAttributes()
{
  data()->addAttribute(ParametersPlugin_Parameter::VARIABLE_ID(),
                       ModelAPI_AttributeString::typeId());
  data()->addAttribute(ParametersPlugin_Parameter::EXPRESSION_ID(),
                       ModelAPI_AttributeString::typeId());
}

void ParametersPlugin_Parameter::execute()
{
  std::string anExpression = string(ParametersPlugin_Parameter::EXPRESSION_ID())->value();
  if(anExpression.empty())
    return;
  ResultParameterPtr aParameterResult = document()->createParameter(data());
  //AttributeDoublePtr anAttr = aParameterResult->data()->real(ModelAPI_ResultParameter::VALUE());

  double aValue = evaluate(anExpression);
  //anAttr->setValue(aValue);
  setResult(aParameterResult);

}

double ParametersPlugin_Parameter::evaluate(std::string theExpression)
{
  std::list<std::string> anExprParams = myInterp->compile(theExpression);
  // find expression's params in the model
  // todo
  // myInterp->extendLocalContext();
  std::string outError;
  return myInterp->evaluate(theExpression, outError);

}
