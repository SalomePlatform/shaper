#ifndef CONFIG_MESSAGE_H
#define CONFIG_MESSAGE_H

#include "Config.h"

#include <Event_Message.hxx>
#include <string>

class CONFIG_EXPORT Config_FeatureMessage : public Event_Message
{
  std::string m_id;
  std::string m_text;
  std::string m_tooltip;
  std::string m_icon;
  std::string m_keysequence;

  std::string m_groupId;
  std::string m_groupText;
  std::string m_workbenchId;
  std::string m_workbenchText;

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
  const std::string& groupText() const;
  const std::string& workbenchId() const;
  const std::string& workbenchText() const;

  void setIcon(const std::string& icon);
  void setId(const std::string& id);
  void setKeysequence(const std::string& keysequence);
  void setText(const std::string& text);
  void setTooltip(const std::string& tooltip);
  void setGroupId(const std::string& groupId);
  void setGroupText(const std::string& groupText);
  void setWorkbenchId(const std::string& workbenchId);
  void setWorkbenchText(const std::string& workbenchText);
};

#endif // CONFIG_MESSAGE_H
