/*
 * ModuleBase_WidgetToolbox.h
 *
 *  Created on: Feb 27, 2015
 *      Author: sbh
 */

#ifndef MODULEBASE_WIDGETTOOLBOX_H_
#define MODULEBASE_WIDGETTOOLBOX_H_

#include <ModuleBase_ModelWidget.h>

#include <QToolBox>

class ModuleBase_PageBase;

class ModuleBase_WidgetToolbox : public ModuleBase_ModelWidget
{
  Q_OBJECT
 public:
  ModuleBase_WidgetToolbox(QWidget* theParent, const Config_WidgetAPI* theData,
                           const std::string& theParentId);
  virtual ~ModuleBase_WidgetToolbox();

  virtual bool restoreValue();
  virtual QList<QWidget*> getControls() const;
  virtual bool focusTo();

  int addPage(QWidget* theWidget, const QString& theName, const QString& theCaseId);

 protected:
  virtual bool storeValueCustom() const;

 protected slots:
  void onPageChanged();

 private:
  QToolBox* myToolBox;
  QStringList myCaseIds;
};

#endif /* MODULEBASE_WIDGETTOOLBOX_H_ */
