// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_ModelWidget.cpp
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "ModuleBase_ModelWidget.h"
#include "ModuleBase_IPropertyPanel.h"
#include "ModuleBase_ViewerPrs.h"
#include "ModuleBase_Tools.h"
#include "ModuleBase_WidgetValidator.h"

#include <Events_InfoMessage.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>
#include <Config_Translator.h>
#include <Config_PropManager.h>

#include <Events_Loop.h>

#include <QEvent>
#include <QLabel>
#include <QFocusEvent>
#include <QTextCodec>

//#define DEBUG_VALUE_STATE

//#define DEBUG_WIDGET_INSTANCE
//#define DEBUG_ENABLE_SKETCH_INPUT_FIELDS

ModuleBase_ModelWidget::ModuleBase_ModelWidget(QWidget* theParent,
                                               const Config_WidgetAPI* theData)
    : QWidget(theParent),
      myIsEditing(false),
      myState(Stored),
      myIsValueStateBlocked(false),
      myFlushUpdateBlocked(false),
      myWidgetValidator(0)
{
#ifdef DEBUG_WIDGET_INSTANCE
  qDebug("ModuleBase_ModelWidget::ModuleBase_ModelWidget");
#endif

  myFeatureId = theData->featureId();

  myIsInternal = theData->getBooleanAttribute(ATTR_INTERNAL, false);

  myIsModifiedInEdit = theData->getProperty(ATTR_MODIFIED_IN_EDIT);

  myDefaultValue = theData->getProperty(ATTR_DEFAULT);
  myUseReset = theData->getBooleanAttribute(ATTR_USE_RESET, true);
  myIsComputedDefault = theData->getProperty(ATTR_DEFAULT) == DOUBLE_WDG_DEFAULT_COMPUTED;
  myAttributeID = theData ? theData->widgetId() : "";
  myIsObligatory = theData->getBooleanAttribute(ATTR_OBLIGATORY, true);

  myIsValueEnabled = On; // not defined or "true"
  std::string anEnableValue = theData->getProperty(DOUBLE_WDG_ENABLE_VALUE);
  if (anEnableValue == "false")
    myIsValueEnabled = Off;
  if (anEnableValue == DOUBLE_WDG_ENABLE_VALUE_BY_PREFERENCES)
    myIsValueEnabled = DefinedInPreferences;

  connect(this, SIGNAL(valuesChanged()), this, SLOT(onWidgetValuesChanged()));
  connect(this, SIGNAL(valuesModified()), this, SLOT(onWidgetValuesModified()));
}

