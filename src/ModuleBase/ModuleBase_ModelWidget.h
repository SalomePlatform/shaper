// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_ModelWidget.h
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef MODULEBASE_MODELWIDGET_H
#define MODULEBASE_MODELWIDGET_H

#include <ModuleBase.h>
#include <ModuleBase_ViewerPrs.h>

#include <ModelAPI_Feature.h>

#include <QWidget>

#include <memory>

class Config_WidgetAPI;
class ModuleBase_IWorkshop;
class QKeyEvent;

/**\class ModuleBase_ModelWidget
 * \ingroup GUI
 * \brief An abstract custom widget class. This class realization is assumed to create some controls.
 * The controls values modification should send signal about values change.
 *
 * Common interface for widgets in the property panel.
 * Every widget are able to save/restore data from the model and/or to contain other widgets.
 *
 */
class MODULEBASE_EXPORT ModuleBase_ModelWidget : public QWidget
{
Q_OBJECT
 public:
   /// State of the widget
   enum ValueState { Stored, /// modification is finished and applyed to the model
                     Modified, /// modification has not been finished and set to the model yet
                     Reset };

   /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuration. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent of the current attribute
  ModuleBase_ModelWidget(QWidget* theParent, const Config_WidgetAPI* theData,
                         const std::string& theParentId);
  /// Destructor
  virtual ~ModuleBase_ModelWidget()
  {
  }

  /// Fills the widget with default values. It calls the resetCustom method and change
  /// the widget state to Reset if the reset is performed.
  /// \return true if the widget current value is reset
  bool reset();

  /// Returns the state whether the attribute of the feature is initialized
  /// \param theObject a model feature to be checked
  /// \return the boolean result
  bool isInitialized(ObjectPtr theObject) const;

  /// Returns true, if default value of the widget should be computed
  /// on operation's execute, like radius for circle's constraint (can not be zero)
  bool isComputedDefault() const { return myIsComputedDefault; }

  /// Returns true, if default value of the widget is defined in the XML and it is not the
  /// computed value
  /// \return the boolean result
  std::string getDefaultValue() const { return myDefaultValue; }

  /// Returns true, if the obligatory value of the widget is not defined in the XML or has true value
  /// \return the boolean result
  bool isObligatory() const { return myIsObligatory; }

  /// Returns this parameter value in the xml file
  /// \return the boolean result
  bool isUseReset() const { return myUseReset; }

  /// Returns this widget value state
  /// \return the enumeration result
  ValueState getValueState() const { return myState; }

  /// Defines if it is supposed that the widget should interact with the viewer.
  virtual bool isViewerSelector() { return false; }

  /// Defines if it is supported to set the value in this widget
  /// By default it returns true
  virtual bool canSetValue() const { return true; };

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// \param theValues the wrapped selection values
  /// \param theToValidate the boolean value whether the value should be checked by filters
  virtual bool setSelection(QList<ModuleBase_ViewerPrs>& theValues,
                            const bool theToValidate)
  {
    return false;
  }

  /// Restore value from attribute data to the widget's control. Emits signals before and after store
  /// \return True in success
  bool restoreValue();

  /// Saves the internal parameters to the given feature. Emits signals before and after store
  /// \return True in success
  void storeValueByApply();

  /// Set focus to the first control of the current widget. The focus policy of the control is checked.
  /// If the widget has the NonFocus focus policy, it is skipped.
  /// \return the state whether the widget can accept the focus
  virtual bool focusTo();

  /// The method called when widget is activated
  void activate();

  /// The method called when widget is deactivated
  virtual void deactivate() {}

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const = 0;

  /// FocusIn events processing
  virtual bool eventFilter(QObject* theObject, QEvent *theEvent);

  /// \brief Enables processing of focus event on all controls by the widget
  /// if this widget is not obligatory and set no-focus policy otherwise
  virtual void enableFocusProcessing();

  //! Switch On/Off highlighting of the widget
  virtual void setHighlighted(bool isHighlighted);

  /// Returns the attribute name
  /// \returns the string value
  std::string attributeID() const
  {
    return myAttributeID;
  }

