#ifndef CONFIG_MESSAGE_H
#define CONFIG_MESSAGE_H

#include "Config.h"

#include <Event_Message.hxx>
#include <string>

class CONFIG_EXPORT Config_FeatureMessage : public Event_Message
{
public:
  std::string m_id;
  std::string m_text;
  std::string m_tooltip;
  std::string m_icon;
  std::string m_keysequence;

  std::string m_group;

public:
  //const Event_ID theID, const void* theSender = 0
  Config_FeatureMessage(const Event_ID theId, const void* theParent = 0);

};


#endif // CONFIG_MESSAGE_H
