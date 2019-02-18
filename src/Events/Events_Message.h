// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef Events_Message_H_
#define Events_Message_H_

#include <Events.h>

/**\class Events_ID
 * \ingroup EventsLoop
 * \brief Identifier of the event kind.
 *
 * Each event ID is created in main Envent_Loop class
 * that stores correspondence between the string-name of the
 * identifier and the pointer to the static string that is really
 * used as an identifier (this is useful for debugging of the events
 * with log files and in debugger).
 */
class EVENTS_EXPORT Events_ID
{
  /// pointer to the text-identifier of the event, unique pointer for all events of such type
  char* myID;

  Events_ID(char* theID)
  {
    myID = theID;
  }

  friend class Events_Loop;
 public:
  /// Returns the text-identifier of the event (for debugging reasons)
  char* eventText() const
  {
    return myID;
  }
  /// Allows to compare identifiers
  bool operator==(const Events_ID& theID) const
  {
    return myID == theID.myID;
  }
};

/**\class Events_Message
 * \ingroup EventsLoop
 * \brief Message for communication between sender and listener of event.
 * Normally it is inherited by the higher-level 
 */
class EVENTS_EXPORT Events_Message
{
  Events_ID myEventsId;  ///< identifier of the event
  void* mySender;  ///< the sender object

 public:

  //! Creates the message
  Events_Message(const Events_ID theID, const void* theSender = 0)
      : myEventsId(theID),
        mySender((void*) theSender)
  {
  }
  //! do nothing in the destructor yet
  virtual ~Events_Message()
  {
  }

  //! Returns identifier of the message
  const Events_ID& eventID() const
  {
    return myEventsId;
  }

  //! Returns sender of the message or NULL if it is anonymous message
  void* sender() const
  {
    return mySender;
  }
};

#endif
