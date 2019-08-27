// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include <Config_Keywords.h>
#include <Config_Common.h>
#include <Config_FeatureMessage.h>
#include <Config_AttributeMessage.h>
#include <Config_FeatureReader.h>
#include <Config_Translator.h>
#include <Events_Message.h>
#include <Events_Loop.h>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlstring.h>

#include <string>
#include <algorithm>
#include <list>

#ifdef _DEBUG
#include <iostream>
#endif

Config_FeatureReader::Config_FeatureReader(const std::string& theXmlFile,
                                           const std::string& theLibraryName,
                                           const char* theEventGenerated)
    : Config_XMLReader(theXmlFile),
      myLibraryName(theLibraryName),
      myEventGenerated(theEventGenerated ? theEventGenerated : Config_FeatureMessage::GUI_EVENT()),
      myIsProcessWidgets(theEventGenerated != NULL)
{
}

Config_FeatureReader::~Config_FeatureReader()
{
}

std::list<std::string> Config_FeatureReader::features() const
{
  return myFeatures;
}

void Config_FeatureReader::processNode(xmlNodePtr theNode)
{
  Events_ID aMenuItemEvent = Events_Loop::eventByName(myEventGenerated);
  if (isNode(theNode, NODE_FEATURE, NULL)) {
    storeAttribute(theNode, _ID);
    std::shared_ptr<Config_FeatureMessage>
      aMessage(new Config_FeatureMessage(aMenuItemEvent, this));
    fillFeature(theNode, aMessage);
    myFeatures.push_back(getProperty(theNode, _ID));
    //If a feature has xml definition for it's widget:
    aMessage->setUseInput(hasChild(theNode));
    Events_Loop::loop()->send(aMessage);
    //The m_last* variables always defined before fillFeature() call. XML is a tree.
  } else if (isNode(theNode, NODE_WORKBENCH, NODE_GROUP, NULL)) {
    storeAttribute(theNode, _ID);
    storeAttribute(theNode, GROUP_TOOLBAR);
    storeAttribute(theNode, WORKBENCH_DOC, true);
  } else if (myIsProcessWidgets) {
    // widgets, like shape_selector or containers, like toolbox
    if (isAttributeNode(theNode)) {
      std::shared_ptr<Config_AttributeMessage>
        aMessage(new Config_AttributeMessage(aMenuItemEvent, this));
      aMessage->setFeatureId(restoreAttribute(NODE_FEATURE, _ID));
      std::string anAttributeID = getProperty(theNode, _ID);
      if (!anAttributeID.empty()) {
        aMessage->setAttributeId(anAttributeID);
        aMessage->setObligatory(getBooleanAttribute(theNode, ATTR_OBLIGATORY, true));
        bool isConcealment = getBooleanAttribute(theNode, ATTR_CONCEALMENT, false);
        aMessage->setConcealment(isConcealment);
        bool isMainArg = isConcealment && getBooleanAttribute(theNode, ATTR_MAIN_ARG, false);
        aMessage->setMainArgument(isMainArg);
        aMessage->setGeometricalSelection(getBooleanAttribute(theNode,
                                                              ATTR_GEOMETRICAL_SELECTION,
                                                              false));

        std::list<std::pair<std::string, std::string> > aCases;
        xmlNodePtr aCaseNode = hasParentRecursive(theNode,
          WDG_SWITCH_CASE, WDG_TOOLBOX_BOX, WDG_OPTIONALBOX, WDG_RADIOBOX_ITEM, NULL);
        while(aCaseNode) {
          std::string aCaseNodeID = getProperty(aCaseNode, _ID);
          std::string aSwitchNodeID = "";
          const xmlChar* aName = aCaseNode->name;
          xmlNodePtr aSwitchNode;
          if (!xmlStrcmp(aName, (const xmlChar *) WDG_SWITCH_CASE)) {
            aSwitchNode = hasParentRecursive(aCaseNode, WDG_SWITCH, NULL);
          }
          else if (!xmlStrcmp(aName, (const xmlChar *) WDG_TOOLBOX_BOX)) {
            aSwitchNode = hasParentRecursive(aCaseNode, WDG_TOOLBOX, NULL);
          }
          else if (!xmlStrcmp(aName, (const xmlChar *)WDG_RADIOBOX_ITEM)) {
            aSwitchNode = hasParentRecursive(aCaseNode, WDG_RADIOBOX, NULL);
          }
          if (!xmlStrcmp(aName, (const xmlChar *) WDG_OPTIONALBOX)) {
            /// the box is optional, attribute is in case
            /// if the optional attribute value is not empty
            aSwitchNode = aCaseNode;
          }
          if (aSwitchNode)
            aSwitchNodeID = getProperty(aSwitchNode, _ID);

          aCases.push_back(std::make_pair(aSwitchNodeID, aCaseNodeID));
          aCaseNode = hasParentRecursive(aSwitchNode, WDG_SWITCH_CASE,
            WDG_TOOLBOX_BOX, WDG_OPTIONALBOX, WDG_RADIOBOX_ITEM, NULL);
        }
        aMessage->setCases(aCases);
        Events_Loop::loop()->send(aMessage);
      }
    // container pages, like "case" or "box"
    } else if (isNode(theNode, WDG_OPTIONALBOX, WDG_SWITCH, WDG_SWITCH_CASE,
                      WDG_TOOLBOX, WDG_TOOLBOX_BOX,
                      WDG_RADIOBOX, WDG_RADIOBOX_ITEM, NULL)) {
      storeAttribute(theNode, _ID); // save case:caseId (or box:boxId)
    }
  }
  //Process SOURCE nodes.
  Config_XMLReader::processNode(theNode);
}

