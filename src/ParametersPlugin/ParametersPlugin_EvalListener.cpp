/*
 * ParametersPlugin_EvalListener.cpp
 *
 *  Created on: Apr 28, 2015
 *      Author: sbh
 */

#include <ParametersPlugin_EvalListener.h>

#include <Events_Error.h>

#include <ModelAPI_Events.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_AttributeDouble.h>

#include <string>
#include <sstream>

ParametersPlugin_EvalListener::ParametersPlugin_EvalListener()
{
  Events_Loop* aLoop = Events_Loop::loop();
  const Events_ID kEvaluationEvent = ModelAPI_AttributeEvalMessage::eventId();
  aLoop->registerListener(this, kEvaluationEvent, NULL, true);

  myInterp = std::shared_ptr<ParametersPlugin_PyInterp>(new ParametersPlugin_PyInterp());
  myInterp->initialize();
}

ParametersPlugin_EvalListener::~ParametersPlugin_EvalListener()
{
}

void ParametersPlugin_EvalListener::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  if (!theMessage.get())
    return;

  const Events_ID kEvaluationEvent = ModelAPI_AttributeEvalMessage::eventId();
  if (theMessage->eventID() == kEvaluationEvent) {
    std::shared_ptr<ModelAPI_AttributeEvalMessage> aMessage =
        std::dynamic_pointer_cast<ModelAPI_AttributeEvalMessage>(theMessage);
    AttributeDoublePtr aDoubleAttribute =
        std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aMessage->attribute());
    if (aDoubleAttribute.get()) {
      std::string anError;
      double aValue = evaluate(aDoubleAttribute->text(), anError);
      if (anError.empty()) {
        aDoubleAttribute->setValue(aValue);
        aDoubleAttribute->setExpressionInvalid(false);
      } else { // set feature as invalid-parameter arguments
        aDoubleAttribute->setExpressionInvalid(true);
      }
    }
  } else {
    Events_Error::send(std::string("ParametersPlugin python interpreter, unhandled message caught: ")
                       + theMessage->eventID().eventText());
  }
}

double ParametersPlugin_EvalListener::evaluate(const std::string& theExpression,
                                               std::string& theError)
{
  std::list<std::string> anExprParams = myInterp->compile(theExpression);
  // find expression's params in the model
  std::list<std::string> aContext;
  std::list<std::string>::iterator it = anExprParams.begin();
  for ( ; it != anExprParams.end(); it++) {
    double aValue;
    if (!ModelAPI_Tools::findVariable(*it, aValue)) continue;

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

