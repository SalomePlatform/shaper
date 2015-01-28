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
 * \ingroup Config
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

  //! Get id of the filter
  CONFIG_EXPORT const std::string& validatorId() const;
  //! Get id of a feature to which the filter belongs to
  CONFIG_EXPORT const std::string& featureId() const;
  //! Get id of an attribute to which the filter belongs to
  CONFIG_EXPORT const std::string& attributeId() const;
  //! Get filter parameters
  CONFIG_EXPORT const std::list<std::string>& parameters() const;
  //! Returns true if validator id is not empty
  CONFIG_EXPORT bool isValid() const;

  //! Set id of the filter
  CONFIG_EXPORT void setValidatorId(const std::string& theId);
  //! Set id of a feature to which the filter belongs to
  CONFIG_EXPORT void setFeatureId(const std::string& theId);
  //! Set id of an attribute to which the filter belongs to
  CONFIG_EXPORT void setAttributeId(const std::string& theId);
  //! Get filter parameters
  CONFIG_EXPORT void setValidatorParameters(const std::list<std::string>& parameters);
};

#endif /* Config_ValidatorMessage_H_ */