void Config_FeatureReader::cleanup(xmlNodePtr theNode)
{
  if (isNode(theNode, WDG_OPTIONALBOX, WDG_SWITCH, WDG_SWITCH_CASE,
    WDG_TOOLBOX, WDG_TOOLBOX_BOX, WDG_RADIOBOX_ITEM, WDG_RADIOBOX, NULL)) {
    // cleanup id of cases when leave case node
    cleanupAttribute(theNode, _ID);
  }
}

bool Config_FeatureReader::processChildren(xmlNodePtr theNode)
{
  bool result = isNode(theNode, NODE_WORKBENCH, NODE_GROUP, NULL);
  if(!result && myIsProcessWidgets) {
    result = isNode(theNode, NODE_FEATURE,
                             WDG_GROUP, WDG_OPTIONALBOX,
                             WDG_TOOLBOX, WDG_TOOLBOX_BOX,
                             WDG_RADIOBOX, WDG_RADIOBOX_ITEM,
                             WDG_SWITCH, WDG_SWITCH_CASE, NULL);
  }
  return result;
}

void Config_FeatureReader::fillFeature(xmlNodePtr theFeatureNode,
  const std::shared_ptr<Config_FeatureMessage>& outFeatureMessage)
{
  std::string anId = getProperty(theFeatureNode, _ID);
  outFeatureMessage->setId(anId);
  outFeatureMessage->setPluginLibrary(myLibraryName);
  outFeatureMessage->setNestedFeatures(getProperty(theFeatureNode, FEATURE_NESTED));
  outFeatureMessage->setActionsWhenNested(getNormalizedProperty(theFeatureNode,
                                          FEATURE_WHEN_NESTED));
  outFeatureMessage->setModal(getBooleanAttribute(theFeatureNode, FEATURE_MODAL, false));
  bool isAutoPreview = getBooleanAttribute(theFeatureNode, FEATURE_AUTO_PREVIEW, true);
  outFeatureMessage->setAutoPreview(isAutoPreview);
  outFeatureMessage->setApplyContinue(
    getBooleanAttribute(theFeatureNode, FEATURE_APPLY_CONTINUE, false));

  bool isInternal = getBooleanAttribute(theFeatureNode, ATTR_INTERNAL, false);
  outFeatureMessage->setInternal(isInternal);

  std::string aHelpFile = getProperty(theFeatureNode, HELP_FILE);
  if (!aHelpFile.empty())
    outFeatureMessage->setHelpFileName(myLibraryName + "/" + aHelpFile);

  if (isInternal) {
    //Internal feature has no visual representation.
    return;
  }

  std::string aText = Config_Translator::translate(anId, getProperty(theFeatureNode, FEATURE_TEXT));
  outFeatureMessage->setText(aText);
  bool isTitleInToolbar = getBooleanAttribute(theFeatureNode, FEATURE_TITLE_IN_TOOLBAR, true);
  outFeatureMessage->setTitleInToolbar(isTitleInToolbar);
  std::string aToolTip = Config_Translator::translate(
    anId, getProperty(theFeatureNode, FEATURE_TOOLTIP));
  outFeatureMessage->setTooltip(aToolTip);
  outFeatureMessage->setIcon(getProperty(theFeatureNode, FEATURE_ICON));
  outFeatureMessage->setKeysequence(getProperty(theFeatureNode, FEATURE_KEYSEQUENCE));

  std::string aGroupName = restoreAttribute(NODE_GROUP, _ID);
  std::string aWBNName = restoreAttribute(NODE_WORKBENCH, _ID);
  std::string isGroupToolbarId = restoreAttribute(NODE_GROUP, GROUP_TOOLBAR);
  bool isGroupToolbar = false;
  if (isGroupToolbarId.length() > 0)
    isGroupToolbar = (isGroupToolbarId == "yes");
  outFeatureMessage->setGroupId(aGroupName);
  outFeatureMessage->setWorkbenchId(aWBNName);
  outFeatureMessage->setToolBarId(isGroupToolbar ? aGroupName : aWBNName);

  // Get document kind of a feature, if empty set workbench's kind (might be empty too)
  std::string aDocKind = getProperty(theFeatureNode, FEATURE_DOC);
  if(aDocKind.empty()) {
    aDocKind = restoreAttribute(NODE_WORKBENCH, WORKBENCH_DOC);
  }
  outFeatureMessage->setDocumentKind(aDocKind);
}
