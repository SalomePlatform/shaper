/*
 * XGUI_WidgetFactory.h
 *
 *  Created on: Apr 3, 2014
 *      Author: sbh
 */

#ifndef XGUI_WIDGETFACTORY_H_
#define XGUI_WIDGETFACTORY_H_

#include "XGUI.h"
#include <QString>

class QWidget;
class Config_WidgetAPI;
class ModuleBase_PropPanelOperation;

class XGUI_EXPORT XGUI_WidgetFactory
{
public:
  XGUI_WidgetFactory(ModuleBase_PropPanelOperation*);
  virtual ~XGUI_WidgetFactory();

  void createWidget(QWidget* theParent);

protected:
  //Widgets
  QWidget* createWidgetByType(const std::string& theType, QWidget* theParent = NULL);
  QWidget* labelControl(QWidget* theParent);
  QWidget* doubleSpinBoxControl();
  QWidget* createContainer(const std::string& theType, QWidget* theParent = NULL);

  bool connectWidget(QWidget*, const QString&);
  QString qs(const std::string& theStdString) const;

private:
  Config_WidgetAPI* myWidgetApi;
  ModuleBase_PropPanelOperation*   myOperation;


};

#endif /* XGUI_WIDGETFACTORY_H_ */
