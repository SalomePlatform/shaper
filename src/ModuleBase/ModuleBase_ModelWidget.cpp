// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_ModelWidget.cpp
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
#include <QGraphicsDropShadowEffect>
#include <QColor>
#include <QLabel>
#include <QFocusEvent>

ModuleBase_ModelWidget::ModuleBase_ModelWidget(QWidget* theParent,
                                               const Config_WidgetAPI* theData,
                                               const std::string& theParentId)
    : QWidget(theParent),
      myParentId(theParentId),
      myIsEditing(false),
      myState(Stored)
{
  myDefaultValue = theData->getProperty(ATTR_DEFAULT);
  myUseReset = theData->getBooleanAttribute(ATTR_USE_RESET, true);
  myIsComputedDefault = theData->getProperty(ATTR_DEFAULT) == DOUBLE_WDG_DEFAULT_COMPUTED;
  myAttributeID = theData ? theData->widgetId() : "";
  myIsObligatory = theData->getBooleanAttribute(ATTR_OBLIGATORY, true);

  connect(this, SIGNAL(valuesChanged()), this, SLOT(onWidgetValuesChanged()));
  connect(this, SIGNAL(valuesModified()), this, SLOT(onWidgetValuesModified()));
}

bool ModuleBase_ModelWidget::reset()
{
  bool aResult = resetCustom();
  if (aResult)
    setValueState(Reset);

  return aResult;
}

bool ModuleBase_ModelWidget::isInitialized(ObjectPtr theObject) const
{
  return theObject->data()->attribute(attributeID())->isInitialized();
}

void ModuleBase_ModelWidget::enableFocusProcessing()
{
  QList<QWidget*> aMyControls = getControls();
  foreach(QWidget*  eachControl, aMyControls) {
    if (myIsObligatory) {
      eachControl->setFocusPolicy(Qt::StrongFocus);
      eachControl->installEventFilter(this);
    }
    else {
      eachControl->setFocusPolicy(Qt::NoFocus);
    }
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

void ModuleBase_ModelWidget::setFeature(const FeaturePtr& theFeature, const bool theToStoreValue)
{
  myFeature = theFeature;
  if (theToStoreValue)
    storeValue();
}

bool ModuleBase_ModelWidget::focusTo()
{
  QList<QWidget*> aControls = getControls();
  QList<QWidget*>::const_iterator anIt = aControls.begin(), aLast = aControls.end();
  bool isFocusAccepted = false;
  for (; anIt != aLast && !isFocusAccepted; anIt++) {
    QWidget* aWidget = *anIt;
    if (aWidget && aWidget->focusPolicy() != Qt::NoFocus) {
      aWidget->setFocus();
      isFocusAccepted = true;
    }
  }
  return isFocusAccepted;
}

void ModuleBase_ModelWidget::activate()
{
  // the control value is stored to the mode by the focus in on the widget
  // we need the value is initialized in order to enable the apply button in the property panel.
  // It should happens in the creation mode only because all fields are filled in the edition mode
  if (!isEditingMode()) {
    AttributePtr anAttribute = myFeature->data()->attribute(myAttributeID);
    if (anAttribute.get() != NULL && !anAttribute->isInitialized()) {
      if (isComputedDefault()) {
        if (myFeature->compute(myAttributeID)) {
          restoreValue();
        }
      }
      else {
        storeValue();
      }
    }
  }
  activateCustom();
}

void ModuleBase_ModelWidget::setDefaultValue(const std::string& theValue)
{
  myDefaultValue = theValue;
}

bool ModuleBase_ModelWidget::storeValue()
{
  setValueState(Stored);

  emit beforeValuesChanged();
  bool isDone = storeValueCustom();
  emit afterValuesChanged();

  return isDone;
}

void ModuleBase_ModelWidget::setValueState(const ValueState& theState)
{
  if (myState == theState)
    return;

  myState = theState;
  emit valueStateChanged();
}

bool ModuleBase_ModelWidget::restoreValue()
{
  emit beforeValuesRestored();
  bool isDone = restoreValueCustom();
  emit afterValuesRestored();

  return isDone;
}

void ModuleBase_ModelWidget::storeValueByApply()
{
  // do not emit signal about update the currenty feature object
  // in order to do not perform additional redisplay in the viewer.
  // It should happens by finish operation of the apply action
  storeValueCustom();
}

void ModuleBase_ModelWidget::updateObject(ObjectPtr theObj)
{
  blockUpdateViewer(true);

  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));

  blockUpdateViewer(false);
}

void ModuleBase_ModelWidget::moveObject(ObjectPtr theObj)
{
  //blockUpdateViewer(true);

  static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_MOVED);
  ModelAPI_EventCreator::get()->sendUpdated(theObj, anEvent);
  Events_Loop::loop()->flush(anEvent);

  //blockUpdateViewer(false);
}

bool ModuleBase_ModelWidget::isEventProcessed(QKeyEvent* theEvent)
{
  return false;
}

bool ModuleBase_ModelWidget::eventFilter(QObject* theObject, QEvent *theEvent)
{
  QWidget* aWidget = qobject_cast<QWidget*>(theObject);
  if (theEvent->type() == QEvent::FocusIn) {
    #ifdef _DEBUG
    // The following two lines are for debugging purpose only
    QFocusEvent* aFocusEvent = dynamic_cast<QFocusEvent*>(theEvent);
    bool isWinFocus = aFocusEvent->reason() == Qt::ActiveWindowFocusReason;
    #endif
    if (getControls().contains(aWidget)) {
      emit focusInWidget(this);
    }
  }
  // pass the event on to the parent class

  return QObject::eventFilter(theObject, theEvent);
}

//**************************************************************
void ModuleBase_ModelWidget::onWidgetValuesChanged()
{
  storeValue();
}

//**************************************************************
void ModuleBase_ModelWidget::onWidgetValuesModified()
{
  setValueState(Modified);
}

//**************************************************************
void ModuleBase_ModelWidget::blockUpdateViewer(const bool theValue)
{
  // the viewer update should be blocked in order to avoid the temporary feature content
  // when the solver processes the feature, the redisplay message can be flushed
  // what caused the display in the viewer preliminary states of object
  // e.g. fillet feature, angle value change
  std::shared_ptr<Events_Message> aMsg;
  if (theValue) {
    aMsg = std::shared_ptr<Events_Message>(
        new Events_Message(Events_Loop::eventByName(EVENT_UPDATE_VIEWER_BLOCKED)));
  }
  else {
    // the viewer update should be unblocked
    aMsg = std::shared_ptr<Events_Message>(
        new Events_Message(Events_Loop::eventByName(EVENT_UPDATE_VIEWER_UNBLOCKED)));
  }
  Events_Loop::loop()->send(aMsg);
}
