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

  data()->addAttribute(EXPRESSION_ERROR_ID(), ModelAPI_AttributeString::typeId());
  data()->string(EXPRESSION_ERROR_ID())->setIsArgument(false);
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EXPRESSION_ERROR_ID());

  data()->addAttribute(ARGUMENTS_ID(), ModelAPI_AttributeRefList::typeId());
  data()->reflist(ARGUMENTS_ID())->setIsArgument(false);
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), ARGUMENTS_ID());
}

bool ParametersPlugin_Parameter::isInHistory()
{
  return false;
}

void ParametersPlugin_Parameter::attributeChanged(const std::string& theID)
{
  if (theID == EXPRESSION_ID())
    updateExpression();

  data()->execState(ModelAPI_StateMustBeUpdated);
}

void ParametersPlugin_Parameter::updateName()
{
  std::string aName = string(VARIABLE_ID())->value();
  data()->setName(aName);

  ResultParameterPtr aParam = document()->createParameter(data());
  aParam->data()->setName(aName);
  setResult(aParam);
}

bool ParametersPlugin_Parameter::updateExpression()
{
  std::string anExpression = string(EXPRESSION_ID())->value();

  std::string outErrorMessage;
  double aValue = evaluate(anExpression, outErrorMessage);

  data()->string(EXPRESSION_ERROR_ID())->setValue(outErrorMessage);
  if (!outErrorMessage.empty())
    return false;

  ResultParameterPtr aParam = document()->createParameter(data());
  AttributeDoublePtr aValueAttribute = aParam->data()->real(ModelAPI_ResultParameter::VALUE());
  aValueAttribute->setValue(aValue);
  setResult(aParam);
  return true;
}

void ParametersPlugin_Parameter::execute()
{
  updateName();
  if (!updateExpression())
    setError("Expression error.", false);
}

double ParametersPlugin_Parameter::evaluate(const std::string& theExpression, std::string& theError)
{
  std::list<std::string> anExprParams = myInterp->compile(theExpression);
  // find expression's params in the model
  std::list<std::string> aContext;
  std::list<std::string>::iterator it = anExprParams.begin();
  std::list<ResultParameterPtr> aParamsList;
  for ( ; it != anExprParams.end(); it++) {
    std::string& aVariableName = *it;

    // Parameter with the same name should be searched in the parent document.
    // For the PartSet assume that the parameter is absent.
    // Currently there is no way to get parent document, so we get PartSet for all.
    DocumentPtr aDocument = document();
    if (data()->name() == aVariableName) {
      if (aDocument = ModelAPI_Session::get()->moduleDocument())
        continue;
      aDocument = ModelAPI_Session::get()->moduleDocument();
    }

    double aValue;
    ResultParameterPtr aParamRes;
    if (!ModelAPI_Tools::findVariable(aVariableName, aValue, aParamRes, aDocument)) continue;
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
