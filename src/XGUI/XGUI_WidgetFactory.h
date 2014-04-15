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
class ModuleBase_Operation;

class XGUI_EXPORT XGUI_WidgetFactory
{
public:
  XGUI_WidgetFactory(ModuleBase_Operation*);
  virtual ~XGUI_WidgetFactory();

  void fillWidget(QWidget* theParent);

protected:
  QWidget* doubleSpinBoxWidget();

  bool connectWidget(QWidget*, const QString&);

private:
  QString qs(const std::string& theStdString) const;

  Config_WidgetAPI* myWidgetApi;
  ModuleBase_Operation*   myOperation;
};

#endif /* XGUI_WIDGETFACTORY_H_ */
