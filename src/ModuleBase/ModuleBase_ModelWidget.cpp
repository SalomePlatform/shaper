// File:        ModuleBase_ModelWidget.h
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "ModuleBase_ModelWidget.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Session.h>

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <Events_Loop.h>

#include <QEvent>
#include <QWidget>

ModuleBase_ModelWidget::ModuleBase_ModelWidget(QObject* theParent, const Config_WidgetAPI* theData,
                                               const std::string& theParentId)
    : QObject(theParent),
      myParentId(theParentId)
{
  myIsComputedDefault = false;
  myIsObligatory = theData ? theData->getBooleanAttribute(FEATURE_OBLIGATORY, true) : true;
  myAttributeID = theData ? theData->widgetId() : "";
}

bool ModuleBase_ModelWidget::isInitialized(ObjectPtr theObject) const
{
  return theObject->data()->attribute(attributeID())->isInitialized();
}

void ModuleBase_ModelWidget::enableFocusProcessing()
{
  QList<QWidget*> aMyControls = getControls();
  foreach(QWidget*  eachControl, aMyControls) {
    if(!myFocusInWidgets.contains(eachControl)) {
      enableFocusProcessing(eachControl);
    }
  }
}

bool ModuleBase_ModelWidget::focusTo()
{
  QList<QWidget*> aControls = getControls();
  QList<QWidget*>::const_iterator anIt = aControls.begin(), aLast = aControls.end();
  for (; anIt != aLast; anIt++) {
    QWidget* aWidget = *anIt;
    if (aWidget && aWidget->focusPolicy() != Qt::NoFocus) {
      aWidget->setFocus();
      break;
    }
  }
  return true;
}


void ModuleBase_ModelWidget::updateObject(ObjectPtr theObj) const
{
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY);
  ModelAPI_EventCreator::get()->sendUpdated(theObj, anEvent);
}

void ModuleBase_ModelWidget::enableFocusProcessing(QWidget* theWidget)
{
  theWidget->setFocusPolicy(Qt::StrongFocus);
  theWidget->installEventFilter(this);
  myFocusInWidgets.append(theWidget);
}

bool ModuleBase_ModelWidget::eventFilter(QObject* theObject, QEvent *theEvent)
{
  QWidget* aWidget = dynamic_cast<QWidget*>(theObject);
  if (theEvent->type() == QEvent::MouseButtonRelease && 
      myFocusInWidgets.contains(aWidget)) {
    emit focusInWidget(this);
  } 
  // pass the event on to the parent class
  return QObject::eventFilter(theObject, theEvent);
}
