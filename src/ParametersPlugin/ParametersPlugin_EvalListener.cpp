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

#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Session.h>
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

void ParametersPlugin_EvalListener::processEvaluationEvent(
    const std::shared_ptr<Events_Message>& theMessage)
{
  std::shared_ptr<ModelAPI_AttributeEvalMessage> aMessage =
      std::dynamic_pointer_cast<ModelAPI_AttributeEvalMessage>(theMessage);

  // Double
  AttributeDoublePtr aDoubleAttribute =
      std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aMessage->attribute());
  if (aDoubleAttribute.get()) {
    std::string anError;
    double aValue = evaluate(aDoubleAttribute->text(), anError);
    if (anError.empty()) {
      aDoubleAttribute->setCalculatedValue(aValue);
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

    aPointAttribute->setCalculatedValue(
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

    aPoint2DAttribute->setCalculatedValue(
        isValid[0] ? aValue[0] : aPoint2DAttribute->x(),
        isValid[1] ? aValue[1] : aPoint2DAttribute->y()
    );
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
  // Double
  AttributeDoublePtr aDoubleAttribute =
      std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theAttribute);
  if (aDoubleAttribute.get()) {
    std::string anExpressionString = aDoubleAttribute->text();
    anExpressionString = renameInPythonExpression(anExpressionString,
                                                  theOldName,
                                                  theNewName);
    aDoubleAttribute->setText(anExpressionString);
  }

  // Point
  AttributePointPtr aPointAttribute =
      std::dynamic_pointer_cast<GeomDataAPI_Point>(theAttribute);
  if (aPointAttribute.get()) {
    std::string anExpressionString[3] = {
      aPointAttribute->textX(),
      aPointAttribute->textY(),
      aPointAttribute->textZ()
    };
    for (int i = 0; i < 3; ++i)
      anExpressionString[i] = renameInPythonExpression(anExpressionString[i],
                                                       theOldName,
                                                       theNewName);
    aPointAttribute->setText(anExpressionString[0],
                             anExpressionString[1],
                             anExpressionString[2]);
  }

  // Point2D
  AttributePoint2DPtr aPoint2DAttribute =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theAttribute);
  if (aPoint2DAttribute.get()) {
    std::string anExpressionString[2] = {
      aPoint2DAttribute->textX(),
      aPoint2DAttribute->textY()
    };
    for (int i = 0; i < 2; ++i)
      anExpressionString[i] = renameInPythonExpression(anExpressionString[i],
                                                       theOldName,
                                                       theNewName);
    aPoint2DAttribute->setText(anExpressionString[0],
                               anExpressionString[1]);
  }
}

void ParametersPlugin_EvalListener::processObjectRenamedEvent(
    const std::shared_ptr<Events_Message>& theMessage)
{
  std::shared_ptr<ModelAPI_ObjectRenamedMessage> aMessage =
      std::dynamic_pointer_cast<ModelAPI_ObjectRenamedMessage>(theMessage);

  if (!aMessage.get() || aMessage->oldName().empty() || aMessage->newName().empty())
    return;

  // check that the renamed object is a result 
  std::shared_ptr<ModelAPI_ResultParameter> aResultParameter =
      std::dynamic_pointer_cast<ModelAPI_ResultParameter>(aMessage->object());
  if (!aResultParameter.get()) 
    return;
  
  // get parameter feature for the result
  std::shared_ptr<ModelAPI_Feature> aFeature = aResultParameter->document()->feature(aResultParameter);
  std::shared_ptr<ParametersPlugin_Parameter> aParameter =
      std::dynamic_pointer_cast<ParametersPlugin_Parameter>(aFeature);
  if (!aParameter.get())
    return;

  // rename a parameter attributes
  // short way:
  //aParameter->string(ParametersPlugin_Parameter::VARIABLE_ID())->setValue(aMessage->newName());
  //aParameter->execute();
  // manual way:
  aParameter->data()->setName(aMessage->newName());
  aParameter->string(ParametersPlugin_Parameter::VARIABLE_ID())->setValue(aMessage->newName());

  // List of documents to process
  std::list<DocumentPtr> aDocList;
  SessionPtr aSession = ModelAPI_Session::get();
  DocumentPtr aDocument = aSession->activeDocument();
  DocumentPtr aRootDocument = aSession->moduleDocument();
  aDocList.push_back(aDocument);
  if (aDocument != aRootDocument) {
    aDocList.push_back(aRootDocument);
  }

  // Find all features
  for (std::list<DocumentPtr>::const_iterator aDicumentIt = aDocList.begin(); 
       aDicumentIt != aDocList.end(); ++aDicumentIt) {
    const DocumentPtr& aDocument = *aDicumentIt;
    std::list<FeaturePtr> aFeatures = aDocument->allFeatures();
    std::list<FeaturePtr>::iterator aFeatureIt = aFeatures.begin();
    for (; aFeatureIt != aFeatures.end(); ++aFeatureIt) {
      const FeaturePtr& aFeature = *aFeatureIt;
      
      // If Parameter feature then rename its expression
      std::shared_ptr<ParametersPlugin_Parameter> aParameter =
          std::dynamic_pointer_cast<ParametersPlugin_Parameter>(aFeature);
      if (aParameter.get()) {
        // Rename
        renameInParameter(aParameter, aMessage->oldName(), aMessage->newName());
        continue;
      }      

      // Find all attributes
      std::list<AttributePtr> anAttributes = aFeature->data()->attributes(std::string());
      std::list<AttributePtr>::const_iterator anAttributeIt = anAttributes.begin();
      for (; anAttributeIt != anAttributes.end(); ++anAttributeIt) {
        const AttributePtr& anAttribute = *anAttributeIt;

        // Rename
        renameInAttribute(anAttribute, aMessage->oldName(), aMessage->newName());
      }
    }
  }
}

