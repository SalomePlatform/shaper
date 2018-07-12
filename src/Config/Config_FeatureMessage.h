// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef CONFIG_FEATUREMESSAGE_H
#define CONFIG_FEATUREMESSAGE_H

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
  std::string myToolBarId;  ///<Id of toolbar/ By default == myWorkbenchId
  std::string myDocumentKind;  ///< kind of the document of the workbench (all documents if empty)
  std::string myPluginLibrary;  ///<Name of feature's library

  bool myUseInput;  ///<Action is being checked until user commit the operation
  bool myInternal;  ///<Internal feature without GUI representation
  bool myModal;     ///<True if the feature has to be represented by modal dialog box
  bool myIsAutoPreview; ///< Preview computation is performed automatically

  /// True if the feature can have Apply/Continue button in its property panel
  bool myIsApplyContinue;

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
  /// Id of Feature's Toolbar
  CONFIG_EXPORT const std::string& toolBarId() const;
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
  /// If true - the feature will be represented by modal dialog box GUI
  CONFIG_EXPORT bool isModal() const;
  /// If true - the feature can have Apply/Continue button in its property panel
  CONFIG_EXPORT bool isApplyContinue() const;



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
  ///Set id of Feature's Group
  CONFIG_EXPORT void setToolBarId(const std::string& aId);
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
  ///Set modality state; If true - the feature will be represented by modal dialog box GUI
  CONFIG_EXPORT void setModal(bool isModal);
  ///Set Apply/Continue state;
  ///If true - the feature can have Apply/Continue button in its property panel
  CONFIG_EXPORT void setApplyContinue(bool isModal);
};

#endif // CONFIG_MESSAGE_H
