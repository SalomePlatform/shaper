/*
 * ParametersPlugin_EvalListener.cpp
 *
 *  Created on: Apr 28, 2015
 *      Author: sbh
 */

#include <pyconfig.h>

#include <ParametersPlugin_EvalListener.h>
#include <ParametersPlugin_Parameter.h>
#include <ParametersPlugin_PyInterp.h>

#include <Events_Error.h>

#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeValidator.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>

#include <ModelAPI_AttributeDouble.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>

#include <string>
#include <set>
#include <sstream>

ParametersPlugin_EvalListener::ParametersPlugin_EvalListener()
{
  Events_Loop* aLoop = Events_Loop::loop();
  const Events_ID kEvaluationEvent = ModelAPI_AttributeEvalMessage::eventId();
  aLoop->registerListener(this, kEvaluationEvent, NULL, true);
  const Events_ID kObjectRenamedEvent = ModelAPI_ObjectRenamedMessage::eventId();
  aLoop->registerListener(this, kObjectRenamedEvent, NULL, true);

  myInterp = std::shared_ptr<ParametersPlugin_PyInterp>(new ParametersPlugin_PyInterp());
  myInterp->initialize();
}

ParametersPlugin_EvalListener::~ParametersPlugin_EvalListener()
{
}

void ParametersPlugin_EvalListener::processEvent(
    const std::shared_ptr<Events_Message>& theMessage)
{
  if (!theMessage.get())
    return;

  const Events_ID kEvaluationEvent = ModelAPI_AttributeEvalMessage::eventId();
  const Events_ID kObjectRenamedEvent = ModelAPI_ObjectRenamedMessage::eventId();
  if (theMessage->eventID() == kEvaluationEvent) {
    processEvaluationEvent(theMessage);
  } else if (theMessage->eventID() == kObjectRenamedEvent) {
    processObjectRenamedEvent(theMessage);
  } else {
    Events_Error::send(std::string("ParametersPlugin python interpreter, unhandled message caught: ")
                       + theMessage->eventID().eventText());
  }
}

double ParametersPlugin_EvalListener::evaluate(const std::string& theExpression, std::string& theError, 
                                               const std::shared_ptr<ModelAPI_Document>& theDocument)
{
  std::list<std::string> anExprParams = myInterp->compile(theExpression);
  // find expression's params in the model
  std::list<std::string> aContext;
  std::list<std::string>::iterator it = anExprParams.begin();
  for ( ; it != anExprParams.end(); it++) {
    double aValue;
    ResultParameterPtr aParamRes;
    // If variable does not exist python interpreter will generate an error. It is OK.
    if (!ModelAPI_Tools::findVariable(*it, aValue, aParamRes, theDocument)) continue;

    std::ostringstream sstream;
    sstream << aValue;
    std::string aParamValue = sstream.str();
    aContext.push_back(*it + "=" + aParamValue);
  }
  myInterp->extendLocalContext(aContext);
  double result = myInterp->evaluate(theExpression, theError);
  myInterp->clearLocalContext();
  return result;
}

std::set<std::string> toSet(const std::list<std::string>& theContainer)
{
  return std::set<std::string>(theContainer.begin(), theContainer.end());
}

