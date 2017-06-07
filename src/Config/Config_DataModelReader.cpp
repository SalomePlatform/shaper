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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

/*
 * Config_DataModelReader.cpp
 *
 *  Created on: Jul 21, 2015
 *      Author: vsv
 */

#include "Config_DataModelReader.h"
#include <Config_Keywords.h>
#include "Config_Common.h"

#include <Events_InfoMessage.h>


Config_DataModelReader::Config_DataModelReader()
    : Config_XMLReader(DATAMODEL_FILE), isRootReading(true), myIsResultLink(false)
{
}

Config_DataModelReader::~Config_DataModelReader()
{
}

void Config_DataModelReader::processNode(xmlNodePtr theNode)
{
  if (isNode(theNode, NODE_FOLDER, NULL)) {
    std::string aName = getProperty(theNode, FOLDER_NAME);
    std::string aGroupType = getProperty(theNode, GROUP_TYPE);
    if (aName.empty() || aGroupType.empty())
      Events_InfoMessage("Config_DataModelReader",
        "Reading dataModel.xml: wrong folder definition.").send();

    std::string aIcon = getProperty(theNode, NODE_ICON);
    std::string aEmpty = getProperty(theNode, SHOW_EMPTY);
    std::string aFeatures = getProperty(theNode, FOLDER_FEATURES);
    std::string::iterator aIt;
    for (aIt = aEmpty.begin(); aIt != aEmpty.end(); aIt++) {
      (*aIt) = toupper(*aIt);
    }
    bool aIsEmpty = (aEmpty == "FALSE")? false : true;

   if (isRootReading) {
      myRootFolderNames.push_back(aName);
      myRootFolderTypes.push_back(aGroupType);
      myRootFolderIcons.push_back(aIcon);
      myRootFolderShowEmpty.push_back(aIsEmpty);
      myRootFeaturesList.push_back(aFeatures);
   } else {
      mySubFolderNames.push_back(aName);
      mySubFolderTypes.push_back(aGroupType);
      mySubFolderIcons.push_back(aIcon);
      mySubFolderShowEmpty.push_back(aIsEmpty);
      mySubFeaturesList.push_back(aFeatures);
   }
  } else if  (isNode(theNode, ROOT_DOCUMENT, NULL)) {
    isRootReading = true;
    myRootTypes = getProperty(theNode, GROUP_TYPE);
  } else if  (isNode(theNode, SUB_DOCUMENT, NULL)) {
    isRootReading = false;
    mySubTypes = getProperty(theNode, GROUP_TYPE);
    std::string isResult = getProperty(theNode, LINK_ITEM);
    std::string::iterator aIt;
    for (aIt = isResult.begin(); aIt != isResult.end(); aIt++) {
      (*aIt) = toupper(*aIt);
    }
    myIsResultLink = (isResult == "TRUE")? true : false;
  }
}

int Config_DataModelReader::rootFolderId(std::string theType) const
{
  std::vector<std::string>::const_iterator aIt;
  int aId;
  for (aIt = myRootFolderTypes.cbegin(), aId = 0; aIt != myRootFolderTypes.cend(); ++aIt, ++aId) {
    if ((*aIt) == theType)
      return aId;
  }
  return -1;
}

int Config_DataModelReader::subFolderId(std::string theType) const
{
  std::vector<std::string>::const_iterator aIt;
  int aId;
  for (aIt = mySubFolderTypes.cbegin(), aId = 0; aIt != mySubFolderTypes.cend(); ++aIt, ++aId) {
    if ((*aIt) == theType)
      return aId;
  }
  return -1;
}

std::string getFolderFeatures(const std::string& theFolderName,
                    const std::vector<std::string>& theNames,
                    const std::vector<std::string>& theFeatures)
{
  int aId;
  bool aFound = false;
  std::vector<std::string>::const_iterator aIt;
  for(aIt = theNames.cbegin(), aId = 0; aIt != theNames.cend(); ++aIt, ++aId) {
    if ((*aIt) == theFolderName) {
      aFound = true;
      break;
    }
  }
  if (aFound)
    return theFeatures.at(aId);
  return std::string();
}

std::string Config_DataModelReader::
  subFolderFeatures(const std::string& theFolderName) const
{
  return getFolderFeatures(theFolderName, mySubFolderNames, mySubFeaturesList);
}


std::string Config_DataModelReader::
  rootFolderFeatures(const std::string& theFolderName) const
{
  return getFolderFeatures(theFolderName, myRootFolderNames, myRootFeaturesList);
}

