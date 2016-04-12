// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef CONFIG_MESSAGE_H
#define CONFIG_MESSAGE_H

#include <Config_def.h>
#include <Events_Message.h>

#include <string>

/*!
 * \class Config_FeatureMessage
 * \ingroup Config
 * \brief Class to pass a feature entry extracted from xml file.
 * Example of the feature entry:
 * \code
 * <feature id="Part" text="New part" tooltip="Creates a new part" icon=":pictures/part_ico.png"/>
 * \endcode
 */
class Config_FeatureMessage : public Events_Message
{
  std::string myId;  ///<Feature unique id
  std::string myText;  ///<Represents action's text
  std::string myTooltip;  ///<Represents action's tooltip
  std::string myIcon;  ///<Represents action's icon
  std::string myKeysequence;  ///<Represents action's key sequence

  std::string myGroupId;  ///<Id of feature's group
  std::string myWorkbenchId;  ///<Id of feature's workbench
  std::string myDocumentKind;  ///< kind of the document of the workbench (all documents if empty)
  std::string myPluginLibrary;  ///<Name of feature's library

  bool myUseInput;  ///<Action is being checked until user commit the operation
  bool myInternal;  ///<Internal feature without GUI representation
  bool myIsAutoPreview; ///< Preview computation is performed automatically

  std::string myNestedFeatures; ///<Space separated list of child features
  std::string myActionsWhenNested; ///<Space separated list of actions

 public:
  /// Event ID that feature is loaded in workbench (GUI)
  inline static const char* GUI_EVENT()
  {
    static const char * MY_GUI_EVENT_ID("WorkshopFeatureLoaded");
    return MY_GUI_EVENT_ID;
  }
  /// Event ID that feature is loaded in workbench (Model)
  inline static const char* MODEL_EVENT()
  {
    static const char * MY_MODEL_EVENT_ID("ModelFeatureLoaded");
    return MY_MODEL_EVENT_ID;
  }

  /// Constructs Config_FeatureMessage
  CONFIG_EXPORT Config_FeatureMessage(const Events_ID theId, const void* theParent = 0);
  /// Deletes Config_FeatureMessage
  CONFIG_EXPORT virtual ~Config_FeatureMessage();

  //Auto-generated getters/setters
  /// Feature's Id
  CONFIG_EXPORT const std::string& id() const;
  /// Feature's Icon
  CONFIG_EXPORT const std::string& icon() const;
  /// Feature's shortcut
  CONFIG_EXPORT const std::string& keysequence() const;
  /// Feature's text
  CONFIG_EXPORT const std::string& text() const;
  /// Feature's tooltip
  CONFIG_EXPORT const std::string& tooltip() const;
  /// Id of Feature's Group
  CONFIG_EXPORT const std::string& groupId() const;
  /// Id of Feature's Workbench
  CONFIG_EXPORT const std::string& workbenchId() const;
  /// Kind of a document which contains the feature
  CONFIG_EXPORT const std::string& documentKind() const;
  /// Name of a library which contains the feature
  CONFIG_EXPORT const std::string& pluginLibrary() const;
  /// Space separated list of nested features
  CONFIG_EXPORT const std::string& nestedFeatures() const;
  /// Space separated list of actions
  CONFIG_EXPORT const std::string& actionsWhenNested() const;
  /// If false - feature has no Property panel representation
  CONFIG_EXPORT bool isUseInput() const;
  /// If true - feature will not be added into the workbench
  CONFIG_EXPORT bool isInternal() const;

  /// If true - preview of the feature is done by any modification of the feature attributes
  CONFIG_EXPORT bool isAutoPreview() const;

  ///Set feature's Id
  CONFIG_EXPORT void setId(const std::string& id);
  ///Set feature's Icon
  CONFIG_EXPORT void setIcon(const std::string& icon);
  ///Set feature's shortcut
  CONFIG_EXPORT void setKeysequence(const std::string& keysequence);
  ///Set feature's text
  CONFIG_EXPORT void setText(const std::string& text);
  ///Set feature's tooltip
  CONFIG_EXPORT void setTooltip(const std::string& tooltip);
  ///Set id of Feature's Group
  CONFIG_EXPORT void setGroupId(const std::string& groupId);
  ///Set id of Feature's Workbench
  CONFIG_EXPORT void setWorkbenchId(const std::string& workbenchId);
  ///Set kind of a document which contains the feature
  CONFIG_EXPORT void setDocumentKind(const std::string& documentKind);
  ///Set name of a library which contains the feature
  CONFIG_EXPORT void setPluginLibrary(const std::string& thePluginLibrary);
  ///Set space separated list of nested features
  CONFIG_EXPORT void setNestedFeatures(const std::string& theNestedFeatures);
  ///Set space separated list of nested features
  CONFIG_EXPORT void setActionsWhenNested(const std::string& theActions);
  ///Set use input state; If false - feature has no Property panel representation
  CONFIG_EXPORT void setUseInput(bool isUseInput);
  ///Set internal state; If true - feature will not be added into the workbench
  CONFIG_EXPORT void setInternal(bool isInternal);
  ///Set auto preview state; If true - preview of the feature is computed automatically
  CONFIG_EXPORT void setAutoPreview(bool isAutoPreview);
};

#endif // CONFIG_MESSAGE_H