void ParametersPlugin_EvalListener::processEvaluationEvent(
    const std::shared_ptr<Events_Message>& theMessage)
{
  std::shared_ptr<ModelAPI_AttributeEvalMessage> aMessage =
      std::dynamic_pointer_cast<ModelAPI_AttributeEvalMessage>(theMessage);

  if (aMessage->attribute()->attributeType() == ModelAPI_AttributeDouble::typeId()) {
    AttributeDoublePtr anAttribute =
        std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aMessage->attribute());
    std::string anError;
    double aValue = evaluate(anAttribute->text(), anError, anAttribute->owner()->document());
    bool isValid = anError.empty();
    if (isValid)
      anAttribute->setCalculatedValue(aValue);
    anAttribute->setUsedParameters(isValid ? toSet(myInterp->compile(anAttribute->text())) : std::set<std::string>());
    anAttribute->setExpressionInvalid(!isValid);
    anAttribute->setExpressionError(anAttribute->text().empty() ? "" : anError);
  } else
  if (aMessage->attribute()->attributeType() == GeomDataAPI_Point::typeId()) {
    AttributePointPtr anAttribute =
        std::dynamic_pointer_cast<GeomDataAPI_Point>(aMessage->attribute());
    std::string aText[] = {
      anAttribute->textX(),
      anAttribute->textY(),
      anAttribute->textZ()
    };
    double aCalculatedValue[] = {
      anAttribute->x(),
      anAttribute->y(),
      anAttribute->z()
    };
    for (int i = 0; i < 3; ++i) {
      std::string anError;
      double aValue = evaluate(aText[i], anError, anAttribute->owner()->document());
      bool isValid = anError.empty();
      if (isValid) aCalculatedValue[i] = aValue;
      anAttribute->setUsedParameters(i, isValid ? toSet(myInterp->compile(aText[i])) : std::set<std::string>());
      anAttribute->setExpressionInvalid(i, !isValid);
      anAttribute->setExpressionError(i, aText[i].empty() ? "" : anError);
    }
    anAttribute->setCalculatedValue(aCalculatedValue[0],
                                    aCalculatedValue[1],
                                    aCalculatedValue[2]);
  } else
  if (aMessage->attribute()->attributeType() == GeomDataAPI_Point2D::typeId()) {
    AttributePoint2DPtr anAttribute =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aMessage->attribute());
    std::string aText[] = {
      anAttribute->textX(),
      anAttribute->textY()
    };
    double aCalculatedValue[] = {
      anAttribute->x(),
      anAttribute->y()
    };
    for (int i = 0; i < 2; ++i) {
      std::string anError;
      double aValue = evaluate(aText[i], anError, anAttribute->owner()->document());
      bool isValid = anError.empty();
      if (isValid) aCalculatedValue[i] = aValue;
      anAttribute->setUsedParameters(i, isValid ? toSet(myInterp->compile(aText[i])) : std::set<std::string>());
      anAttribute->setExpressionInvalid(i, !isValid);
      anAttribute->setExpressionError(i, aText[i].empty() ? "" : anError);
    }
    anAttribute->setCalculatedValue(aCalculatedValue[0],
                                    aCalculatedValue[1]);
  }
}

std::string ParametersPlugin_EvalListener::renameInPythonExpression(
    const std::string& theExpression,
    const std::string& theOldName,
    const std::string& theNewName)
{
  std::string anExpressionString = theExpression;

  std::list<std::pair<int, int> > aPositions =
      myInterp->positions(anExpressionString, theOldName);

  if (aPositions.empty())
    return anExpressionString;

  std::map<int, std::list<int> > aLines;
  std::list<std::pair<int, int> >::const_iterator it = aPositions.begin();
  for (; it != aPositions.end(); ++it)
    aLines[it->first].push_back(it->second);

  // Start renaming from the end to keep indexes if theNewName is longer then theOldName
  std::map<int, std::list<int> >::const_reverse_iterator ritLine = aLines.rbegin();
  for (; ritLine != aLines.rend(); ++ritLine) {
    // Calculate the start of the line (find the aLineNo occurrence of "\n" )
    int aLineNo = ritLine->first - 1;
    size_t aLineStart = 0;
    for (int i = 0; i < aLineNo; ++i)
      aLineStart = anExpressionString.find("\n", aLineStart) + 1;

    const std::list<int>& aColOffsets = ritLine->second;
    std::list<int>::const_reverse_iterator ritOffset = aColOffsets.rbegin();
    for (; ritOffset != aColOffsets.rend(); ++ritOffset) {
      int anOffset = *ritOffset;
      anExpressionString.replace(aLineStart + anOffset, theOldName.size(), theNewName);
    }
  }

  return anExpressionString;
}

void ParametersPlugin_EvalListener::renameInParameter(
    std::shared_ptr<ParametersPlugin_Parameter> theParameter,
    const std::string& theOldName,
    const std::string& theNewName)
{
  std::shared_ptr<ModelAPI_AttributeString> anExpressionAttribute =
      theParameter->string(ParametersPlugin_Parameter::EXPRESSION_ID());

  std::string anExpressionString = anExpressionAttribute->value();
  anExpressionString = renameInPythonExpression(anExpressionString,
                                                theOldName,
                                                theNewName);
  // Issue #588. No need for reevaluating expression. 
  // Moreover, current history may not contain necessary parameters.
  anExpressionAttribute->owner()->data()->blockSendAttributeUpdated(true);
  anExpressionAttribute->setValue(anExpressionString);
  anExpressionAttribute->owner()->data()->blockSendAttributeUpdated(false);
}

