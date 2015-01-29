// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

/*
 * XGUI_QEvents.h
 *
 *  Created on: Sep 12, 2014
 *      Author: sbh
 */

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
