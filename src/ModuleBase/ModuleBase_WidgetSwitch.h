// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ModuleBase_WidgetSwitch.h
 *
 *  Created on: Apr 16, 2014
 *      Author: sbh
 */

#ifndef MODULEBASE_WIDGETSWITCH_H_
#define MODULEBASE_WIDGETSWITCH_H_

#include <ModuleBase.h>
#include <ModuleBase_PagedContainer.h>

class QComboBox;
class QStackedLayout;

/**
* \ingroup GUI
* Implements a model widget for switch as a container widget. It can be defined in XML with "switch" keyword
*/
class MODULEBASE_EXPORT ModuleBase_WidgetSwitch : public ModuleBase_PagedContainer
{
  Q_OBJECT
 public:
   /// Constructor
   /// \param parent a parent widget
  ModuleBase_WidgetSwitch(QWidget* theParent,
                          const Config_WidgetAPI* theData,
                          const std::string& theParentId);
  virtual ~ModuleBase_WidgetSwitch();

  /// Defines if it is supported to set the value in this widget
  /// It returns false because this is an info widget
  virtual bool canSetValue() const { return false; };

  /// Add a page to the widget
  /// \param theWidget a page widget
  /// \param theName a name of page
  virtual int addPage( ModuleBase_PageBase* theWidget,
                       const QString& theName,
                       const QString& theCaseId,
                       const QIcon& theIcon );

 protected:
  virtual int currentPageIndex() const;
   /// Set current page by index
  /// \param index index of the page
  virtual void setCurrentPageIndex(int index);

 private:
  /// Combo box
  QComboBox* myCombo;
  QStackedLayout* myPagesLayout;
};

#endif /* ModuleBase_WidgetSwitch_H_ */
