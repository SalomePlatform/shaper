/*
 * ModuleBase_WidgetFactory.h
 *
 *  Created on: Apr 3, 2014
 *      Author: sbh
 */

#ifndef ModuleBase_WidgetFactory_H_
#define ModuleBase_WidgetFactory_H_

#include <ModuleBase.h>
#include <ModuleBase_ModelWidget.h>

#include <QString>
#include <QList>

class QObject;
class QWidget;
class Config_WidgetAPI;
class ModuleBase_Operation;

class MODULEBASE_EXPORT ModuleBase_WidgetFactory
{
public:
  ModuleBase_WidgetFactory(ModuleBase_Operation*);
  virtual ~ModuleBase_WidgetFactory();

  void createWidget(QWidget* theParent);

  QList<ModuleBase_ModelWidget*> getModelWidgets() const
  {
    return myModelWidgets;
  }

protected:
  //Widgets
  QWidget* createWidgetByType(const std::string& theType, QWidget* theParent = NULL);
  QWidget* labelControl(QWidget* theParent);
  QWidget* doubleSpinBoxControl();
  QWidget* pointSelectorControl(QWidget* theParent);
  QWidget* createContainer(const std::string& theType, QWidget* theParent = NULL);

  bool connectWidget(QObject*, const QString&);
  QString qs(const std::string& theStdString) const;

private:
  Config_WidgetAPI* myWidgetApi;
  ModuleBase_Operation*   myOperation;

  QList<ModuleBase_ModelWidget*> myModelWidgets;
};

#endif /* ModuleBase_WidgetFactory_H_ */
