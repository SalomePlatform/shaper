// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

/*
 * XGUI_QEvents.cpp
 *
 *  Created on: Sep 12, 2014
 *      Author: sbh
 */

#include "XGUI_QtEvents.h"

QEvent::Type PostponeMessageQtEvent::PostponeMessageQtEventType = QEvent::Type(QEvent::registerEventType());


std::shared_ptr<Events_Message> PostponeMessageQtEvent::postponedMessage()
{
  return myMessage;
}

