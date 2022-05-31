// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "Config_FeatureMessage.h"

Config_FeatureMessage::Config_FeatureMessage(const Events_ID theId, const void* theParent)
    : Events_Message(theId, theParent)
{
  myId = "";
  myText = "";
  myTooltip = "";
  myIcon = "";
  myKeysequence = "";
  myHelpFile = "";

  myGroupId = "";
  myWorkbenchId = "";
  myToolBarId = "";
  myPluginLibrary = "";

  myInternal = false;
  myUseInput = false;
  myNestedFeatures = "";
  myModal = false;
  myIsTitleInToolbar = true;
  myIsApplyContinue = false;
  myHideFacesPanel = false;
  myAbortConfirmation = true;
}

Config_FeatureMessage::~Config_FeatureMessage()
{

}

const std::string& Config_FeatureMessage::icon() const
{
  return myIcon;
}

void Config_FeatureMessage::setIcon(const std::string& icon)
{
  myIcon = icon;
}

const std::string& Config_FeatureMessage::id() const
{
  return myId;
}

void Config_FeatureMessage::setId(const std::string& id)
{
  myId = id;
}

const std::string& Config_FeatureMessage::keysequence() const
{
  return myKeysequence;
}

void Config_FeatureMessage::setKeysequence(const std::string& keysequence)
{
  myKeysequence = keysequence;
}

const std::string& Config_FeatureMessage::text() const
{
  return myText;
}

void Config_FeatureMessage::setText(const std::string& text)
{
  myText = text;
}

const std::string& Config_FeatureMessage::tooltip() const
{
  return myTooltip;
}

const std::string& Config_FeatureMessage::groupId() const
{
  return myGroupId;
}

void Config_FeatureMessage::setGroupId(const std::string& groupId)
{
  myGroupId = groupId;
}

const std::string& Config_FeatureMessage::toolBarId() const
{
  return myToolBarId;
}

void Config_FeatureMessage::setToolBarId(const std::string& aId)
{
  myToolBarId = aId;
}

const std::string& Config_FeatureMessage::workbenchId() const
{
  return myWorkbenchId;
}

void Config_FeatureMessage::setWorkbenchId(const std::string& workbenchId)
{
  myWorkbenchId = workbenchId;
}

const std::string& Config_FeatureMessage::documentKind() const
{
  return myDocumentKind;
}

void Config_FeatureMessage::setDocumentKind(const std::string& documentKind)
{
  myDocumentKind = documentKind;
}

void Config_FeatureMessage::setTooltip(const std::string& tooltip)
{
  myTooltip = tooltip;
}

const std::string& Config_FeatureMessage::pluginLibrary() const
{
  return myPluginLibrary;
}

void Config_FeatureMessage::setPluginLibrary(const std::string& thePluginLibrary)
{
  this->myPluginLibrary = thePluginLibrary;
}

// LCOV_EXCL_START
bool Config_FeatureMessage::isUseInput() const
{
  return myUseInput;
}

bool Config_FeatureMessage::isInternal() const
{
  return myInternal;
}

bool Config_FeatureMessage::isAutoPreview() const
{
  return myIsAutoPreview;
}

bool Config_FeatureMessage::isTitleInToolbar() const
{
  return myIsTitleInToolbar;
}

bool Config_FeatureMessage::isModal() const
{
  return myModal;
}

bool Config_FeatureMessage::isApplyContinue() const
{
  return myIsApplyContinue;
}
// LCOV_EXCL_STOP

void Config_FeatureMessage::setUseInput(bool isUseInput)
{
  myUseInput = isUseInput;
}

void Config_FeatureMessage::setInternal(bool isInternal)
{
  myInternal = isInternal;
}

void Config_FeatureMessage::setModal(bool isModal)
{
  myModal = isModal;
}

const std::string& Config_FeatureMessage::nestedFeatures() const
{
  return myNestedFeatures;
}

const std::string& Config_FeatureMessage::actionsWhenNested() const
{
  return myActionsWhenNested;
}

void Config_FeatureMessage::setNestedFeatures(const std::string& theNestedFeatures)
{
  myNestedFeatures = theNestedFeatures;
}

void Config_FeatureMessage::setActionsWhenNested(const std::string& theActions)
{
  myActionsWhenNested = theActions;
}

void Config_FeatureMessage::setAutoPreview(bool isAutoPreview)
{
  myIsAutoPreview = isAutoPreview;
}

void Config_FeatureMessage::setApplyContinue(bool isModal)
{
  myIsApplyContinue = isModal;
}

const std::string& Config_FeatureMessage::helpFileName() const
{
  return myHelpFile;
}

void Config_FeatureMessage::setHelpFileName(const std::string& aName)
{
  myHelpFile = aName;
}

void Config_FeatureMessage::setTitleInToolbar(bool theValue)
{
  myIsTitleInToolbar = theValue;
}

bool Config_FeatureMessage::isHideFacesPanel() const
{
  return myHideFacesPanel;
}


void Config_FeatureMessage::setHideFacesPanel(bool theValue)
{
  myHideFacesPanel = theValue;
}

bool Config_FeatureMessage::isAbortConfirmation() const
{
  return myAbortConfirmation;
}

void Config_FeatureMessage::setAbortConfirmation(bool theValue)
{
  myAbortConfirmation = theValue;
}
