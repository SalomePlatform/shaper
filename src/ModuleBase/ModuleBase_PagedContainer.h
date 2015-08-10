/*
 * ModuleBase_PagedContainer.h
 *
 *  Created on: Mar 13, 2015
 *      Author: sbh
 */

#ifndef MODULEBASE_PAGEDCONTAINER_H_
#define MODULEBASE_PAGEDCONTAINER_H_

#include <ModuleBase.h>
#include <ModuleBase_ModelWidget.h>

class ModuleBase_PageBase;

class MODULEBASE_EXPORT ModuleBase_PagedContainer : public ModuleBase_ModelWidget
{
  Q_OBJECT
 public:
  ModuleBase_PagedContainer(QWidget* theParent, const Config_WidgetAPI* theData,
                           const std::string& theParentId);
  virtual ~ModuleBase_PagedContainer();

  virtual int addPage( ModuleBase_PageBase* theWidget,
                       const QString& theName,
                       const QString& theCaseId,
                       const QIcon& theIcon );

  // ModuleBase_ModelWidget
  virtual QList<QWidget*> getControls() const;
  virtual bool focusTo();
  virtual void setHighlighted(bool isHighlighted);
  virtual void enableFocusProcessing();

 protected:
  virtual int currentPageIndex() const = 0;
  virtual void setCurrentPageIndex(int ) = 0;
  // ModuleBase_ModelWidget
  virtual void activateCustom();
  virtual bool storeValueCustom() const;
  virtual bool restoreValueCustom();

 protected slots:
  void onPageChanged();

 private:
  bool myIsFocusOnCurrentPage;
  QStringList myCaseIds;
  QList<ModuleBase_PageBase*> myPages;

};

#endif /* MODULEBASE_PAGEDCONTAINER_H_ */
