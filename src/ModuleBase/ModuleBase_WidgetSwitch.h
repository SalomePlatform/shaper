// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ModuleBase_WidgetSwitch.h
 *
 *  Created on: Apr 16, 2014
 *      Author: sbh
 */

#ifndef ModuleBase_WidgetSwitch_H_
#define ModuleBase_WidgetSwitch_H_

#include <ModuleBase.h>
#include <ModuleBase_ModelWidget.h>

class QComboBox;
class QVBoxLayout;

/**
* \ingroup GUI
* Implements a model widget for swithch as a container widget. It can be defined in XML with "switch" keyword
*/
class MODULEBASE_EXPORT ModuleBase_WidgetSwitch : public ModuleBase_ModelWidget
{
  Q_OBJECT
 public:
   /// Constructor
   /// \param parent a parent widget
  ModuleBase_WidgetSwitch(QWidget* theParent,
                          const Config_WidgetAPI* theData,
                          const std::string& theParentId);
  virtual ~ModuleBase_WidgetSwitch();

  virtual bool restoreValue() {
    return false;
  }

  virtual QList<QWidget*> getControls() const;

  virtual bool focusTo() {
    return false;
  }

  /// Add a page to the widget
  /// \param theWidget a page widget
  /// \param theName a name of page
  int addPage(QWidget* theWidget, const QString & theName);

  /// Returns count of pages
  int count() const;

  /// Returns index of current page
  int currentIndex() const;

  /// Returns current widget (page)
  QWidget * currentWidget() const;

  /// Returns index of widget (page)
  /// \param theWidget a widget page
  int indexOf(QWidget * theWidget) const;

  /// Insert page
  /// \param index an index (position) to insert 
  /// \param theWidget a page widget
  /// \param theName a name of the page
  int insertPage(int index, QWidget * theWidget, const QString & theName);

  /// Returns True if a page by given index is enabled
  /// \param index index of the page
  bool isPageEnabled(int index) const;

  /// Returns text of the page by its id
  /// \param index index of the page
  QString pageText(int index) const;

  /// Returns tooltip of the page by its id
  /// \param index index of the page
  QString pageToolTip(int index) const;

  /// Remove page by its id
  /// \param index index of the page
  void removePage(int index);

  /// Enale/disable a page by its Id
  /// \param index index of the page
  /// \param enabled an enable flag
  void setPageEnabled(int index, bool enabled);

  /// Set page name
  /// \param index index of the page
  /// \param text a name of the page
  void setPageName(int index, const QString & text);

  /// Set page tooltip
  /// \param index index of the page
  /// \param toolTip a tooltip of the page
  void setPageToolTip(int index, const QString & toolTip);

 public slots:
   /// Set current page by index
  /// \param index index of the page
  void setCurrentIndex(int index);

signals:
  /// Emitted on current page change
  void currentPageChanged(int);

 protected:
  virtual bool storeValueCustom() const {
    return false;
  }
  /// Update widget
  void refresh();

 private:
   /// Layout
  QVBoxLayout* myMainLay;

  /// Combo box
  QComboBox* myCombo;

  /// List of pages
  QWidgetList myCases;
};

#endif /* ModuleBase_WidgetSwitch_H_ */
