/*
 * Config_WidgetMessage.cpp
 *
 *  Created on: Mar 21, 2014
 *      Author: sbh
 */

#include <Config_WidgetMessage.h>

Config_WidgetMessage::Config_WidgetMessage(const Event_ID theId, const void* theParent)
    : Event_Message(theId, theParent)
{

}

Config_WidgetMessage::~Config_WidgetMessage()
{
}

const std::string& Config_WidgetMessage::featureId() const
{
  return myFeatureId;
}

void Config_WidgetMessage::setFeatureId(const std::string& theFeatureId)
{
  myFeatureId = theFeatureId;
}

const std::string& Config_WidgetMessage::xmlRepresentation() const
{
  return myXMLRepr;
}

void Config_WidgetMessage::setXmlRepresentation(const std::string& theXmlRep)
{
  myXMLRepr = theXmlRep;
}
