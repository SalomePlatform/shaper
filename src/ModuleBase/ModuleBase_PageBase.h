/*
 * ModuleBase_PageBase.h
 *
 *  Created on: Mar 4, 2015
 *      Author: sbh
 */

#ifndef MODULEBASE_PAGEBASE_H_
#define MODULEBASE_PAGEBASE_H_

#include <ModuleBase.h>
#include <ModuleBase_Tools.h>

class ModuleBase_ModelWidget;
class QLayout;
class QWidget;

/*!
 * Represent a property panel's list of ModuleBase_ModelWidgets.
 */
class MODULEBASE_EXPORT ModuleBase_PageBase
{
 public:
  ModuleBase_PageBase();
  virtual ~ModuleBase_PageBase();
  QWidget* pageWidget();

  void addModelWidget(ModuleBase_ModelWidget* theWidget);
  void addPageWidget(ModuleBase_PageBase* theWidget);

  void clearPage();
  bool takeFocus();
  QList<ModuleBase_ModelWidget*> modelWidgets();
  void alignToTop();

 protected:
  virtual void placeModelWidget(ModuleBase_ModelWidget* theWidget) = 0;
  virtual void placePageWidget(ModuleBase_PageBase* theWidget) = 0;
  virtual QLayout* pageLayout() = 0;
  virtual void addPageStretch() = 0;

 private:
  QList<ModuleBase_ModelWidget*> myWidgetList;

};

#endif /* MODULEBASE_PAGEBASE_H_ */
