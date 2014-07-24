/*
 * ModuleBase_WidgetFactory.h
 *
 *  Created on: Apr 3, 2014
 *      Author: sbh
 */

#ifndef ModuleBase_WidgetFactory_H_
#define ModuleBase_WidgetFactory_H_

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"

#include <QString>
#include <QList>

class QObject;
class QWidget;
class Config_WidgetAPI;
class ModuleBase_IWorkshop;

class MODULEBASE_EXPORT ModuleBase_WidgetFactory
{
public:
  ModuleBase_WidgetFactory(const std::string& theXmlRepresentation, ModuleBase_IWorkshop* theWorkshop);
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
  QWidget* doubleSpinBoxControl(QWidget* theParent);
  QWidget* pointSelectorControl(QWidget* theParent);
  QWidget* featureSelectorControl(QWidget* theParent);
  QWidget* featureOrAttributeSelectorControl(QWidget* theParent);
  QWidget* doubleValueEditor(QWidget* theParent);
  QWidget* createContainer(const std::string& theType, QWidget* theParent = NULL);
  QWidget* selectorControl(QWidget* theParent);
  QWidget* booleanControl(QWidget* theParent);
  QWidget* point2dDistanceControl(QWidget* theParent);

  /// Check whether the XML definition for the given type contains internal property
  /// \param theType the widget type
  /// \return the boolean result
  bool isInternalWidget(const std::string& theType);

  QString qs(const std::string& theStdString) const;

private:
  Config_WidgetAPI* myWidgetApi;
  ModuleBase_IWorkshop*   myWorkshop;

  QList<ModuleBase_ModelWidget*> myModelWidgets;
  std::string myParentId;
};

#endif /* ModuleBase_WidgetFactory_H_ */
