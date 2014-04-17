/*
 * XGUI_SwitchWidget.cpp
 *
 *  Created on: Apr 16, 2014
 *      Author: sbh
 */

#include <XGUI_SwitchWidget.h>

#include <QComboBox>
#include <QVBoxLayout>
#include <QSpacerItem>

XGUI_SwitchWidget::XGUI_SwitchWidget(QWidget* parent)
: QFrame(parent)
{
  myMainLay = new QVBoxLayout(this);
  myMainLay->setContentsMargins(2, 4, 2, 2);
  myCombo = new QComboBox(this);
  myCombo->hide();
  myMainLay->addWidget(myCombo);
  this->setFrameShape(QFrame::StyledPanel);
  connect(myCombo, SIGNAL(currentIndexChanged(int)),
          this, SLOT(setCurrentIndex(int)));
  connect(myCombo, SIGNAL(currentIndexChanged(int)),
          this, SIGNAL(currentPageChanged(int)));

}

XGUI_SwitchWidget::~XGUI_SwitchWidget()
{
}

int XGUI_SwitchWidget::addPage(QWidget* theWidget, const QString& theName)
{
  return insertPage(count(), theWidget, theName);
}

int XGUI_SwitchWidget::count() const
{
  return myCombo->count();
}

int XGUI_SwitchWidget::currentIndex() const
{
  return myCombo->currentIndex();
}

QWidget* XGUI_SwitchWidget::currentWidget() const
{
  int idx = currentIndex();
  return myCases[idx];
}

int XGUI_SwitchWidget::indexOf(QWidget* theWidget) const
{
  return myCases.indexOf(theWidget);
}

int XGUI_SwitchWidget::insertPage(int theIndex, QWidget* theWidget, const QString& theName)
{
  int index = theIndex < count() ? theIndex : count();
  if(count() == 0)
    myCombo->show();
  myCombo->insertItem(index, theName);
  myCases.insert(index, theWidget);
  myMainLay->addWidget(theWidget);
  setCurrentIndex(theIndex);
  return index;
}

bool XGUI_SwitchWidget::isPageEnabled(int index) const
{
  return myCases[index]->isEnabled();
}

QString XGUI_SwitchWidget::pageText(int index) const
{
  return myCombo->itemText(index);
}

QString XGUI_SwitchWidget::pageToolTip(int index) const
{
  return myCases[index]->toolTip();
}

void XGUI_SwitchWidget::removePage(int index)
{
  myCombo->removeItem(index);
  myCases.removeAt(index);
  if (count() == 0) {
    myCombo->hide();
  }
}

void XGUI_SwitchWidget::setPageEnabled(int index, bool enabled)
{
  myCases[index]->setEnabled(enabled);
}

void XGUI_SwitchWidget::setPageName(int index, const QString& theName)
{
  myCombo->setItemText(index, theName);
}

void XGUI_SwitchWidget::setPageToolTip(int index, const QString& toolTip)
{
  myCases[index]->setToolTip(toolTip);
}

void XGUI_SwitchWidget::setCurrentIndex(int index)
{
  myCombo->setCurrentIndex(index);
  refresh();
}

void XGUI_SwitchWidget::refresh()
{
  foreach(QWidget* eachWidget, myCases) {
    eachWidget->setVisible(false);
  }
  if(currentIndex() >= myCases.count())
    return;
  myCases[currentIndex()]->setVisible(true);
}
