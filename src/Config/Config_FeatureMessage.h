#ifndef CONFIG_MESSAGE_H
#define CONFIG_MESSAGE_H

#include "Config.h"

#include <Event_Message.h>
#include <string>

class CONFIG_EXPORT Config_FeatureMessage: public Event_Message
{

  std::string myId;
  std::string myText;
  std::string myTooltip;
  std::string myIcon;
  std::string myKeysequence;

  std::string myGroupId;
  std::string myWorkbenchId;

public:
  //const Event_ID theID, const void* theSender = 0
  Config_FeatureMessage(const Event_ID theId, const void* theParent = 0);

  //Auto-generated getters/setters
  const std::string& icon() const;
  const std::string& id() const;
  const std::string& keysequence() const;
  const std::string& text() const;
  const std::string& tooltip() const;

  const std::string& groupId() const;
  const std::string& workbenchId() const;

  void setIcon(const std::string& icon);
  void setId(const std::string& id);
  void setKeysequence(const std::string& keysequence);
  void setText(const std::string& text);
  void setTooltip(const std::string& tooltip);

  void setGroupId(const std::string& groupId);
  void setWorkbenchId(const std::string& workbenchId);
};

#endif // CONFIG_MESSAGE_H