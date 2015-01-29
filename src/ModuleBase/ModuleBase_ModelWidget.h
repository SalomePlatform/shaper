// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_ModelWidget.h
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef MODULEBASE_MODELWIDGET_H
#define MODULEBASE_MODELWIDGET_H

#include <ModuleBase.h>
#include <ModuleBase_ViewerPrs.h>

#include <ModelAPI_Feature.h>

#include <QObject>

#include <memory>

class Config_WidgetAPI;
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
class MODULEBASE_EXPORT ModuleBase_ModelWidget : public QObject
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent of the current attribute
  ModuleBase_ModelWidget(QWidget* theParent, const Config_WidgetAPI* theData,
                         const std::string& theParentId);
  /// Destructor
  virtual ~ModuleBase_ModelWidget()
  {
  }

  /// Returns the state whether the attribute of the feature is initialized
  /// \param theObject a model feature to be checked
  /// \return the boolean result
  bool isInitialized(ObjectPtr theObject) const;

  /// Returns true, if default value of the widget should be computed
  /// on operation's execute, like radius for circle's constraint (can not be zero)
  bool isComputedDefault() { return myIsComputedDefault; }

  /// Returns true, if default value of the widget is defined in the XML and it is not the
  /// computed value
  /// \return the boolean result
  bool isValueDefault() { return myIsValueDefault; }

  /// Defines if it is supposed that the widget should interact with the viewer.
  virtual bool isViewerSelector() { return false; }

  /// Defines if it is supported to set the value in this widget
  /// By default it returns true
  virtual bool canSetValue() const { return true; };

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// \param theValue the wrapped widget value
  virtual bool setSelection(ModuleBase_ViewerPrs theValue)
  {
    return false;
  }

  /// Saves the internal parameters to the given feature
  virtual bool storeValue() const = 0;

  /// Restore value from attribute data to the widget's control
  virtual bool restoreValue() = 0;

  /// Set focus to the first control of the current widget. The focus policy of the control is checked.
  /// If the widget has the NonFocus focus policy, it is skipped.
  /// \return the state whether the widget can accept the focus
  virtual bool focusTo();

  /// The methiod called when widget is activated
  void activate();

  /// The methiod called when widget is deactivated
  virtual void deactivate() {}

  /// Returns the internal parent wiget control, that can be shown anywhere
  /// \returns the widget
  virtual QWidget* getControl() const = 0;

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const = 0;

  /// FocusIn events processing
  virtual bool eventFilter(QObject* theObject, QEvent *theEvent);

  //! \brief Enables processing of focus event on all controls by the widget
  void enableFocusProcessing();

  //! Switch On/Off highlighting of the widget
  void setHighlighted(bool isHighlighted);

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
  void setFeature(const FeaturePtr& theFeature)
  {
    myFeature = theFeature;
  }

  /// Editing mode depends on mode of current operation. This value is defined by it.
  void setEditingMode(bool isEditing) { myIsEditing = isEditing; }

  /// \return Current Editing mode
  bool isEditingMode() const { return myIsEditing; }

signals:
  /// The signal about widget values changed
  void valuesChanged();

  /// The signal about key release on the control, that corresponds to the attribute
  /// \param theEvent key release event
  void keyReleased(QKeyEvent* theEvent);

  /// The signal about the widget is get focus
  /// \param theWidget the model base widget
  void focusInWidget(ModuleBase_ModelWidget* theWidget);

  /// The signal about the widget is lost focus
  /// \param theWidget the model base widget
  void focusOutWidget(ModuleBase_ModelWidget* theWidget);

 protected:
  /// \brief Set the attribute name
  /// \param theAttribute the string value with attribute name
  void setAttributeID(const std::string& theAttribute)
  {
    myAttributeID = theAttribute;
  }

  /// The methiod called when widget is activated
  virtual void activateCustom() {};

  /// Sends Update and Redisplay for the given object
  /// \param theObj is updating object
  void updateObject(ObjectPtr theObj) const;

  /// Sends Move event for the given object
  /// \param theObj is object for moving
  void moveObject(ObjectPtr theObj) const;

 protected:

  /// The attribute name of the model feature
  std::string myAttributeID; 

  /// Name of parent
  std::string myParentId;    

  /// A feature which is processing by active operation
  FeaturePtr myFeature;      

  /// Value should be computed on execute, like radius for circle's constraint (can not be zero)
  bool myIsComputedDefault; 
                        
  /// the default value is defined in the XML for this attribute    
  bool myIsValueDefault;
  /// Flag which shows that current operation is in editing mode
  bool myIsEditing; 
};

#endif
