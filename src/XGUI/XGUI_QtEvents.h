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

  //TODO(mpv): ModelAPI_Document is taken here for example
  //the commented code should be ok when you implement boost::shared_ptr on Events_Message
  //the same for #1-4
  PostponeMessageQtEvent(boost::shared_ptr<ModelAPI_Document> theDoc)
  //PostponeMessageQtEvent(boost::shared_ptr<Events_Message> theMessage)
      : QEvent(PostponeMessageQtEventType),
      //TODO(mpv): #1
      //myMessage(theMessage)
        myTestDoc(theDoc)
  {
  }
  static QEvent::Type type()
  {
    return PostponeMessageQtEventType;
  }

  //TODO(mpv): #2
  //boost::shared_ptr<Events_Message> postponedMessage();
  boost::shared_ptr<ModelAPI_Document> resultDoc();

 private:
  //TODO(mpv): #3
  //boost::shared_ptr<Events_Message> myMessage;
  boost::shared_ptr<ModelAPI_Document> myTestDoc;
};

#endif /* XGUI_QEVENTS_H_ */
