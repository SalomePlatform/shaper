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

#include <pyconfig.h>

#include <ParametersPlugin_EvalListener.h>
#include <ParametersPlugin_Parameter.h>

#include <Events_InfoMessage.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeValidator.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>

#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>

#include <QMessageBox>

#include <ModuleBase_Tools.h>

#include <string>
#include <set>
#include <sstream>

//------------------------------------------------------------------------------
// Tools

static std::string toStdString(double theValue)
{
  std::ostringstream sstream;
  sstream << theValue;
  size_t aPos = sstream.str().find(".");
  std::string aPnt = "";
  if (aPos == std::string::npos)
    aPnt = ".";
  return sstream.str() + aPnt;
}

//------------------------------------------------------------------------------

ParametersPlugin_EvalListener::ParametersPlugin_EvalListener()
{
  Events_Loop* aLoop = Events_Loop::loop();

  Events_ID anEvents_IDs[] = {
      ModelAPI_ObjectRenamedMessage::eventId(),
      ModelAPI_ReplaceParameterMessage::eventId()
  };

  for (unsigned long long i = 0; i < sizeof(anEvents_IDs)/sizeof(anEvents_IDs[0]); ++i)
    aLoop->registerListener(this, anEvents_IDs[i], NULL, true);
}

ParametersPlugin_EvalListener::~ParametersPlugin_EvalListener()
{
}

void ParametersPlugin_EvalListener::processEvent(
    const std::shared_ptr<Events_Message>& theMessage)
{
  if (!theMessage.get())
    return;

  const Events_ID kObjectRenamedEvent = ModelAPI_ObjectRenamedMessage::eventId();
  const Events_ID kReplaceParameterEvent = ModelAPI_ReplaceParameterMessage::eventId();

  if (theMessage->eventID() == kObjectRenamedEvent) {
    processObjectRenamedEvent(theMessage);
  } else if (theMessage->eventID() == kReplaceParameterEvent) {
    processReplaceParameterEvent(theMessage);
  }
}

std::string ParametersPlugin_EvalListener::renameInPythonExpression(
    const std::string& theExpression,
    const std::string& theOldName,
    const std::string& theNewName)
{
  std::string anExpressionString = theExpression;

  // ask interpreter to compute the positions in the expression
  std::shared_ptr<ModelAPI_ComputePositionsMessage> aMsg =
    ModelAPI_ComputePositionsMessage::send(theExpression, theOldName, this);
  const std::list<std::pair<int, int> >& aPositions = aMsg->positions();

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
  bool aWasBlocked = anExpressionAttribute->owner()->data()->blockSendAttributeUpdated(true);
  anExpressionAttribute->setValue(anExpressionString);
  anExpressionAttribute->owner()->data()->blockSendAttributeUpdated(aWasBlocked);
}

