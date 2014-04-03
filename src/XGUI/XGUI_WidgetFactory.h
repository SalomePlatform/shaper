/*
 * XGUI_WidgetFactory.h
 *
 *  Created on: Apr 3, 2014
 *      Author: sbh
 */

#ifndef XGUI_WIDGETFACTORY_H_
#define XGUI_WIDGETFACTORY_H_

#include <QString>
#include <string>

class QWidget;
class Config_WidgetAPI;

class XGUI_WidgetFactory
{
public:
  XGUI_WidgetFactory(const std::string&);
  virtual ~XGUI_WidgetFactory();

  void fillWidget(QWidget* theParent);

protected:
  QWidget* valueWidget();

private:
  QString qs(const std::string& theStdString) const;

  Config_WidgetAPI* myWidgetApi;
};

#endif /* XGUI_WIDGETFACTORY_H_ */
