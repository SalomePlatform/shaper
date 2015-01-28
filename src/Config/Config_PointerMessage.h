// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * Config_PointerMessage.h
 *
 *  Created on: Mar 21, 2014
 *      Author: sbh
 */

#ifndef PARTSET_MESSAGE_H_
#define PARTSET_MESSAGE_H_

#include <Config_def.h>
#include <Events_Message.h>

/*!
 * \class Config_PointerMessage
 * \ingroup Config
 * \brief A general class to pass pointers over the event loop.
 */
class CONFIG_EXPORT Config_PointerMessage : public Events_Message
{
 public:
  /// Constructor
  Config_PointerMessage(const Events_ID theId, const void* theParent = 0);
  virtual ~Config_PointerMessage();

  /// Returns pointer to an object
  void* pointer() const;
  /// Sets pointer to an object
  void setPointer(void* pointer);

 private:
  void* myPointer; ///< Pointer to an object
};

#endif /* PARTSET_MESSAGE_H_ */
