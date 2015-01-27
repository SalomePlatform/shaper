// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef ATTRIBUTE_MESSAGE_H
#define ATTRIBUTE_MESSAGE_H

#include <Config_def.h>
#include <Config_FeatureMessage.h>
#include <Events_Message.h>

#include <string>

/*!
 * \class Config_AttributeMessage
 * \brief Class to pass an attribute's (widget's) entry info extracted from xml file.
 * \code
 * <widget id="tool_object" concealment="true" obligatory="0"/>
 * \endcode
 */
class Config_AttributeMessage : public Events_Message
{
  std::string myAttributeId;  //Feature unique id
  std::string myFeatureId;  //Feature unique id
  bool myIsObligatory;
  bool myIsConcealment;

 public:
  // Same event as Config_FeatureMessage::MODEL_EVENT()
  inline static const char* MODEL_EVENT()
  {
    return Config_FeatureMessage::MODEL_EVENT();
  }

  //const Events_ID theID, const void* theSender = 0
  CONFIG_EXPORT Config_AttributeMessage(const Events_ID theId, const void* theParent = 0);
  CONFIG_EXPORT virtual ~Config_AttributeMessage();

  //Auto-generated getters/setters
  CONFIG_EXPORT const std::string& attributeId() const;
  CONFIG_EXPORT const std::string& featureId() const;
  CONFIG_EXPORT bool isObligatory() const;
  CONFIG_EXPORT bool isConcealment() const;

  CONFIG_EXPORT void setAttributeId(const std::string& theId);
  CONFIG_EXPORT void setFeatureId(const std::string& id);
  CONFIG_EXPORT void setConcealment(bool isConcealment);
  CONFIG_EXPORT void setObligatory(bool isObligatory);
};

#endif // ATTRIBUTE_MESSAGE_H
