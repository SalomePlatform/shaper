/*
 * Config_WidgetMessage.h
 *
 *  Created on: Mar 21, 2014
 *      Author: sbh
 */

#ifndef PARTSET_MESSAGE_H_
#define PARTSET_MESSAGE_H_

#include <Config.h>

#include <Event_Message.h>
#include <string>

class CONFIG_EXPORT Config_WidgetMessage: public Event_Message
{
public:
  Config_WidgetMessage(const Event_ID theId, const void* theParent = 0);
  virtual ~Config_WidgetMessage();

  const std::string& featureId() const;
  const std::string& xmlRepresentation() const;
  void setFeatureId(const std::string& featureId);
  void setXmlRepresentation(const std::string& xmlRepresentation);

  std::string myFeatureId;
  std::string myXMLRepr;
};

#endif /* PARTSET_MESSAGE_H_ */
