/*
 * XGUI_SwitchWidget.h
 *
 *  Created on: Apr 16, 2014
 *      Author: sbh
 */

#ifndef XGUI_SWITCHWIDGET_H_
#define XGUI_SWITCHWIDGET_H_

#include <XGUI.h>
#include <QFrame>

class QComboBox;
class QVBoxLayout;

class XGUI_EXPORT XGUI_SwitchWidget: public QFrame
{
  Q_OBJECT
public:
  XGUI_SwitchWidget(QWidget* parent = NULL);
  virtual ~XGUI_SwitchWidget();

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

#endif /* XGUI_SWITCHWIDGET_H_ */
