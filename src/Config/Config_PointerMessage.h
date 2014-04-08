/*
 * Config_PointerMessage.h
 *
 *  Created on: Mar 21, 2014
 *      Author: sbh
 */

#ifndef PARTSET_MESSAGE_H_
#define PARTSET_MESSAGE_H_

#include <Config.h>
#include <Event_Message.h>

/*
 * A general class to pass pointers over the event loop.
 */
class CONFIG_EXPORT Config_PointerMessage: public Event_Message
{
public:
  Config_PointerMessage(const Event_ID theId, const void* theParent = 0);
  virtual ~Config_PointerMessage();

  void* pointer() const;
  void setPointer(void* pointer);

private:
  void* myPointer;
};

#endif /* PARTSET_MESSAGE_H_ */
