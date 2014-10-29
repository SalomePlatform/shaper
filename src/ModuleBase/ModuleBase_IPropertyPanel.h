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

class MODULEBASE_EXPORT ModuleBase_IPropertyPanel : public QDockWidget
{
Q_OBJECT
public:
  ModuleBase_IPropertyPanel(QWidget* theParent) : QDockWidget(theParent) {}

  /// Returns currently active widget
  virtual ModuleBase_ModelWidget* activeWidget() const = 0;

  /// Returns all property panel's widget created by WidgetFactory
  virtual const QList<ModuleBase_ModelWidget*>& modelWidgets() const = 0;

signals:
  /// The signal about key release on the control, that corresponds to the attribute
  /// \param theEvent key release event
  void keyReleased(QKeyEvent* theEvent);
  /// The signal about the widget activation
  /// \param theWidget the activated widget
  void widgetActivated(ModuleBase_ModelWidget* theWidget);

public slots:
  /// Activate the next widget in the property panel
  /// \param theWidget a widget. The next widget should be activated
  virtual void activateNextWidget(ModuleBase_ModelWidget* theWidget) = 0;

  /// Activate the next from current widget in the property panel
  virtual void activateNextWidget() = 0;
};

#endif