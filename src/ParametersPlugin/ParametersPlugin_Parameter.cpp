// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include <pyconfig.h>

#include "ParametersPlugin_Parameter.h"

#include <Locale_Convert.h>

#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Events.h>

#include <string>
#include <sstream>

ParametersPlugin_Parameter::ParametersPlugin_Parameter()
{
}

ParametersPlugin_Parameter::~ParametersPlugin_Parameter()
{
}

void ParametersPlugin_Parameter::initAttributes()
{
  data()->addAttribute(VARIABLE_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(EXPRESSION_ID(), ModelAPI_AttributeString::typeId());

  data()->addAttribute(COMMENT_ID(), ModelAPI_AttributeString::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), COMMENT_ID());

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
  std::wstring aName = string(VARIABLE_ID())->isUValue() ?
      Locale::Convert::toWString(string(VARIABLE_ID())->valueU()) :
      Locale::Convert::toWString(string(VARIABLE_ID())->value());
  data()->setName(aName);

  ResultParameterPtr aParam = document()->createParameter(data());
  std::wstring anOldName = aParam->data()->name();
  aParam->data()->setName(aName);
  setResult(aParam);


  // #2474 : if parameter name now hides/shows the higher level parameter name,
  // update the depended expressions
  DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
  if (aParam->document() != aRootDoc) {
    std::list<std::wstring> aNames; // collect names in the root document that must be checked
    aNames.push_back(aName);
    if (anOldName != aName) {
      aNames.push_back(anOldName);
    }
    std::list<std::wstring>::iterator aNIter = aNames.begin();
    for (; aNIter != aNames.end(); aNIter++) {
      double aValue;
      ResultParameterPtr aRootParam;
      FeaturePtr aThis =
        std::dynamic_pointer_cast<ModelAPI_Feature>(string(VARIABLE_ID())->owner());
      if (ModelAPI_Tools::findVariable(aThis, *aNIter, aValue, aRootParam, aRootDoc)) {
        std::set<std::shared_ptr<ModelAPI_Attribute> > anAttributes =
          aRootParam->data()->refsToMe();
        std::set<std::shared_ptr<ModelAPI_Attribute> >::const_iterator anAttributeIt =
          anAttributes.cbegin();
        for (; anAttributeIt != anAttributes.cend(); ++anAttributeIt) {
          const AttributePtr& anAttribute = *anAttributeIt;
          ModelAPI_AttributeEvalMessage::send(anAttribute, NULL);
        }
      }
    }
  }
}

bool ParametersPlugin_Parameter::updateExpression()
{
  std::wstring anExpression = string(EXPRESSION_ID())->isUValue() ?
      Locale::Convert::toWString(string(EXPRESSION_ID())->valueU()) :
      Locale::Convert::toWString(string(EXPRESSION_ID())->value());

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

double ParametersPlugin_Parameter::evaluate(const std::wstring& /*theExpression*/,
                                            std::string& theError)
{
  FeaturePtr aMyPtr = std::dynamic_pointer_cast<ModelAPI_Feature>(data()->owner());
  std::shared_ptr<ModelAPI_ParameterEvalMessage> aProcessMessage =
    ModelAPI_ParameterEvalMessage::send(aMyPtr, this);

  double aResult = 0;
  if (aProcessMessage->isProcessed()) {
    const std::list<ResultParameterPtr>& aParamsList = aProcessMessage->params();
    aResult = aProcessMessage->result();
    theError = aProcessMessage->error();
    // compare the list of parameters to store if changed
    AttributeRefListPtr aParams = reflist(ARGUMENTS_ID());
    bool aDifferent = aParams->size() != (int)aParamsList.size();
    if (!aDifferent) {
      std::list<ResultParameterPtr>::const_iterator aNewIter = aParamsList.begin();
      std::list<ObjectPtr> anOldList = aParams->list();
      std::list<ObjectPtr>::const_iterator anOldIter = anOldList.begin();
      for(; !aDifferent && aNewIter != aParamsList.end(); aNewIter++, anOldIter++) {
        if (*aNewIter != *anOldIter)
          aDifferent = true;
      }
    }
    if (aDifferent) {
      aParams->clear();
      std::list<ResultParameterPtr>::const_iterator aNewIter = aParamsList.begin();
      for(; aNewIter != aParamsList.end(); aNewIter++) {
        aParams->append(*aNewIter);
      }
    }
  } else { // error: python interpreter is not active
    theError = "Python interpreter is not available";
  }
  return aResult;
}

bool ParametersPlugin_Parameter::isPreviewNeeded() const
{
  return true;
}
