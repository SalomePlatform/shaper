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

class XGUI_EXPORT PostponeMessageQtEvent : public QEvent
{
 public:
  static QEvent::Type PostponeMessageQtEventType;

  PostponeMessageQtEvent(const std::shared_ptr<Events_Message>& theMessage)
      : QEvent(PostponeMessageQtEventType),
      myMessage(theMessage)
  {
  }
  static QEvent::Type type()
  {
    return PostponeMessageQtEventType;
  }

  std::shared_ptr<Events_Message> postponedMessage();

 private:
  std::shared_ptr<Events_Message> myMessage;
};

#endif /* XGUI_QEVENTS_H_ */
