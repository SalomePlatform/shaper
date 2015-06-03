// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        ParametersPlugin_Parameter.cpp
// Created:     23 MArch 2015
// Author:      sbh

#include <pyconfig.h>

#include "ParametersPlugin_Parameter.h"
#include <ParametersPlugin_PyInterp.h>

#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <string>
#include <sstream>

ParametersPlugin_Parameter::ParametersPlugin_Parameter()
{
  myInterp = std::shared_ptr<ParametersPlugin_PyInterp>(new ParametersPlugin_PyInterp());
  myInterp->initialize();
}

ParametersPlugin_Parameter::~ParametersPlugin_Parameter()
{
}

void ParametersPlugin_Parameter::initAttributes()
{
  data()->addAttribute(VARIABLE_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(EXPRESSION_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(ARGUMENTS_ID(), ModelAPI_AttributeRefList::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), ARGUMENTS_ID());
}

bool ParametersPlugin_Parameter::isInHistory()
{
  return false;
}

void ParametersPlugin_Parameter::attributeChanged(const std::string& theID)
{
  if (theID == EXPRESSION_ID()) { // recompute only on change of the expression
    ResultParameterPtr aParam = document()->createParameter(data());

    std::string anExpression = string(EXPRESSION_ID())->value();
    if(anExpression.empty()) {
      // clear error/result if the expression is empty
      setError("", false);
      return;
    }
    std::string outErrorMessage;
    double aValue = evaluate(anExpression, outErrorMessage);
    // Name
    std::string aName = string(VARIABLE_ID())->value();
    std::ostringstream sstream;
    sstream << aValue;
    std::string aParamValue = sstream.str();
    data()->setName(aName);
    aParam->data()->setName(aName);
    // Error
    if (!outErrorMessage.empty()) {
      std::string aStateMsg("Error: " + outErrorMessage);
      data()->execState(ModelAPI_StateExecFailed);
      setError(aStateMsg, false);
    } else {
      static const std::string anEmptyMsg(""); // it is checked in the validator by the empty message
      setError(anEmptyMsg, false);
      data()->execState(ModelAPI_StateDone);
    }
    // Value
    AttributeDoublePtr aValueAttribute = aParam->data()->real(ModelAPI_ResultParameter::VALUE());
    aValueAttribute->setValue(aValue);
    setResult(aParam);
  }
}

void ParametersPlugin_Parameter::execute()
{
  // just call recompute
  attributeChanged(EXPRESSION_ID());
}

double ParametersPlugin_Parameter::evaluate(const std::string& theExpression, std::string& theError)
{

  std::list<std::string> anExprParams = myInterp->compile(theExpression);
  // find expression's params in the model
  std::list<std::string> aContext;
  std::list<std::string>::iterator it = anExprParams.begin();
  std::list<ResultParameterPtr> aParamsList;
  for ( ; it != anExprParams.end(); it++) {
    double aValue;
    ResultParameterPtr aParamRes;
    if (!ModelAPI_Tools::findVariable(*it, aValue, aParamRes)) continue;
    aParamsList.push_back(aParamRes);

    std::ostringstream sstream;
    sstream << aValue;
    std::string aParamValue = sstream.str();
    aContext.push_back(*it + "=" + aParamValue);
  }
  // compare the list of parameters to store if changed
  AttributeRefListPtr aParams = reflist(ARGUMENTS_ID());
  bool aDifferent = aParams->size() != aParamsList.size();
  if (!aDifferent) {
    std::list<ResultParameterPtr>::iterator aNewIter = aParamsList.begin();
    std::list<ObjectPtr> anOldList = aParams->list();
    std::list<ObjectPtr>::iterator anOldIter = anOldList.begin();
    for(; !aDifferent && aNewIter != aParamsList.end(); aNewIter++, anOldIter++) {
      if (*aNewIter != *anOldIter)
        aDifferent = true;
    }
  }
  if (aDifferent) {
    aParams->clear();
    std::list<ResultParameterPtr>::iterator aNewIter = aParamsList.begin();
    for(; aNewIter != aParamsList.end(); aNewIter++) {
      aParams->append(*aNewIter);
    }
  }

  myInterp->extendLocalContext(aContext);
  double result = myInterp->evaluate(theExpression, theError);
  myInterp->clearLocalContext();
  return result;
}

bool ParametersPlugin_Parameter::isPreviewNeeded() const
{
  return false;
}
