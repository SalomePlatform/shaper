// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * Config_SelectionFilterMessage.cpp
 *
 *  Created on: 08 Jul 2014
 *      Author: sbh
 */

#include <Config_SelectionFilterMessage.h>

Config_SelectionFilterMessage::Config_SelectionFilterMessage(const Events_ID theId,
                                                             const void* theParent)
    : Events_Message(theId, theParent)
{
  mySelectionFilterId = "";
  myFeatureId = "";
  myAttributeId = "";
}

Config_SelectionFilterMessage::~Config_SelectionFilterMessage()
{
}

const std::string& Config_SelectionFilterMessage::selectionFilterId() const
{
  return mySelectionFilterId;
}

const std::string& Config_SelectionFilterMessage::featureId() const
{
  return myFeatureId;
}

void Config_SelectionFilterMessage::setSelectionFilterId(const std::string& theId)
{
  mySelectionFilterId = theId;
}

const std::string& Config_SelectionFilterMessage::attributeId() const
{
  return myAttributeId;
}

void Config_SelectionFilterMessage::setFeatureId(const std::string& theId)
{
  myFeatureId = theId;
}

void Config_SelectionFilterMessage::setAttributeId(const std::string& theId)
{
  myAttributeId = theId;
}

