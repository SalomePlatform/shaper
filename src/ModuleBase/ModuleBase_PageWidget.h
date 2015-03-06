/*
 * ModuleBase_PageWidget.h
 *
 *  Created on: Mar 4, 2015
 *      Author: sbh
 */

#ifndef MODULEBASE_PAGEWIDGET_H_
#define MODULEBASE_PAGEWIDGET_H_

#include <ModuleBase.h>
#include <ModuleBase_PageBase.h>

#include <QFrame>
#include <QList>

class ModuleBase_ModelWidget;
class QGridLayout;

/*!
 * Represent a property panel's list of ModuleBase_ModelWidgets.
 */
class MODULEBASE_EXPORT ModuleBase_PageWidget : public QFrame, public ModuleBase_PageBase
{
 public:
  explicit ModuleBase_PageWidget(QWidget* theParent = 0);
  virtual ~ModuleBase_PageWidget();

 protected:
  virtual void placeModelWidget(ModuleBase_ModelWidget* theWidget);
  virtual void placePageWidget(ModuleBase_PageBase* theWidget);
  virtual QLayout* pageLayout();

 private:
  QGridLayout* myMainLayout;
};

#endif /* MODULEBASE_PAGEWIDGET_H_ */
