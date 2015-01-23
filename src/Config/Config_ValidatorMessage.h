// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * Config_ValidatorMessage.h
 *
 *  Created on: 08 Jul 2014 �.
 *      Author: sbh
 */

#ifndef CONFIG_VALIDATORMESSAGE_H_
#define CONFIG_VALIDATORMESSAGE_H_

#include <Events_Message.h>
#include <Config_def.h>

#include <list>
#include <string>

/// Event ID that Validator is loaded (comes with Config_ValidatorMessage)
static const char * EVENT_VALIDATOR_LOADED = "ValidatorLoaded";

/*!
 * \class Config_ValidatorMessage
 * \brief Transmits info about created validator on "ValidatorLoaded" event
 */
class Config_ValidatorMessage : public Events_Message
{
  std::string myValidatorId;
  std::string myFeatureId;
  std::string myAttributeId;
  std::list<std::string> myVaidatorParameters;

 public:
  CONFIG_EXPORT Config_ValidatorMessage(const Events_ID theId, const void* theParent = 0);
  CONFIG_EXPORT virtual ~Config_ValidatorMessage();

  //CONFIG_EXPORT static const char* UID() const;

  CONFIG_EXPORT const std::string& validatorId() const;
  CONFIG_EXPORT const std::string& featureId() const;
  CONFIG_EXPORT const std::string& attributeId() const;
  CONFIG_EXPORT const std::list<std::string>& parameters() const;
  CONFIG_EXPORT bool isValid() const;

  CONFIG_EXPORT void setValidatorId(const std::string& theId);
  CONFIG_EXPORT void setFeatureId(const std::string& theId);
  CONFIG_EXPORT void setAttributeId(const std::string& theId);
  CONFIG_EXPORT void setValidatorParameters(const std::list<std::string>& parameters);
};

#endif /* Config_ValidatorMessage_H_ */
