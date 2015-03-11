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
  virtual void setHighlighted(bool isHighlighted);
  virtual void enableFocusProcessing();

  int addPage(ModuleBase_PageBase* theWidget,
              const QString& theName, const QString& theCaseId);

 protected:
  virtual void activateCustom();
  virtual bool storeValueCustom() const;

 protected slots:
  void onPageChanged();

 private:
  bool myIsPassFocusToCurrentPage;
  QToolBox* myToolBox;
  QStringList myCaseIds;
  QList<ModuleBase_PageBase*> myPages;
};

#endif /* MODULEBASE_WIDGETTOOLBOX_H_ */
