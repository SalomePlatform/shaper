// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * Config_ValidatorMessage.cpp
 *
 *  Created on: 08 Jul 2014 �.
 *      Author: sbh
 */

#include <Config_ValidatorMessage.h>

Config_ValidatorMessage::Config_ValidatorMessage(const Events_ID theId, const void* theParent)
    : Events_Message(theId, theParent)
{
  myValidatorId = "";
  myFeatureId = "";
  myAttributeId = "";
}

Config_ValidatorMessage::~Config_ValidatorMessage()
{
}

const std::string& Config_ValidatorMessage::validatorId() const
{
  return myValidatorId;
}

const std::string& Config_ValidatorMessage::featureId() const
{
  return myFeatureId;
}

const std::string& Config_ValidatorMessage::attributeId() const
{
  return myAttributeId;
}

const std::list<std::string>& Config_ValidatorMessage::parameters() const
{
  return myVaidatorParameters;
}

bool Config_ValidatorMessage::isValid() const
{
  return !myValidatorId.empty();
}

void Config_ValidatorMessage::setValidatorId(const std::string& validatorId)
{
  myValidatorId = validatorId;
}

void Config_ValidatorMessage::setValidatorParameters(const std::list<std::string>& parameters)
{
  myVaidatorParameters = parameters;
}

void Config_ValidatorMessage::setFeatureId(const std::string& theId)
{
  myFeatureId = theId;
}

void Config_ValidatorMessage::setAttributeId(const std::string& theId)
{
  myAttributeId = theId;
}
