/*
 * ModuleBase_PageGroupBox.h
 *
 *  Created on: Mar 4, 2015
 *      Author: sbh
 */

#ifndef MODULEBASE_PAGEGROUPBOX_H_
#define MODULEBASE_PAGEGROUPBOX_H_

#include <ModuleBase.h>
#include <ModuleBase_PageBase.h>

#include <QGroupBox>
#include <QList>

class ModuleBase_ModelWidget;
class QGridLayout;


/*!
 * Represent a property panel's list of ModuleBase_ModelWidgets.
 */
class MODULEBASE_EXPORT ModuleBase_PageGroupBox : public QGroupBox, public ModuleBase_PageBase
{
 public:
  explicit ModuleBase_PageGroupBox(QWidget* theParent = 0);
  virtual ~ModuleBase_PageGroupBox();

 protected:
  virtual void placeModelWidget(ModuleBase_ModelWidget* theWidget);
  virtual void placePageWidget(ModuleBase_PageBase* theWidget);
  virtual QLayout* pageLayout();
  virtual void addPageStretch();

 private:
  QGridLayout* myMainLayout;
};

#endif /* MODULEBASE_PAGEGROUPBOX_H_ */
