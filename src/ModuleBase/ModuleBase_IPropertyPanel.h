// Copyright (C) 2014-2024  CEA, EDF
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

  /// Returns currently active widget. This is a widget from internal container of widgets
  /// (myWidgets) activated/deactivated by focus in property panel. If parameter is true,
  /// the method finds firstly the custom widget, after the direct active widget.
  /// \param isUseCustomWidget boolean state if the custom widget might be a result
  virtual ModuleBase_ModelWidget* activeWidget(const bool isUseCustomWidget = false) const = 0;

  /// Returns all property panel's widget created by WidgetFactory
  virtual const QList<ModuleBase_ModelWidget*>& modelWidgets() const = 0;

  /// Returns widget, that has the given attribute index
  /// \param theAttributeId an attribute from XML
  virtual ModuleBase_ModelWidget* modelWidget(const std::string& theAttributeId) const;

  /// Removes all widgets in the widget area of the property panel
  virtual void cleanContent() = 0;

  /// Editing mode depends on mode of current operation. This value is defined by it.
  /// \param isEditing state of editing mode flag
  virtual void setEditingMode(bool isEditing) { myIsEditing = isEditing; }

  /// \return State of editing mode flag
  bool isEditingMode() const { return myIsEditing; }

  /// Set focus on the Ok button
  virtual void setFocusOnOkButton() = 0;

  /// Set Enable/Disable state of Cancel button
  /// \param theEnabled Enable/Disable state of Cancel button
  virtual void setCancelEnabled(bool theEnabled) = 0;

  /// \return Enable/Disable state of Cancel button
  virtual bool isCancelEnabled() const = 0;

  /// Returns widget processed by preselection
  virtual ModuleBase_ModelWidget* preselectionWidget() const = 0;

  /// Sets widget processed by preselection
  virtual void setPreselectionWidget(ModuleBase_ModelWidget* theWidget) = 0;

  /// Returns the first widget, where canAcceptFocus returns true
  /// \return a widget or null
  ModuleBase_ModelWidget* findFirstAcceptingValueWidget();

  /// The method is called on accepting of operation
  virtual void onAcceptData() = 0;

  /// Returns True if data of its feature was modified during operation
  virtual bool isModified() const;

  /// Returns the first widget, where canAcceptFocus returns true
  /// \return a widget or null
  static ModuleBase_ModelWidget* findFirstAcceptingValueWidget(
                          const QList<ModuleBase_ModelWidget*>& theWidgets);

signals:
  /// The signal about key release on the control, that corresponds to the attribute
  /// \param theObject a sender of the event
  /// \param theEvent key release event
  void keyReleased(QObject* theObject, QKeyEvent* theEvent);

  /// The signal about the widget activation
  /// \param theWidget the activated widget
  void beforeWidgetActivated(ModuleBase_ModelWidget* theWidget);

  /// The signal about the widget activation
  /// \param theWidget the activated widget
  //void widgetActivated(ModuleBase_ModelWidget* theWidget);

  /// Emited when there is no next widget
  /// \param thePreviousAttributeID an attribute key of the previous active widget
  void noMoreWidgets(const std::string& thePreviousAttributeID);

public slots:
  /// Activate the next widget in the property panel
  /// \param theWidget a widget. The next widget should be activated
  virtual void activateNextWidget(ModuleBase_ModelWidget* theWidget) = 0;

  /// Activate the next from current widget in the property panel
  virtual void activateNextWidget() = 0;

  /**
  * Makes the given widget active, highlights it and removes
  * highlighting from the previous active widget
  * \param theWidget which has to be activated
  * \param theEmitSignal a flag to prohibit signal emit
  */
  virtual void activateWidget(ModuleBase_ModelWidget* theWidget,
                              const bool theEmitSignal = true) = 0;

protected:

  /// Flag which shows that current operation is in editing mode
  bool myIsEditing;
};

#endif