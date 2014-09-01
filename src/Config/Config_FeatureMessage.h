#ifndef CONFIG_MESSAGE_H
#define CONFIG_MESSAGE_H

#include <Config_def.h>
#include <Events_Message.h>

#include <string>

/// Event ID that feature is loaded (comes with Config_FeatureMessage)
static const char * EVENT_FEATURE_LOADED = "FeatureLoaded";

/*
 * Class to pass a feature entry extracted from xml file.
 * Example of the feature entry:
 * <feature id="Part" text="New part" tooltip="Creates a new part" icon=":pictures/part_ico.png"/>
 */
class Config_FeatureMessage : public Events_Message
{
  std::string myId;  //Feature unique id
  std::string myText;  //Represents action's text
  std::string myTooltip;  //Represents action's tooltip
  std::string myIcon;  //Represents action's icon
  std::string myKeysequence;  //Represents action's key sequence

  std::string myGroupId;  //Id of feature's group
  std::string myWorkbenchId;  //Id of feature's workbench
  std::string myPluginLibrary;  //Name of feature's library

  bool myUseInput;  //Action is being checked until user commit the operation
  bool myInternal;  //Internal feature without GUI representation
  std::string myNestedFeatures;

 public:
  //const Events_ID theID, const void* theSender = 0
  CONFIG_EXPORT Config_FeatureMessage(const Events_ID theId, const void* theParent = 0);CONFIG_EXPORT virtual ~Config_FeatureMessage();

  //Auto-generated getters/setters
  CONFIG_EXPORT const std::string& icon() const;CONFIG_EXPORT const std::string& id() const;CONFIG_EXPORT const std::string& keysequence() const;CONFIG_EXPORT const std::string& text() const;CONFIG_EXPORT const std::string& tooltip() const;

  CONFIG_EXPORT const std::string& groupId() const;CONFIG_EXPORT const std::string& workbenchId() const;CONFIG_EXPORT const std::string& pluginLibrary() const;CONFIG_EXPORT const std::string& nestedFeatures() const;CONFIG_EXPORT bool isUseInput() const;CONFIG_EXPORT bool isInternal() const;

  CONFIG_EXPORT void setIcon(const std::string& icon);CONFIG_EXPORT void setId(
      const std::string& id);CONFIG_EXPORT void setKeysequence(const std::string& keysequence);CONFIG_EXPORT void setText(
      const std::string& text);CONFIG_EXPORT void setTooltip(const std::string& tooltip);CONFIG_EXPORT void setGroupId(
      const std::string& groupId);CONFIG_EXPORT void setWorkbenchId(const std::string& workbenchId);CONFIG_EXPORT void setPluginLibrary(
      const std::string& thePluginLibrary);CONFIG_EXPORT void setNestedFeatures(
      const std::string& theNestedFeatures);CONFIG_EXPORT void setUseInput(bool isUseInput);CONFIG_EXPORT void setInternal(
      bool isInternal);
};

#endif // CONFIG_MESSAGE_H
