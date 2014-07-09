/*
 * Config_ValidatorMessage.h
 *
 *  Created on: 08 июля 2014 г.
 *      Author: sbh
 */

#ifndef Config_ValidatorMessage_H_
#define Config_ValidatorMessage_H_

#include <Events_Message.h>
#include <Config_def.h>

#include <list>
#include <string>

/// Event ID that feature is loaded (comes with Config_FeatureMessage)
static const char * EVENT_VALIDATOR_LOADED = "ValidatorLoaded";

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
  const std::string& featureId() const;
  const std::string& attributeId() const;
  CONFIG_EXPORT const std::list<std::string>& parameters() const;
  CONFIG_EXPORT bool isValid() const;

  CONFIG_EXPORT void setValidatorId(const std::string& theId);
  CONFIG_EXPORT void setFeatureId(const std::string& theId);
  CONFIG_EXPORT void setAttributeId(const std::string& theId);
  CONFIG_EXPORT void setValidatorParameters(const std::list<std::string>& parameters);
};

#endif /* Config_ValidatorMessage_H_ */
