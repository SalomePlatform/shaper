/*
 * ModuleBase_WidgetToolbox.h
 *
 *  Created on: Feb 27, 2015
 *      Author: sbh
 */

#ifndef MODULEBASE_WIDGETTOOLBOX_H_
#define MODULEBASE_WIDGETTOOLBOX_H_

#include <ModuleBase.h>
#include <ModuleBase_PagedContainer.h>

#include <QToolBox>

class ModuleBase_PageBase;

class MODULEBASE_EXPORT ModuleBase_WidgetToolbox : public ModuleBase_PagedContainer
{
  Q_OBJECT
 public:
  ModuleBase_WidgetToolbox(QWidget* theParent, const Config_WidgetAPI* theData,
                           const std::string& theParentId);
  virtual ~ModuleBase_WidgetToolbox();
  /// Overrides ModuleBase_PagedContainer
  int addPage(ModuleBase_PageBase* theWidget,
              const QString& theName, const QString& theCaseId);

 protected:
  /// Implements ModuleBase_PagedContainer
  virtual int currentPageIndex() const;
  /// Implements ModuleBase_PagedContainer
  virtual void setCurrentPageIndex(int);

 private:
  QToolBox* myToolBox;
};

#endif /* MODULEBASE_WIDGETTOOLBOX_H_ */
