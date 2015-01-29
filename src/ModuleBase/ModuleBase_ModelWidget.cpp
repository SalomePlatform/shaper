// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

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
#include <QGraphicsDropShadowEffect>
#include <QColor>
#include <QLabel>

ModuleBase_ModelWidget::ModuleBase_ModelWidget(QWidget* theParent, const Config_WidgetAPI* theData,
                                               const std::string& theParentId)
    : QObject(theParent),
      myParentId(theParentId)
{
  myIsValueDefault = !theData->getProperty(ATTR_DEFAULT).empty();
  myIsComputedDefault = false;
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
    eachControl->setFocusPolicy(Qt::StrongFocus);
    eachControl->installEventFilter(this);
  }
}

void ModuleBase_ModelWidget::setHighlighted(bool isHighlighted)
{
  QList<QWidget*> aWidgetList = getControls();
  foreach(QWidget* aWidget, aWidgetList) {
    QLabel* aLabel = qobject_cast<QLabel*>(aWidget);
    // We won't set the effect to QLabels - it looks ugly
    if(aLabel) continue;
    if(isHighlighted) {
      // If effect is the installed on a different widget, setGraphicsEffect() will
      // remove the effect from the widget and install it on this widget.
      // That's why we create a new effect for each widget
      QGraphicsDropShadowEffect* aGlowEffect = new QGraphicsDropShadowEffect();
      aGlowEffect->setOffset(.0);
      aGlowEffect->setBlurRadius(10.0);
      aGlowEffect->setColor(QColor(0, 170, 255)); // Light-blue color, #00AAFF
      aWidget->setGraphicsEffect(aGlowEffect);
    } else {
      QGraphicsEffect* anEffect = aWidget->graphicsEffect();
      if(anEffect)
        anEffect->deleteLater();
      aWidget->setGraphicsEffect(NULL);
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

void ModuleBase_ModelWidget::activate()
{
  if (!isEditingMode()) {
    // the control value is stored to the mode by the focus in on the widget
    // we need the value is initialized in order to enable the apply button in the property panel
    // it should happens only in the creation mode because during edition all fields are filled
    storeValue();
  }
  activateCustom();
}

void ModuleBase_ModelWidget::updateObject(ObjectPtr theObj) const
{
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY);
  ModelAPI_EventCreator::get()->sendUpdated(theObj, anEvent);
}

void ModuleBase_ModelWidget::moveObject(ObjectPtr theObj) const
{
  static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_MOVED);
  ModelAPI_EventCreator::get()->sendUpdated(theObj, anEvent);
  Events_Loop::loop()->flush(anEvent);
}

bool ModuleBase_ModelWidget::eventFilter(QObject* theObject, QEvent *theEvent)
{
  QWidget* aWidget = qobject_cast<QWidget*>(theObject);
  if (theEvent->type() == QEvent::FocusIn) {
    if (getControls().contains(aWidget)) {
      emit focusInWidget(this);
    }
  } 
  // pass the event on to the parent class

  return QObject::eventFilter(theObject, theEvent);
}
