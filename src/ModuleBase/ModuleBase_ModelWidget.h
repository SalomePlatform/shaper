// Copyright (C) 2014-2023  CEA, EDF
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef MODULEBASE_MODELWIDGET_H
#define MODULEBASE_MODELWIDGET_H

#include <ModuleBase.h>
#include <ModuleBase_ActionType.h>
#include <ModuleBase_Definitions.h>
#include <ModuleBase_OperationFeature.h>
#include <ModuleBase_ActionInfo.h>
#include <ModuleBase_ActionParameter.h>
#include <ModelAPI_Feature.h>

#include <SelectMgr_ListOfFilter.hxx>

#include <QWidget>

#include <memory>

#ifdef _MSC_VER
#pragma warning(disable: 4100)
#endif

class Config_WidgetAPI;
class Events_InfoMessage;
class ModuleBase_IPropertyPanel;
class ModuleBase_IWorkshop;
class ModuleBase_ViewerPrs;
class ModuleBase_WidgetValidator;
class QKeyEvent;

/**\class ModuleBase_ModelWidget
 * \ingroup GUI
 * \brief An abstract custom widget class. This class realization is assumed 
 * to create some controls.
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
                     ModifiedInPP, /// modification has not been finished and set to the model yet
                     ModifiedInViewer, /// modification performed by viewer events
                     Reset }; /// the value is reset

   enum EnableState { On,  /// the widget value is always enabled
                      Off, /// the widget value is always disabled
                      /// the widget value enable state is defined in preferences
                      DefinedInPreferences };

  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuration. The attribute of the model widget is obtained from
  /// a low-level API for reading xml definitions of widgets
  ModuleBase_ModelWidget(QWidget* theParent, const Config_WidgetAPI* theData);
  /// Destructor
  virtual ~ModuleBase_ModelWidget();

  /// Fills the widget with default values. It calls the resetCustom method and change
  /// the widget state to Reset if the reset is performed.
  /// \return true if the widget current value is reset
  bool reset();

  /// Returns the state whether the attribute of the feature is initialized
  /// \param theObject a model feature to be checked
  /// \return the boolean result
  bool isInitialized(ObjectPtr theObject) const;

  /// Fills given container with selection modes if the widget has it
  /// \param [out] theModuleSelectionModes module additional modes, -1 means all default modes
  /// \param [out] theModes a container of modes
  virtual void selectionModes(int& theModuleSelectionModes, QIntList& theModes);

  /// Appends into container of workshop selection filters
  /// \param [out] theModuleSelectionFilters module additional modes, -1 means all default modes
  /// \param [out] theSelectionFilters selection filters
  virtual void selectionFilters(QIntList& theModuleSelectionFilters,
                                SelectMgr_ListOfFilter& theSelectionFilters);

  /// Returns true, if default value of the widget should be computed
  /// on operation's execute, like radius for circle's constraint (can not be zero)
  bool isComputedDefault() const { return myIsComputedDefault; }

  /// Returns true, if default value of the widget is defined in the XML and it is not the
  /// computed value
  /// \return the boolean result
  std::string getDefaultValue() const { return myDefaultValue; }

  /// Returns true, if widget is internal
  /// \return the boolean result
  bool isInternal() const { return myIsInternal; }

  /// Returns true, if the obligatory value of the widget is
  /// not defined in the XML or has true value
  /// \return the boolean result
  bool isObligatory() const { return myIsObligatory; }

  /// Returns true, if the widget value is enabled and might be modified manualy.
  /// It returns false if the application preferences allow having disabled value
  /// and the internal state tells to disable
  /// \return the boolean result
  virtual bool isValueEnabled() const;

  /// Returns this parameter value in the xml file
  /// \return the boolean result
  bool isUseReset() const { return myUseReset; }

  /// Returns this parameter value in the xml file
  /// \return the boolean result
  std::string isModifiedInEdit() const { return myIsModifiedInEdit; }

  /// Returns this widget value state
  /// \return the enumeration result
  ValueState getValueState() const { return myState; }

  /// Stores the widget value if it is modified
  void processValueState();

  /// Returns an attribute error according to the value state
  /// It exists in all cases excepring the "Store" case
  Events_InfoMessage getValueStateError() const;

  /// Defines if it is supposed that the widget should interact with the viewer.
  virtual bool isViewerSelector() { return false; }

  /// Defines if it is supported to set the value in this widget
  /// By default it returns true
  virtual bool canAcceptFocus() const { return true; };

  //! Returns the widget error, get it from the attribute validator and state of the widget
  //! If the feature is correct, it returns an empty value
  //! \param theValueStateChecked the boolean flag if the state of the widget should be checked
  //! \return string value
  virtual QString getError(const bool theValueStateChecked = true) const;

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// \param theValues the wrapped selection values
  /// \param theToValidate the boolean value whether the value should be checked by filters
  virtual bool setSelection(QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues,
                            const bool theToValidate)
  {
    return false;
  }

  /// Returns values which should be highlighted when the whidget is active
  /// \param theValues a list of presentations
  virtual void getHighlighted(QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues) {};

  /// Checks if the selection presentation is valid in widget
  /// \param theValue a selected presentation in the view
  /// \return a boolean value
  virtual bool isValidSelectionCustom(const std::shared_ptr<ModuleBase_ViewerPrs>& theValue)
  { return true; }

  /// Returns widget validator, by default it is NULL. To be created in a child if necessary
  ModuleBase_WidgetValidator* widgetValidator() { return myWidgetValidator; }

  /// Restore value from attribute data to the widget's control.
  /// Emits signals before and after store
  /// \return True in success
  bool restoreValue();

  /// Saves the internal parameters to the given feature. Emits signals before and after store
  /// \return True in success
  bool storeValue();

  /// Set focus to the first control of the current widget.
  /// The focus policy of the control is checked.
  /// If the widget has the NonFocus focus policy, it is skipped.
  /// \return the state whether the widget can accept the focus
  virtual bool focusTo();

  /// Select the internal content if it can be selected. It is empty in the default realization
  virtual void selectContent() {}

  /// The method called when widget is activated
  void activate();

  /// The method called when widget is deactivated
  virtual void deactivate();

  /// Opportunity to do something after the active widget of the property panel changed
  virtual void updateAfterDeactivation() {}
  /// Opportunity to do something after the active widget of the property panel changed
  virtual void updateAfterActivation() {}

  /// The method called if widget should be activated always
  virtual bool needToBeActivated() { return false; }

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const = 0;

  /// Returns the first or the last control that can accept the focus
  /// \param isFirst if true, the first controls is returned or the last one
  /// \return a control from a list of controls
  QWidget* getControlAcceptingFocus(const bool isFirst);

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

  /// \return Current feature
  FeaturePtr feature() const
  {
    return myFeature;
  }

  /// \return Context for translation
  virtual std::string context() const {
    bool isAppendAttr = true;
    std::string aContext = myFeatureId;
    if(!aContext.empty() && !myAttributeID.empty()) {
      // workaround for ModelAPI_Filter's attributes:
      // do not keep attribute name, because it may be changed
      // if the same filter is selected twice or more.
      if (myAttributeID.find(aContext + "__") != std::string::npos)
        isAppendAttr = false;
      else
        aContext += ":";
    }
    if (isAppendAttr)
      aContext += myAttributeID;

    return aContext;
  }

  /// Set feature which is processing by active operation
  /// \param theFeature a feature object
  /// \param theToStoreValue a value about necessity to store the widget value to the feature
  /// \param isUpdateFlushed a flag if update should be flushed on store value
  virtual void setFeature(const FeaturePtr& theFeature, const bool theToStoreValue = false,
                          const bool isUpdateFlushed = true);

  /// Editing mode depends on mode of current operation. This value is defined by it.
  virtual void setEditingMode(bool isEditing) { myIsEditing = isEditing; }

  /// \return Current Editing mode
  bool isEditingMode() const { return myIsEditing; }

  /// Returns true if the action can be processed. By default it is empty and returns false.
  /// \param theActionType an action type
  /// \param isActionEnabled if true, the enable state of the action
  virtual bool canProcessAction(ModuleBase_ActionType theActionType, bool& isActionEnabled);

  /// Returns true if the event is processed. The default implementation is empty, returns false.
  virtual bool processAction(ModuleBase_ActionType theActionType,
                             const ActionParamPtr& theParam = ActionParamPtr());

  /// Returns list of accessible actions for Undo/Redo commands. By default it returns empty list.
  /// \param theActionType type of action. It can be ActionUndo or ActionRedo.
  virtual QList<ActionInfo> actionsList(ModuleBase_ActionType theActionType) const
  { return QList<ActionInfo>(); }

  /// Sends Update and Redisplay for the given object
  /// \param theObj is updating object
  void updateObject(ObjectPtr theObj);

  /// Translate passed string with widget context()
  virtual QString translate(const std::string& theStr) const;

  /// Emit focus in widget to set this control as active in propety panel
  void emitFocusInWidget() { emit focusInWidget(this); }

  /// Finds model widget parent of the given sub widget
  /// \param theWidget a candidate to be a child of the model widget
  /// \param theProp a property panel instance
  /// \return a model widget or NULL
  static ModuleBase_ModelWidget* findModelWidget(ModuleBase_IPropertyPanel* theProp,
                                                 QWidget* theWidget);

  /// Returns True if the widget uses feature attribute.
  /// If not then it means that the widget do not need attribute at all.
  virtual bool usesAttribute() const { return true; }

  /// It is called when user press Ok or OkPlus buttons in the parent property panel
  /// By default this slot does nothing
  virtual void onFeatureAccepted() {}

  /// Returns True in case if the widget contains useful information for inspection tool
  virtual bool isInformative() const { return true; }

  /// If widgets has several panels then this method has to show a page which contains information
  /// for current feature. By default does nothing
  virtual void showInformativePage() {}

  /// Returns True if data of its attribute was modified
  virtual bool isModified() const { return false; }

  virtual void setReadOnly(bool isReadOnly) { setEnabled(!isReadOnly); }

  virtual bool isReadOnly() const { return !isEnabled(); }

  /// Returns true if the widget should have access to external parts
  bool canUseExternalParts() const { return myUseExternalParts; }

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
  /// \param theObject a sender of the event
  /// \param theEvent key release event
  void keyReleased(QObject* theObject, QKeyEvent* theEvent);

  /// The signal is emitted if the enter is clicked in the control of the widget
  /// \param theObject a sender of the event
  void enterClicked(QObject* theObject);

  /// The signal about the widget is get focus
  /// \param theWidget the model base widget
  void focusInWidget(ModuleBase_ModelWidget* theWidget);

  /// The signal about the widget is lost focus
  /// \param theWidget the model base widget
  void focusOutWidget(ModuleBase_ModelWidget* theWidget);

  /// The signal about value state modification
  void valueStateChanged(int theState);

  /// The signal is emitted after flush of updates singal for the widget
  void objectUpdated();

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
  /// If the current value state is Blocked, this method do nothing
  /// \param theState a new state
  /// \return the previous value state
  ValueState setValueState(const ValueState& theState);

  /// Blocks the value state change.
  /// \param theBlocked a block state
  /// \return the previous value
  bool blockValueState(const bool theBlocked);

  /// Compute the feature default value and fill the controls with it
  /// or store the control value to the feature
  virtual void initializeValueByActivate();

  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom() = 0;

  /// Restore value from attribute data to the widget's control
  virtual bool restoreValueCustom() = 0;

  /// Fills the widget with default values
  /// \return true if the widget current value is reset
  virtual bool resetCustom() { return false; };

  /// The method called when widget is activated
  virtual void activateCustom() {};

    //// Returns true if the event is processed. The default implementation is empty, returns false.
  virtual bool processEnter();

  //// Returns true if the event is processed. The default implementation is empty, returns false.
  virtual bool processEscape();

  //// Returns true if the event is processed. The default implementation is empty, returns false.
  virtual bool processDelete();

  /// Returns true if envent is processed. It applyes workshop selection for the widget attribute.
  virtual bool processSelection();

  protected slots:
  /// Processing of values changed in model widget by store the current value to the feature
  void onWidgetValuesChanged();

  /// Changes widget state.
  void onWidgetValuesModified();

 protected:
    /// own validator, by default it is zero
   ModuleBase_WidgetValidator* myWidgetValidator;

  /// The attribute name of the model feature
  std::string myAttributeID;

  /// A feature which is processing by active operation
  FeaturePtr myFeature;

  /// A feature ID
  std::string myFeatureId;

  /// Flag which shows whether current widget is obligatory
  /// The non-obligatory widgets should not accept the focus in the property panel
  bool myIsObligatory;

  /// Flag about value of the control is enabled (can be modified)
  EnableState myIsValueEnabled;

  /// The widget value state
  ValueState myState;

private:
  /// Flag which shows that current operation is in editing mode
  bool myIsEditing;

  /// Value should be computed on execute, like radius for circle's constraint (can not be zero)
  bool myIsComputedDefault;

  /// the default value, which is defined in the XML for this attribute
  std::string myDefaultValue;

  /// an XML internal state
  bool myIsInternal;

  // an XML state, the value is not stored into model if the widget is in edit mode
  std::string myIsModifiedInEdit;

  /// the reset state. If it is false, the reset method of the widget is not performed
  bool myUseReset;
  /// blocked flag of modification of the value state
  bool myIsValueStateBlocked;
  /// do not flush updated signal
  bool myFlushUpdateBlocked;

  bool myUpdateVisualAttributes;

  /// A flag which indicates that current widget should have access to external parts
  bool myUseExternalParts;
};

#endif
