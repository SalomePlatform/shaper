// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * Config_SelectionFilterMessage.h
 *
 *  Created on: 08 Jul 2014
 *      Author: sbh
 */

#ifndef CONFIG_SELECTIONFILTERMESSAGE_H_
#define CONFIG_SELECTIONFILTERMESSAGE_H_

#include <Events_Message.h>
#include <Config_def.h>

#include <list>
#include <string>

/// Event ID that Selection Filter is loaded (comes with Config_SelectionFilterMessage)
static const char * EVENT_SELFILTER_LOADED = "SelectionFilterLoaded";

class Config_SelectionFilterMessage : public Events_Message
{
  std::string mySelectionFilterId;
  std::string myFeatureId;
  std::string myAttributeId;
  std::list<std::string> myFilterParameters;

 public:
  CONFIG_EXPORT Config_SelectionFilterMessage(const Events_ID theId, const void* theParent = 0);
  CONFIG_EXPORT virtual ~Config_SelectionFilterMessage();

  CONFIG_EXPORT const std::string& selectionFilterId() const;
  CONFIG_EXPORT const std::string& featureId() const;
  CONFIG_EXPORT const std::string& attributeId() const;
  CONFIG_EXPORT const std::list<std::string>& parameters() const;

  CONFIG_EXPORT void setSelectionFilterId(const std::string& theId);
  CONFIG_EXPORT void setFeatureId(const std::string& theId);
  CONFIG_EXPORT void setAttributeId(const std::string& theId);
  CONFIG_EXPORT void setFilterParameters(const std::list<std::string>& parameters);
};

#endif /* CONFIG_SELECTIONFILTERMESSAGE_H_ */
