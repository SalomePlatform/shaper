// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ModuleBase_IPropertyPanel.h
 *
 *  Created on: Oct 01, 2014
 *      Author: vsv
 */

#ifndef ModuleBase_PROPERTYPANEL_H_
#define ModuleBase_PROPERTYPANEL_H_

#include "ModuleBase.h"

#include <QDockWidget>
#include <QKeyEvent>

class ModuleBase_ModelWidget;

/**
* \ingroup GUI
* A class for Property panel object definition
*/
class MODULEBASE_EXPORT ModuleBase_IPropertyPanel : public QDockWidget
{
Q_OBJECT
public:
  /// Constructor
  /// \param theParent is a parent of the property panel
  ModuleBase_IPropertyPanel(QWidget* theParent);

  /// Returns header widget
  virtual QWidget* headerWidget() const = 0;

  /// Returns currently active widget
  virtual ModuleBase_ModelWidget* activeWidget() const = 0;

  /// Returns all property panel's widget created by WidgetFactory
  virtual const QList<ModuleBase_ModelWidget*>& modelWidgets() const = 0;

  /// Removes all widgets in the widget area of the property panel
  virtual void cleanContent() = 0;

  /// Editing mode depends on mode of current operation. This value is defined by it.
  /// \param isEditing state of editing mode flag
  virtual void setEditingMode(bool isEditing) { myIsEditing = isEditing; }

  /// \return State of editing mode flag
  bool isEditingMode() const { return myIsEditing; }

  /// Set Enable/Disable state of Cancel button
  /// \param theEnabled Enable/Disable state of Cancel button
  virtual void setCancelEnabled(bool theEnabled) = 0;

  /// \return Enable/Disable state of Cancel button
  virtual bool isCancelEnabled() const = 0;

  /// Returns widget processed by preselection
  virtual ModuleBase_ModelWidget* preselectionWidget() const = 0;

  /// Sets widget processed by preselection
  virtual void setPreselectionWidget(ModuleBase_ModelWidget* theWidget) = 0;

signals:
  /// The signal about key release on the control, that corresponds to the attribute
  /// \param theEvent key release event
  void keyReleased(QKeyEvent* theEvent);

  /// The signal about the widget activation
  /// \param theWidget the activated widget
  void beforeWidgetActivated(ModuleBase_ModelWidget* theWidget);

  /// The signal about the widget activation
  /// \param theWidget the activated widget
  void widgetActivated(ModuleBase_ModelWidget* theWidget);

  /// Emited when there is no next widget
  void noMoreWidgets();

public slots:
  /// Activate the next widget in the property panel
  /// \param theWidget a widget. The next widget should be activated
  virtual void activateNextWidget(ModuleBase_ModelWidget* theWidget) = 0;

  /// Activate the next from current widget in the property panel
  virtual void activateNextWidget() = 0;

  /**
  * Makes the given widget active, highlights it and removes
  * highlighting from the previous active widget
  * emits widgetActivated(theWidget) signal
  * \param theWidget which has to be activated
  */
  virtual void activateWidget(ModuleBase_ModelWidget* theWidget) = 0;

protected:

  /// Flag which shows that current operation is in editing mode
  bool myIsEditing;
};

#endif