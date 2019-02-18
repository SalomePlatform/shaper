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

#ifndef XGUI_QTEVENTS_H_
#define XGUI_QTEVENTS_H_

#include <XGUI.h>

#include <ModelAPI_ResultPart.h>
#include <Events_Message.h>

#include <QEvent>
#include <QString>

/**
* \ingroup GUI
* Class of event to send application events in multi thread environment
*/
class XGUI_EXPORT PostponeMessageQtEvent : public QEvent
{
 public:
   /// An event type
  static QEvent::Type PostponeMessageQtEventType;

  /// Constructor
  /// \param theMessage an event message to send
  PostponeMessageQtEvent(const std::shared_ptr<Events_Message>& theMessage)
      : QEvent(PostponeMessageQtEventType),
      myMessage(theMessage)
  {
  }

  /// Returns type of the event
  static QEvent::Type type()
  {
    return PostponeMessageQtEventType;
  }

  /// Returns current messasge
  std::shared_ptr<Events_Message> postponedMessage();

 private:
   /// Message
  std::shared_ptr<Events_Message> myMessage;
};

#endif /* XGUI_QEVENTS_H_ */