void ParametersPlugin_EvalListener::renameInAttribute(
    std::shared_ptr<ModelAPI_Attribute> theAttribute,
    const std::string& theOldName,
    const std::string& theNewName)
{
  if (theAttribute->attributeType() == ModelAPI_AttributeInteger::typeId()) {
    AttributeIntegerPtr anAttribute =
        std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(theAttribute);
    std::string anExpressionString = anAttribute->text();
    anExpressionString = renameInPythonExpression(anExpressionString,
                                                  theOldName, theNewName);
    anAttribute->setText(anExpressionString);
  } else
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

void ParametersPlugin_EvalListener::renameInDependents(
              std::shared_ptr<ModelAPI_ResultParameter> theResultParameter,
              const std::string& theOldName,
              const std::string& theNewName)
{
  std::set<std::shared_ptr<ModelAPI_Attribute> > anAttributes =
    theResultParameter->data()->refsToMe();
  std::set<std::shared_ptr<ModelAPI_Attribute> >::const_iterator anAttributeIt =
    anAttributes.cbegin();
  for (; anAttributeIt != anAttributes.cend(); ++anAttributeIt) {
    const AttributePtr& anAttribute = *anAttributeIt;
    if (anAttribute->attributeType() == ModelAPI_AttributeRefList::typeId()) {
      std::shared_ptr<ParametersPlugin_Parameter> aParameter =
        std::dynamic_pointer_cast<ParametersPlugin_Parameter>(
        anAttribute->owner());
      if (aParameter.get())
        // Rename
        renameInParameter(aParameter, theOldName, theNewName);
    } else
      // Rename
      renameInAttribute(anAttribute, theOldName, theNewName);
  }
}

bool isValidAttribute(const AttributePtr& theAttribute)
{
  std::string aValidator;
  Events_InfoMessage anError;
  return ModelAPI_Session::get()->validators()->validate(theAttribute, aValidator, anError);
}

void setParameterName(ResultParameterPtr theResultParameter, const std::string& theName)
{
  bool aWasBlocked = theResultParameter->data()->blockSendAttributeUpdated(true);
  theResultParameter->data()->setName(theName);
  theResultParameter->data()->blockSendAttributeUpdated(aWasBlocked, false);

  std::shared_ptr<ParametersPlugin_Parameter> aParameter =
      std::dynamic_pointer_cast<ParametersPlugin_Parameter>(
          ModelAPI_Feature::feature(theResultParameter));

  std::string anOldName = aParameter->name();
  aWasBlocked = aParameter->data()->blockSendAttributeUpdated(true);
  aParameter->data()->setName(theName);
  aParameter->string(ParametersPlugin_Parameter::VARIABLE_ID())->setValue(theName);
  aParameter->data()->blockSendAttributeUpdated(aWasBlocked);
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

  std::string aNotActivatedNames;
  if (!ModelAPI_Tools::allDocumentsActivated(aNotActivatedNames)) {
    static const std::string aMsgContext("ParametersPlugin");
    static const std::string aMsgText =
      "Selected objects can be used in Part documents which are not loaded: " +
      std::string("%1. Would you like to continue?");
    Events_InfoMessage aMsg(aMsgContext, aMsgText);
    aMsg.arg(aNotActivatedNames.c_str());
    QMessageBox::StandardButton aRes =
      QMessageBox::warning(0, ModuleBase_Tools::translate(aMsgContext, "Warning"),
        ModuleBase_Tools::translate(aMsg),
        QMessageBox::No | QMessageBox::Yes, QMessageBox::No);
    if (aRes != QMessageBox::Yes) {
      setParameterName(aResultParameter, aMessage->oldName());
      return;
    }
  }

  // try to update the parameter feature according the new name
  setParameterName(aResultParameter, aMessage->newName());
  if (!isValidAttribute(aParameter->string(ParametersPlugin_Parameter::VARIABLE_ID()))) {
    //setParameterName(aResultParameter, aMessage->oldName());
    if (myOldNames.find(aParameter.get()) == myOldNames.end())
      myOldNames[aParameter.get()] = aMessage->oldName();
    return;
  }

  std::string anOldName = aMessage->oldName();
  if (myOldNames.find(aParameter.get()) != myOldNames.end()) {
    anOldName = myOldNames[aParameter.get()];
    myOldNames.erase(aParameter.get());
    aParameter->execute(); // to enable result because of previously incorrect name
  }

  renameInDependents(aResultParameter, anOldName, aMessage->newName());
}

void ParametersPlugin_EvalListener::processReplaceParameterEvent(
    const std::shared_ptr<Events_Message>& theMessage)
{
  std::shared_ptr<ModelAPI_ReplaceParameterMessage> aMessage =
      std::dynamic_pointer_cast<ModelAPI_ReplaceParameterMessage>(theMessage);

  // get parameter feature for the object
  std::shared_ptr<ParametersPlugin_Parameter> aParameter =
      std::dynamic_pointer_cast<ParametersPlugin_Parameter>(
          ModelAPI_Feature::feature(aMessage->object()));
  if (!aParameter.get())
    return;

  ResultParameterPtr aResultParameter =
      std::dynamic_pointer_cast<ModelAPI_ResultParameter>(
          aParameter->firstResult());
  if (!aResultParameter.get())
    return;

  double aRealValue = aResultParameter->data()->real(ModelAPI_ResultParameter::VALUE())->value();
  std::string aValue = toStdString(aRealValue);

  renameInDependents(aResultParameter, aResultParameter->data()->name(), aValue);
}
