/*
 * XGUI_PropertyPanel.h
 *
 *  Created on: Apr 29, 2014
 *      Author: sbh
 */

#ifndef XGUI_PROPERTYPANEL_H_
#define XGUI_PROPERTYPANEL_H_

#include "XGUI.h"

#include <ModuleBase_ModelWidget.h>
#include <ModuleBase_IPropertyPanel.h>

#include <QList>

class QKeyEvent;
class QVBoxLayout;

class XGUI_EXPORT XGUI_PropertyPanel : public ModuleBase_IPropertyPanel
{
Q_OBJECT
 public:
  XGUI_PropertyPanel(QWidget* theParent);
  virtual ~XGUI_PropertyPanel();

  /// Returns main widget of the property panel, which children will be created
  /// by WidgetFactory using the XML definition
  QWidget* contentWidget();
  /// Brings back all widget created by widget factory for signal/slot
  /// connections and further processing
  void setModelWidgets(const QList<ModuleBase_ModelWidget*>& theWidgets);
  /// Returns all property panel's widget created by WidgetFactory
  const QList<ModuleBase_ModelWidget*>& modelWidgets() const;
  /// Removes all widgets in the widget area of the property panel
  void cleanContent();

  /// Returns currently active widget
  virtual ModuleBase_ModelWidget* activeWidget() const { return myActiveWidget; }

  /// Activate the next widget in the property panel
  /// \param theWidget a widget. The next widget should be activated
  virtual void activateNextWidget(ModuleBase_ModelWidget* theWidget);

  /// Activate the next from current widget in the property panel
  virtual void activateNextWidget();

 public slots:
  void updateContentWidget(FeaturePtr theFeature);
  // Enables / disables "ok" ("accept") button
  void setAcceptEnabled(bool);

signals:
  /// Signal about the point 2d set to the feature
  /// \param the feature
  /// \param the attribute of the feature
  void storedPoint2D(ObjectPtr theFeature, const std::string& theAttribute);

 private:
  QWidget* myCustomWidget;
  QList<ModuleBase_ModelWidget*> myWidgets;
  QVBoxLayout* myMainLayout;
  ModuleBase_ModelWidget* myActiveWidget;
};

#endif /* XGUI_PROPERTYPANEL_H_ */
