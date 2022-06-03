// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#ifndef Events_MessageBool_H_
#define Events_MessageBool_H_

#include <Events.h>
#include <Events_Message.h>
#include <Events_Loop.h>

#include <string>
#include <list>

/**\class Events_MessageBool
 * \ingroup EventsLoop
 * \brief An event message for sending a message with a boolean value.
 *        May be used for different where just a Boolean flag is needed to send.
 */
class Events_MessageBool: public Events_Message
{
public:

  /// Constructor
  /// \param theEventID an indentifier of the message
  /// \param theValue a Boolean value to send
  /// \param theSender a pointer on sender object
  Events_MessageBool(const Events_ID theEventID, const bool theValue, const void* theSender = 0):
  Events_Message(theEventID, theSender), myValue(theValue) {}

  /// Default destructor
  virtual ~Events_MessageBool() {}

  /// Returns the value stored in this message.
  const bool value() const { return myValue; }

  /// Sends the message
  EVENTS_EXPORT void send();

private:

  /// The stored value
  bool myValue;

};

#endif
