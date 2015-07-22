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
    : Config_XMLReader(DATAMODEL_FILE)
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
   
    myRootFolderNames.push_back(aName);
    myRootFolderTypes.push_back(aGroupType);
    myRootFolderIcons.push_back(getProperty(theNode, NODE_ICON));
  } else if  (isNode(theNode, ROOT_NODE, NULL)) {
    myRootTypes = getProperty(theNode, GROUP_TYPE);
  }
}
