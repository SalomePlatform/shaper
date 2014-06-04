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

#include <QDockWidget>
#include <QList>

class QKeyEvent;

class XGUI_EXPORT XGUI_PropertyPanel: public QDockWidget
{
  Q_OBJECT
public:
  XGUI_PropertyPanel(QWidget* theParent);
  virtual ~XGUI_PropertyPanel();

  QWidget* contentWidget();
  void setModelWidgets(const QList<ModuleBase_ModelWidget*>& theWidgets);

  virtual bool eventFilter(QObject *theObject, QEvent *theEvent);

public slots:
  void updateContentWidget(FeaturePtr theFeature);
  /// slot to set the focus to the widget visualized an attribute with the given name
  /// \param theAttributteName
  void onFocusActivated(const std::string& theAttributeName);

signals:
  /// The signal about key release on the control, that corresponds to the attribute
  /// \param theAttributeName a name of the attribute
  /// \param theEvent key release event
  void keyReleased(const std::string& theAttributeName, QKeyEvent* theEvent);

private:
  QWidget* myCustomWidget;

  QList<ModuleBase_ModelWidget*> myWidgets;
};

#endif /* XGUI_PROPERTYPANEL_H_ */
