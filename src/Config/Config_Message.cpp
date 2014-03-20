/*
 *
 */

#include "Config_Message.h"

Config_FeatureMessage::Config_FeatureMessage(const Event_ID theId, const void* theParent) :
  Event_Message(theId, theParent)
{
  m_group = "";
  m_id = "";
  m_text = "";
  m_tooltip = "";
  m_icon = "";
  m_keysequence = "";
}

