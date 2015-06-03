/*
 * ParametersPlugin_EvalListener.cpp
 *
 *  Created on: Apr 28, 2015
 *      Author: sbh
 */

#include <pyconfig.h>

#include <ParametersPlugin_EvalListener.h>
#include <ParametersPlugin_PyInterp.h>

#include <Events_Error.h>

#include <ModelAPI_Events.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_AttributeDouble.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>

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

    // Double
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

    // Point
    AttributePointPtr aPointAttribute =
        std::dynamic_pointer_cast<GeomDataAPI_Point>(aMessage->attribute());
    if (aPointAttribute.get()) {
      std::string anError[3];
      double aValue[3] = {
          evaluate(aPointAttribute->textX(), anError[0]),
          evaluate(aPointAttribute->textY(), anError[1]),
          evaluate(aPointAttribute->textZ(), anError[2])
      };
      bool isValid[3] = {
          anError[0].empty(),
          anError[1].empty(),
          anError[2].empty()
      };
      aPointAttribute->setExpressionInvalid(0, !isValid[0]);
      aPointAttribute->setExpressionInvalid(1, !isValid[1]);
      aPointAttribute->setExpressionInvalid(2, !isValid[2]);

      aPointAttribute->setValue(
          isValid[0] ? aValue[0] : aPointAttribute->x(),
          isValid[1] ? aValue[1] : aPointAttribute->y(),
          isValid[2] ? aValue[2] : aPointAttribute->z()
      );
    }

    // Point2D
    AttributePoint2DPtr aPoint2DAttribute =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aMessage->attribute());
    if (aPoint2DAttribute.get()) {
      std::string anError[2];
      double aValue[2] = {
          evaluate(aPoint2DAttribute->textX(), anError[0]),
          evaluate(aPoint2DAttribute->textY(), anError[1])
      };
      bool isValid[2] = {
          anError[0].empty(),
          anError[1].empty()
      };
      aPoint2DAttribute->setExpressionInvalid(0, !isValid[0]);
      aPoint2DAttribute->setExpressionInvalid(1, !isValid[1]);

      aPoint2DAttribute->setValue(
          isValid[0] ? aValue[0] : aPoint2DAttribute->x(),
          isValid[1] ? aValue[1] : aPoint2DAttribute->y()
      );
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
    ResultParameterPtr aParamRes;
    if (!ModelAPI_Tools::findVariable(*it, aValue, aParamRes)) continue;

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
