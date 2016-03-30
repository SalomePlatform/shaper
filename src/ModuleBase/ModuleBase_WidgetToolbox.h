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

class ModuleBase_PageBase;
class ModuleBase_ToolBox;

/**
* \ingroup GUI
* Implements a model widget for switch as a container widget. It can be defined in XML with "toolbox" keyword
*/
class MODULEBASE_EXPORT ModuleBase_WidgetToolbox : public ModuleBase_PagedContainer
{
  Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuration. The attribute of the model widget is obtained from
  ModuleBase_WidgetToolbox(QWidget* theParent, const Config_WidgetAPI* theData);
  virtual ~ModuleBase_WidgetToolbox();

  /// Defines if it is supported to set the value in this widget
  /// \return false because this is an info widget
  virtual bool canAcceptFocus() const { return false; };

  /// Add a page to the widget
  /// \param theWidget a page widget
  /// \param theName a name of page
  /// \param theCaseId an Id of the page
  /// \param theIcon an icon of the page
  virtual int addPage( ModuleBase_PageBase* theWidget,
                       const QString& theName,
                       const QString& theCaseId,
                       const QPixmap& theIcon );

 protected:
  /// Implements ModuleBase_PagedContainer
  virtual int currentPageIndex() const;

  /// Implements ModuleBase_PagedContainer
  virtual void setCurrentPageIndex(int);

 private:
  ModuleBase_ToolBox* myToolBox;
};

#endif /* MODULEBASE_WIDGETTOOLBOX_H_ */
