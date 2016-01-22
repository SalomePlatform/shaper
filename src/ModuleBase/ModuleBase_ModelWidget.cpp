// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_ModelWidget.cpp
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "ModuleBase_ModelWidget.h"
#include "ModuleBase_Tools.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <Events_Loop.h>

#include <QEvent>
#include <QLabel>
#include <QFocusEvent>

ModuleBase_ModelWidget::ModuleBase_ModelWidget(QWidget* theParent,
                                               const Config_WidgetAPI* theData,
                                               const std::string& theParentId)
    : QWidget(theParent),
      myParentId(theParentId),
      myIsEditing(false),
      myState(Stored),
      myIsValueStateBlocked(false)
{
  myIsInternal = theData->getBooleanAttribute(ATTR_INTERNAL, false);

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

QString ModuleBase_ModelWidget::getValueStateError() const
{
  QString anError = "";

  ModuleBase_ModelWidget::ValueState aState = getValueState();
  if (aState != ModuleBase_ModelWidget::Stored) {
    AttributePtr anAttr = feature()->attribute(attributeID());
    if (anAttr.get()) {
      QString anAttributeName = anAttr->id().c_str();
      switch (aState) {
        case ModuleBase_ModelWidget::ModifiedInViewer:
          anError = "Attribute \"" + anAttributeName +
                    "\" is locked by modification value in the viewer.";
          break;
        case ModuleBase_ModelWidget::Reset:
          anError = "Attribute \"" + anAttributeName + "\" is not initialized.";
          break;
        case ModuleBase_ModelWidget::ModifiedInPP: // Apply should be enabled in this mode
        default:
          break;
      }
    }
  }
  return anError;
}

QString ModuleBase_ModelWidget::getError() const
{
  QString anError;

  if (!feature().get())
    return anError;

  std::string anAttributeID = attributeID();
  AttributePtr anAttribute = feature()->attribute(anAttributeID);
  if (!anAttribute.get())
    return anError;

  std::string aValidatorID;
  std::string anErrorMsg;

  static ModelAPI_ValidatorsFactory* aValidators = ModelAPI_Session::get()->validators();
  if (!aValidators->validate(anAttribute, aValidatorID, anErrorMsg)) {
    if (anErrorMsg.empty())
      anErrorMsg = "unknown error.";
    anErrorMsg = anAttributeID + " - " + aValidatorID + ": " + anErrorMsg;
  }

  anError = QString::fromStdString(anErrorMsg);
  if (anError.isEmpty())
    anError = getValueStateError();

  return anError;
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
    // If effect is the installed on a different widget, setGraphicsEffect() will
    // remove the effect from the widget and install it on this widget.
    // That's why we create a new effect for each widget
    ModuleBase_Tools::setShadowEffect(aWidget, isHighlighted);
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
      ModuleBase_Tools::setFocus(aWidget, "ModuleBase_ModelWidget::focusTo()");
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
    if (anAttribute.get() != NULL && !anAttribute->isInitialized())
      initializeValueByActivate();
  }
  activateCustom();
}

void ModuleBase_ModelWidget::deactivate()
{
  myIsValueStateBlocked = false;
  if (myState == ModifiedInPP || myState == ModifiedInViewer)
    storeValue();
  myState = Stored;
}

void ModuleBase_ModelWidget::initializeValueByActivate()
{
  if (isComputedDefault()) {
    if (myFeature->compute(myAttributeID)) {
      restoreValue();
    }
  }
  else {
    storeValue();
  }
}

QWidget* ModuleBase_ModelWidget::getControlAcceptingFocus(const bool isFirst)
{
  QWidget* aControl = 0;

  QList<QWidget*> aControls = getControls();
  int aSize = aControls.size();

  if (isFirst) {
    for (int i = 0; i < aSize && !aControl; i++)  {
      if (aControls[i]->focusPolicy() != Qt::NoFocus)
        aControl = aControls[i];
    }
  }
  else {
    for (int i = aSize - 1; i >= 0 && !aControl; i--)  {
      if (aControls[i]->focusPolicy() != Qt::NoFocus)
        aControl = aControls[i];
    }
  }
  return aControl;
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

ModuleBase_ModelWidget::ValueState ModuleBase_ModelWidget::setValueState(const ModuleBase_ModelWidget::ValueState& theState)
{
  ValueState aState = myState;
  if (myState != theState && !myIsValueStateBlocked) {
    myState = theState;
    emit valueStateChanged(aState);
  }
  return aState;
}

bool ModuleBase_ModelWidget::blockValueState(const bool theBlocked)
{
  bool isBlocked = myIsValueStateBlocked;
  myIsValueStateBlocked = theBlocked;
  return isBlocked;
}

bool ModuleBase_ModelWidget::restoreValue()
{
  emit beforeValuesRestored();
  bool isDone = restoreValueCustom();
  emit afterValuesRestored();

  return isDone;
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

bool ModuleBase_ModelWidget::processEnter()
{
  return false;
}

bool ModuleBase_ModelWidget::processDelete()
{
  // we consider that model objects eats delete key in order to
  // do nothing by for example symbol delete in line edit or spin box
  return true;
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
  else if (theEvent->type() == QEvent::FocusOut) {
    QFocusEvent* aFocusEvent = dynamic_cast<QFocusEvent*>(theEvent);

    Qt::FocusReason aReason = aFocusEvent->reason();
    bool aMouseOrKey = aReason == Qt::MouseFocusReason ||
                        aReason == Qt::TabFocusReason ||
                        aReason == Qt::BacktabFocusReason ||
                        aReason == Qt::OtherFocusReason; // to process widget->setFocus()
    if (aMouseOrKey && getControls().contains(aWidget)) {
      if (getValueState() == ModifiedInPP) {
        storeValue();
      }
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
  setValueState(ModifiedInPP);
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
