// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * Config_DataModelReader.cpp
 *
 *  Created on: Jul 21, 2015
 *      Author: vsv
 */

#include "Config_DataModelReader.h"
#include <Config_Keywords.h>
#include "Config_Common.h"

#include <Events_Error.h>


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
      Events_Error::send("Reading dataModel.xml: wrong folder definition");
   
    std::string aIcon = getProperty(theNode, NODE_ICON);
    std::string aEmpty = getProperty(theNode, SHOW_EMPTY);
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
   } else {
      mySubFolderNames.push_back(aName);
      mySubFolderTypes.push_back(aGroupType);
      mySubFolderIcons.push_back(aIcon);
      mySubFolderShowEmpty.push_back(aIsEmpty);
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