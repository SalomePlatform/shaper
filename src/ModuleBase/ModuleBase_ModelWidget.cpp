// File:        ModuleBase_ModelWidget.h
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "ModuleBase_ModelWidget.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_Events.h>

#include "Config_WidgetAPI.h"

#include <Events_Loop.h>

#include <QWidget>

ModuleBase_ModelWidget::ModuleBase_ModelWidget(QObject* theParent, const Config_WidgetAPI* theData,
                                               const std::string& theParentId)
    : QObject(theParent),
      myHasDefaultValue(false),
      myParentId(theParentId)
{
  myAttributeID = theData ? theData->widgetId() : "";
}

bool ModuleBase_ModelWidget::isInitialized(ObjectPtr theObject) const
{
  return theObject->data()->attribute(attributeID())->isInitialized();
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
