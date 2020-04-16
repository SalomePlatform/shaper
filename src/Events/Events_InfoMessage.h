// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef Events_InfoMessage_H_
#define Events_InfoMessage_H_

#include <Events.h>
#include <Events_Message.h>
#include <Events_Loop.h>

#include <string>
#include <list>

/**\class Events_InfoMessage
 * \ingroup EventsLoop
 * \brief An event message for sending a string message which has to be translated.
 */
class Events_InfoMessage: public Events_Message
{
public:

  /// Constructor
  /// \param theSender a pointer on sender object
  explicit Events_InfoMessage(const void* theSender = 0):
    Events_Message(Events_Loop::eventByName("InfoMessage"), theSender) {}

  /// Constructor
  /// \param theSender a pointer on sender object
  Events_InfoMessage(const std::string& theContext,
    const std::string& theMsg, const void* theSender = 0):
  Events_Message(Events_Loop::eventByName("InfoMessage"), theSender),
    myContext(theContext), myMessage(theMsg) {}

  /// default destructor
  virtual ~Events_InfoMessage() {}

  /// Identifier of this event (one for all errors)
  static Events_ID errorID()  { return Events_Loop::loop()->eventByName("InfoMessage"); }

  /// Set a context string
  /// \param theContext a context string
  void  setContext(const std::string& theContext) { myContext = theContext; }


  /// Returns context string
  std::string context() const { return myContext; }

  /// Set message string for translation
  /// \param theMsg the string of message
  void setMessageString(const std::string& theMsg) { myMessage = theMsg; }

  /// Returns message
  std::string messageString() const { return myMessage; }

  Events_InfoMessage& operator=(const std::string& theMsg) {
    setMessageString(theMsg);
    return *this;
  }

  bool empty() const {
    return myMessage.empty();
  }

  /// Add parameter for message string of string type
  /// \param theParam the parameter
  void addParameter(const std::string& theParam)
  {
    myParameters.push_back(theParam);
  }

  /// Add parameter for message string of double type
  /// \param theParam the parameter
  EVENTS_EXPORT void addParameter(double theParam);

  /// Add parameter for message string of integer type
  /// \param theParam the parameter
  EVENTS_EXPORT void addParameter(int theParam);

  /// Returns list of parameters
  std::list<std::string> parameters() const { return myParameters; }

  /// Add parameter for message string of string type
  /// \param theParam the parameter
  Events_InfoMessage& arg(const std::string& theParam) { addParameter(theParam); return *this; }

  /// Add parameter for message string of integer type
  /// \param theParam the parameter
  Events_InfoMessage& arg(int theParam) { addParameter(theParam); return *this; }

  /// Add parameter for message string of double type
  /// \param theParam the parameter
  Events_InfoMessage& arg(double theParam) { addParameter(theParam); return *this; }

  /// Send the message
  EVENTS_EXPORT void send();

private:

  /// Context of the messgae
  std::string myContext;

  /// String of the message
  std::string myMessage;

  /// Parameters of the message
  std::list<std::string> myParameters;
};

#endif