ModuleBase_ModelWidget::~ModuleBase_ModelWidget()
{
#ifdef DEBUG_WIDGET_INSTANCE
  qDebug("ModuleBase_ModelWidget::~ModuleBase_ModelWidget");
#endif
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

bool ModuleBase_ModelWidget::isValueEnabled() const
{
  bool anEnabled = true;
  if (myIsValueEnabled == DefinedInPreferences) {
#ifdef DEBUG_ENABLE_SKETCH_INPUT_FIELDS
    bool aCanDisable = false;
#else
    //Config_PropManager::boolean(SKETCH_TAB_NAME, "disable_input_fields", "true");
    bool aCanDisable = true;
#endif
    if (aCanDisable)
      anEnabled = false;
  }
  else if (myIsValueEnabled == Off)
    anEnabled = false;
  return anEnabled;
}

void ModuleBase_ModelWidget::processValueState()
{
  if (myState == ModifiedInPP || myState == ModifiedInViewer)
    storeValue();
}

Events_InfoMessage ModuleBase_ModelWidget::getValueStateError() const
{
  Events_InfoMessage aMessage;

  ModuleBase_ModelWidget::ValueState aState = getValueState();
  if (aState != ModuleBase_ModelWidget::Stored) {
    AttributePtr anAttr = feature()->attribute(attributeID());
    if (anAttr.get()) {
      const std::string& anAttributeName = anAttr->id();
      switch (aState) {
        case ModuleBase_ModelWidget::ModifiedInViewer:
          aMessage = "Attribute \"%1\" is locked by modification value in the viewer.";
          aMessage.addParameter(anAttributeName);
          break;
        case ModuleBase_ModelWidget::Reset:
          aMessage = "Attribute \"%1\" is not initialized.";
          aMessage.addParameter(anAttributeName);
          break;
        case ModuleBase_ModelWidget::ModifiedInPP: // Apply should be enabled in this mode
        default:
          break;
      }
    }
  }
  return aMessage;
}

QString ModuleBase_ModelWidget::getError(const bool theValueStateChecked) const
{
  QString anError;

  if (!feature().get())
    return anError;

  std::string aFeatureID = feature()->getKind();
  std::string anAttributeID = attributeID();
  AttributePtr anAttribute = feature()->attribute(anAttributeID);
  if (!anAttribute.get())
    return anError;

  std::string aValidatorID;
  Events_InfoMessage anErrorMsg;

  static ModelAPI_ValidatorsFactory* aValidators = ModelAPI_Session::get()->validators();
  if (!aValidators->validate(anAttribute, aValidatorID, anErrorMsg)) {
    if (anErrorMsg.empty())
      anErrorMsg = "Unknown error.";

    if (anErrorMsg.context().empty()) {
      anErrorMsg.setContext(aFeatureID + ":" + anAttributeID + ":" + aValidatorID);
    }
  }

  if (anErrorMsg.empty() && theValueStateChecked) {
    anErrorMsg = getValueStateError();
  }

  if (!anErrorMsg.empty()) {
    anError = ModuleBase_Tools::translate(anErrorMsg);
  }

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

void ModuleBase_ModelWidget::setFeature(const FeaturePtr& theFeature, const bool theToStoreValue,
                                        const bool isUpdateFlushed)
{
  /// it is possible to give this flag as parameter in storeValue/storeCustomValue
  /// after debug, it may be corrected
  myFlushUpdateBlocked = !isUpdateFlushed;
  myFeature = theFeature;
  if (theToStoreValue) {
    /// it is possible that the attribute is filled before the operation is started,
    /// e.g. by reentrant operation case some attributes are filled by values of
    /// feature of previous operation, we should not lost them here
    if (!theFeature->data()->attribute(attributeID())->isInitialized())
      storeValue();
  }
  myFlushUpdateBlocked = false;
}

bool ModuleBase_ModelWidget::focusTo()
{
#ifdef DEBUG_WIDGET_INSTANCE
  qDebug("ModuleBase_ModelWidget::focusTo");
#endif
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
#ifdef DEBUG_WIDGET_INSTANCE
  qDebug("ModuleBase_ModelWidget::activate");
#endif
  // the control value is stored to the mode by the focus in on the widget
  // we need the value is initialized in order to enable the apply button in the property panel.
  // It should happens in the creation mode only because all fields are filled in the edition mode
  if (!isEditingMode()) {
    AttributePtr anAttribute = myFeature->data()->attribute(myAttributeID);
    if (anAttribute.get() != NULL && !anAttribute->isInitialized())
      initializeValueByActivate();
  }

  if (myWidgetValidator)
    myWidgetValidator->activateFilters(true);

  activateCustom();
}

void ModuleBase_ModelWidget::deactivate()
{
#ifdef DEBUG_WIDGET_INSTANCE
  qDebug("ModuleBase_ModelWidget::deactivate");
#endif
  myIsValueStateBlocked = false;
  myState = Stored;
  if (myWidgetValidator)
    myWidgetValidator->activateFilters(false);
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
  bool isDone = false;
  // value is stored only in creation mode and in edition if there is not
  // XML flag prohibited modification in edit mode(macro feature circle/arc)
  if (!isEditingMode() || isModifiedInEdit().empty())
    isDone = storeValueCustom();
  else {
    /// store value in an alternative attribute if possible(attribute has the same type)
    std::string aWidgetAttribute = attributeID();
    myAttributeID = isModifiedInEdit();
    storeValueCustom();
    myAttributeID = aWidgetAttribute;
    // operation will be restarted but if isDone == true, PagedContainer will try to set focus
    // to the current widget, but will be already deleted
    isDone = false;
  }

  emit afterValuesChanged();

  return isDone;
}
#ifdef DEBUG_VALUE_STATE
std::string getDebugInfo(const ModuleBase_ModelWidget::ValueState& theState)
{
  std::string anInfo;
  switch (theState) {
    case ModuleBase_ModelWidget::Stored:           anInfo = "Stored          "; break;
    case ModuleBase_ModelWidget::ModifiedInPP:     anInfo = "ModifiedInPP    "; break;
    case ModuleBase_ModelWidget::ModifiedInViewer: anInfo = "ModifiedInViewer"; break;
    case ModuleBase_ModelWidget::Reset:            anInfo = "Reset           "; break;
    default: break;
  }
  return anInfo;
}

#endif
ModuleBase_ModelWidget::ValueState ModuleBase_ModelWidget::setValueState
                                         (const ModuleBase_ModelWidget::ValueState& theState)
{
  ValueState aState = myState;

  if (myState != theState && !myIsValueStateBlocked) {
#ifdef DEBUG_VALUE_STATE
    qDebug(QString("setValueState: previous state = %1,\t new state = %2")
           .arg(getDebugInfo(myState).c_str())
           .arg(getDebugInfo(theState).c_str()).toStdString().c_str());
#endif
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

void ModuleBase_ModelWidget::updateObject(ObjectPtr theObject)
{
  if (!myFlushUpdateBlocked) {
#ifdef DEBUG_WIDGET_INSTANCE
    qDebug("ModuleBase_ModelWidget::updateObject");
#endif
    ModuleBase_Tools::flushUpdated(theObject);
    emit objectUpdated();
  }
}

void ModuleBase_ModelWidget::moveObject(ObjectPtr theObj)
{
  //blockUpdateViewer(true);
#ifdef DEBUG_WIDGET_INSTANCE
  qDebug("ModuleBase_ModelWidget::moveObject");
#endif

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
    QFocusEvent* aFocusEvent = dynamic_cast<QFocusEvent*>(theEvent);
    Qt::FocusReason aReason = aFocusEvent->reason();
    bool aMouseOrKey = aReason == Qt::MouseFocusReason ||
                        /*aReason == Qt::TabFocusReason ||
                        //aReason == Qt::BacktabFocusReason ||*/
                        aReason == Qt::OtherFocusReason; // to process widget->setFocus()
    if (aMouseOrKey && getControls().contains(aWidget)) {
    //if (getControls().contains(aWidget)) {
      emitFocusInWidget();
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
QString ModuleBase_ModelWidget::translate(const std::string& theStr) const
{
  return ModuleBase_Tools::translate(context(), theStr);
}

//**************************************************************
ModuleBase_ModelWidget* ModuleBase_ModelWidget::findModelWidget(ModuleBase_IPropertyPanel* theProp,
                                                                QWidget* theWidget)
{
  ModuleBase_ModelWidget* aModelWidget;
  if (!theWidget)
    return aModelWidget;

  QObject* aParent = theWidget->parent();
  while (aParent) {
    aModelWidget = qobject_cast<ModuleBase_ModelWidget*>(aParent);
    if (aModelWidget)
      break;
    aParent = aParent->parent();
  }
  return aModelWidget;
}
