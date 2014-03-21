/*
 *
 */
#include "Config_Message.h"

Config_FeatureMessage::Config_FeatureMessage(const Event_ID theId,
                                             const void* theParent)
    : Event_Message(theId, theParent)
{
  m_id = "";
  m_text = "";
  m_tooltip = "";
  m_icon = "";
  m_keysequence = "";

  m_groupId = "";
  m_groupText = "";
  m_workbenchId = "";
  m_workbenchText = "";
}

const std::string& Config_FeatureMessage::icon() const
{
  return m_icon;
}

void Config_FeatureMessage::setIcon(const std::string& icon)
{
  m_icon = icon;
}

const std::string& Config_FeatureMessage::id() const
{
  return m_id;
}

void Config_FeatureMessage::setId(const std::string& id)
{
  m_id = id;
}

const std::string& Config_FeatureMessage::keysequence() const
{
  return m_keysequence;
}

void Config_FeatureMessage::setKeysequence(const std::string& keysequence)
{
  m_keysequence = keysequence;
}

const std::string& Config_FeatureMessage::text() const
{
  return m_text;
}

void Config_FeatureMessage::setText(const std::string& text)
{
  m_text = text;
}

const std::string& Config_FeatureMessage::tooltip() const
{
  return m_tooltip;
}

const std::string& Config_FeatureMessage::groupId() const
{
  return m_groupId;
}

void Config_FeatureMessage::setGroupId(const std::string& groupId)
{
  m_groupId = groupId;
}

const std::string& Config_FeatureMessage::groupText() const
{
  return m_groupText;
}

void Config_FeatureMessage::setGroupText(const std::string& groupText)
{
  m_groupText = groupText;
}

const std::string& Config_FeatureMessage::workbenchId() const
{
  return m_workbenchId;
}

void Config_FeatureMessage::setWorkbenchId(const std::string& workbenchId)
{
  m_workbenchId = workbenchId;
}

const std::string& Config_FeatureMessage::workbenchText() const
{
  return m_workbenchText;
}

void Config_FeatureMessage::setWorkbenchText(const std::string& workbenchText)
{
  m_workbenchText = workbenchText;
}

void Config_FeatureMessage::setTooltip(const std::string& tooltip)
{
  m_tooltip = tooltip;
}
