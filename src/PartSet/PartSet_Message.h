/*
 * PartSet_Message.h
 *
 *  Created on: Mar 21, 2014
 *      Author: sbh
 */

#ifndef PARTSET_MESSAGE_H_
#define PARTSET_MESSAGE_H_

#include <PartSet.h>

#include <QString>

#include <Event_Message.hxx>

class PARTSET_EXPORT PartSet_Message: public Event_Message
{
public:
  PartSet_Message(const Event_ID theId, const void* theParent = 0);
  virtual ~PartSet_Message();
};

#endif /* PARTSET_MESSAGE_H_ */
