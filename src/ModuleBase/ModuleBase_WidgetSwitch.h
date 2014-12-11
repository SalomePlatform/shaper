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
#include <QFrame>

class QComboBox;
class QVBoxLayout;

class MODULEBASE_EXPORT ModuleBase_WidgetSwitch : public QFrame
{
Q_OBJECT
 public:
  ModuleBase_WidgetSwitch(QWidget* parent = NULL);
  virtual ~ModuleBase_WidgetSwitch();

  int addPage(QWidget * theWidget, const QString & theName);
  int count() const;
  int currentIndex() const;
  QWidget * currentWidget() const;
  int indexOf(QWidget * theWidget) const;
  int insertPage(int index, QWidget * theWidget, const QString & theName);
  bool isPageEnabled(int index) const;
  QString pageText(int index) const;
  QString pageToolTip(int index) const;
  void removePage(int index);
  void setPageEnabled(int index, bool enabled);
  void setPageName(int index, const QString & text);
  void setPageToolTip(int index, const QString & toolTip);

 public slots:
  void setCurrentIndex(int index);

signals:
  void currentPageChanged(int);

 protected:
  void refresh();

 private:
  QVBoxLayout* myMainLay;
  QComboBox* myCombo;
  QWidgetList myCases;
};

#endif /* ModuleBase_WidgetSwitch_H_ */
