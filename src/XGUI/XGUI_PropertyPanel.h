/*
 * XGUI_PropertyPanel.h
 *
 *  Created on: Apr 29, 2014
 *      Author: sbh
 */

#ifndef XGUI_PROPERTYPANEL_H_
#define XGUI_PROPERTYPANEL_H_

#include <ModuleBase_IModelWidget.h>

#include <QDockWidget>
#include <QList>

class XGUI_PropertyPanel: public QDockWidget
{
  Q_OBJECT
public:
  XGUI_PropertyPanel(QWidget* theParent);
  virtual ~XGUI_PropertyPanel();

  QWidget* contentWidget();
  void setModelWidgets(const QList<ModuleBase_IModelWidget*>& theWidgets);

public slots:
  void updateContentWidget();

private:
  QWidget* myCustomWidget;

  QList<ModuleBase_IModelWidget*> myWidgets;
};

#endif /* XGUI_PROPERTYPANEL_H_ */