  /// Returns the parent of the attribute
  /// \returns the string value
  std::string parentID() const
  {
    return myParentId;
  }

  /// \return Current feature
  FeaturePtr feature() const
  {
    return myFeature;
  }

  /// Set feature which is processing by active operation
  /// \param theFeature a feature object
  /// \param theToStoreValue a value about necessity to store the widget value to the feature
  void setFeature(const FeaturePtr& theFeature, const bool theToStoreValue = false);

  /// Editing mode depends on mode of current operation. This value is defined by it.
  void setEditingMode(bool isEditing) { myIsEditing = isEditing; }

  /// \return Current Editing mode
  bool isEditingMode() const { return myIsEditing; }

  /// Returns true if the event is processed.
  virtual bool isEventProcessed(QKeyEvent* theEvent);

  /// Sends Update and Redisplay for the given object
  /// \param theObj is updating object
  static void updateObject(ObjectPtr theObj);

  /// Sends Move event for the given object
  /// \param theObj is object for moving
  static void moveObject(ObjectPtr theObj);

signals:
  /// The signal about widget values are to be changed
  void beforeValuesChanged();
  /// The signal about widget values changed
  void valuesChanged();
  /// The signal about widget values modified
  void valuesModified();
  /// The signal about widget values are to be changed
  void afterValuesChanged();

  /// The signal about widget values are to be restored
  void beforeValuesRestored();
  /// The signal about widget values are to be restored
  void afterValuesRestored();

  /// The signal about key release on the control, that corresponds to the attribute
  /// \param theEvent key release event
  void keyReleased(QKeyEvent* theEvent);

  /// The signal about the widget is get focus
  /// \param theWidget the model base widget
  void focusInWidget(ModuleBase_ModelWidget* theWidget);

  /// The signal about the widget is lost focus
  /// \param theWidget the model base widget
  void focusOutWidget(ModuleBase_ModelWidget* theWidget);

  /// The signal about value state modification
  void valueStateChanged();

protected:
  /// Sets default value of widget. Normally, widget should fetch this value
  /// from the xml. However, some widgets derived widgets could define it
  void setDefaultValue(const std::string& theValue);
  /// \brief Set the attribute name
  /// \param theAttribute the string value with attribute name
  void setAttributeID(const std::string& theAttribute)
  {
    myAttributeID = theAttribute;
  }

  /// Sets the current value state. If the value is changed, the signal is emitted
  /// \param theState a new state
  void setValueState(const ValueState& theState);

  /// Saves the internal parameters to the given feature. Emits signals before and after store
  /// \return True in success
  bool storeValue();

  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom() const = 0;

  /// Restore value from attribute data to the widget's control
  virtual bool restoreValueCustom() = 0;

  /// Fills the widget with default values
  /// \return true if the widget current value is reset
  virtual bool resetCustom() { return false; };

  /// The method called when widget is activated
  virtual void activateCustom() {};

  /// Sends a message about block/unblock viewer updating
  /// \param theValue a boolean value
  static void blockUpdateViewer(const bool theValue);

protected slots:
  /// Processing of values changed in model widget by store the current value to the feature
  void onWidgetValuesChanged();

  /// Changes widget state.
  void onWidgetValuesModified();

 protected:

  /// The attribute name of the model feature
  std::string myAttributeID;

  /// Name of parent
  std::string myParentId;

  /// A feature which is processing by active operation
  FeaturePtr myFeature;

  /// Flag which shows that current operation is in editing mode
  bool myIsEditing; 

  /// Flag which shows whether current widget is obligatory
  /// The non-obligatory widgets should not accept the focus in the property panel
  bool myIsObligatory;

  /// The widget value state
  ValueState myState;

private:
  /// Value should be computed on execute, like radius for circle's constraint (can not be zero)
  bool myIsComputedDefault;

  /// the default value, which is defined in the XML for this attribute    
  std::string myDefaultValue;

  /// the reset state. If it is false, the reset method of the widget is not performed
  bool myUseReset;
};

#endif
