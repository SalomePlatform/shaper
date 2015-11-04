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
 * \ingroup GUI
 * Represent a property panel's list of ModuleBase_ModelWidgets.
 */
class MODULEBASE_EXPORT ModuleBase_PageGroupBox : public QGroupBox, public ModuleBase_PageBase
{
  Q_OBJECT
 public:
  /// Constructs a page that looks like a QGroupBox
  explicit ModuleBase_PageGroupBox(QWidget* theParent = 0);
  /// Destructs the page
  virtual ~ModuleBase_PageGroupBox();

 protected:
  /// Adds the given widget to page's layout
  virtual void placeModelWidget(ModuleBase_ModelWidget* theWidget);
  /// Adds the given page to page's layout
  virtual void placePageWidget(ModuleBase_PageBase* theWidget);
  /// Returns page's layout (QGridLayout)
  virtual QLayout* pageLayout();
  /// Adds a stretch to page's layout
  virtual void addPageStretch();

 private:
  QGridLayout* myMainLayout; ///< page's layout
};

#endif /* MODULEBASE_PAGEGROUPBOX_H_ */