void ParametersPlugin_EvalListener::renameInAttribute(
    std::shared_ptr<ModelAPI_Attribute> theAttribute,
    const std::string& theOldName,
    const std::string& theNewName)
{
  if (theAttribute->attributeType() == ModelAPI_AttributeDouble::typeId()) {
    AttributeDoublePtr anAttribute =
        std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theAttribute);
    std::string anExpressionString = anAttribute->text();
    anExpressionString = renameInPythonExpression(anExpressionString,
                                                  theOldName, theNewName);
    anAttribute->setText(anExpressionString);
  } else
  if (theAttribute->attributeType() == GeomDataAPI_Point::typeId()) {
    AttributePointPtr anAttribute =
        std::dynamic_pointer_cast<GeomDataAPI_Point>(theAttribute);
    std::string anExpressionString[3] = {
      anAttribute->textX(),
      anAttribute->textY(),
      anAttribute->textZ()
    };
    for (int i = 0; i < 3; ++i)
      anExpressionString[i] = renameInPythonExpression(anExpressionString[i],
                                                       theOldName, theNewName);
    anAttribute->setText(anExpressionString[0],
                         anExpressionString[1],
                         anExpressionString[2]);
  } else
  if (theAttribute->attributeType() == GeomDataAPI_Point2D::typeId()) {
    AttributePoint2DPtr anAttribute =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theAttribute);
    std::string anExpressionString[2] = {
      anAttribute->textX(),
      anAttribute->textY()
    };
    for (int i = 0; i < 2; ++i)
      anExpressionString[i] = renameInPythonExpression(anExpressionString[i],
                                                       theOldName, theNewName);
    anAttribute->setText(anExpressionString[0],
                         anExpressionString[1]);
  }
}

bool isValidAttribute(const AttributePtr& theAttribute)
{
  std::string aValidator, anError;
  return ModelAPI_Session::get()->validators()->validate(theAttribute, aValidator, anError);
}

void setParameterName(ResultParameterPtr theResultParameter, const std::string& theName)
{
  theResultParameter->data()->blockSendAttributeUpdated(true);
  theResultParameter->data()->setName(theName);
  theResultParameter->data()->blockSendAttributeUpdated(false);

  std::shared_ptr<ParametersPlugin_Parameter> aParameter = 
      std::dynamic_pointer_cast<ParametersPlugin_Parameter>(
          ModelAPI_Feature::feature(theResultParameter));

  aParameter->data()->blockSendAttributeUpdated(true);
  aParameter->data()->setName(theName);
  aParameter->string(ParametersPlugin_Parameter::VARIABLE_ID())->setValue(theName);
  aParameter->data()->blockSendAttributeUpdated(false);
}

void ParametersPlugin_EvalListener::processObjectRenamedEvent(
    const std::shared_ptr<Events_Message>& theMessage)
{
  std::shared_ptr<ModelAPI_ObjectRenamedMessage> aMessage =
      std::dynamic_pointer_cast<ModelAPI_ObjectRenamedMessage>(theMessage);

  // Empty new name is not available too but it will be rejected by
  // name validator in isValidAttribute.
  if (!aMessage.get() || aMessage->oldName().empty())
    return;

  // check if the renamed object is a result parameter
  ResultParameterPtr aResultParameter =
      std::dynamic_pointer_cast<ModelAPI_ResultParameter>(aMessage->object());
  if (!aResultParameter.get()) 
    return;

  // get parameter feature for the result
  std::shared_ptr<ParametersPlugin_Parameter> aParameter =
      std::dynamic_pointer_cast<ParametersPlugin_Parameter>(
          ModelAPI_Feature::feature(aResultParameter));
  if (!aParameter.get())
    return;

  // try to update the parameter feature according the new name
  setParameterName(aResultParameter, aMessage->newName());
  // TODO(spo): replace with ModelAPI_Session::get()->validators()->validate(aParameter, ParametersPlugin_Parameter::VARIABLE_ID())
  // when ModelAPI_ValidatorsFactory::validate(const std::shared_ptr<ModelAPI_Feature>& theFeature, const std::string& theAttribute) const
  // is ready
  if (!isValidAttribute(aParameter->string(ParametersPlugin_Parameter::VARIABLE_ID()))) {
    setParameterName(aResultParameter, aMessage->oldName());
    return;
  }

  std::set<std::shared_ptr<ModelAPI_Attribute> > anAttributes = 
      aResultParameter->data()->refsToMe();
  std::set<std::shared_ptr<ModelAPI_Attribute> >::const_iterator anAttributeIt =
      anAttributes.cbegin();
  for (; anAttributeIt != anAttributes.cend(); ++anAttributeIt) {
    const AttributePtr& anAttribute = *anAttributeIt;
    AttributeRefListPtr anAttributeRefList =
        std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(anAttribute);
    if (anAttributeRefList.get()) {
      std::shared_ptr<ParametersPlugin_Parameter> aParameter =
          std::dynamic_pointer_cast<ParametersPlugin_Parameter>(
              anAttributeRefList->owner());
      if (aParameter.get())
        // Rename
        renameInParameter(aParameter, aMessage->oldName(), aMessage->newName());
    } else
        // Rename
        renameInAttribute(anAttribute, aMessage->oldName(), aMessage->newName());
  }
}

