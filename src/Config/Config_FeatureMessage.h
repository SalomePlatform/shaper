#ifndef CONFIG_MESSAGE_H
#define CONFIG_MESSAGE_H

#include "Config.h"

#include <Event_Message.hxx>
#include <string>

/*
 * Class to pass a feature entry extracted from xml file.
 * Example of the feature entry:
 * <feature id="new_part" text="Part" tooltip="Creates a new part" icon=":pictures/part_ico.png"/>
 */
class CONFIG_EXPORT Config_FeatureMessage: public Event_Message
{
  std::string myId;  //Feature unique id
  std::string myText; //Represents action's text
  std::string myTooltip;  //Represents action's tooltip
  std::string myIcon; //Represents action's icon
  std::string myKeysequence;  //Represents action's key sequence

  std::string myGroupId;  //Id of feature's group
  std::string myWorkbenchId;  //Id of feature's workbench
  std::string myPluginLibrary;  //Name of feature's library

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
  const std::string& pluginLibrary() const;

  void setIcon(const std::string& icon);
  void setId(const std::string& id);
  void setKeysequence(const std::string& keysequence);
  void setText(const std::string& text);
  void setTooltip(const std::string& tooltip);

  void setGroupId(const std::string& groupId);
  void setWorkbenchId(const std::string& workbenchId);
  void setPluginLibrary(const std::string& thePluginLibrary);
};

#endif // CONFIG_MESSAGE_H