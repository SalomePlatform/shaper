/*
 * XGUI_QEvents.cpp
 *
 *  Created on: Sep 12, 2014
 *      Author: sbh
 */

#include "XGUI_QtEvents.h"

QEvent::Type PostponeMessageQtEvent::PostponeMessageQtEventType = QEvent::Type(QEvent::registerEventType());


boost::shared_ptr<Events_Message> PostponeMessageQtEvent::postponedMessage()
{
  return myMessage;
}

