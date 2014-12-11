// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ModuleBase_WidgetSwitch.cpp
 *
 *  Created on: Apr 16, 2014
 *      Author: sbh
 */

#include <ModuleBase_WidgetSwitch.h>

#include <QComboBox>
#include <QVBoxLayout>
#include <QSpacerItem>

ModuleBase_WidgetSwitch::ModuleBase_WidgetSwitch(QWidget* parent)
    : QFrame(parent)
{
  myMainLay = new QVBoxLayout(this);
  myMainLay->setContentsMargins(2, 4, 2, 2);
  myCombo = new QComboBox(this);
  myCombo->hide();
  myMainLay->addWidget(myCombo);
  this->setFrameShape(QFrame::StyledPanel);
  connect(myCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setCurrentIndex(int)));
  connect(myCombo, SIGNAL(currentIndexChanged(int)), this, SIGNAL(currentPageChanged(int)));

}

ModuleBase_WidgetSwitch::~ModuleBase_WidgetSwitch()
{
}

int ModuleBase_WidgetSwitch::addPage(QWidget* theWidget, const QString& theName)
{
  return insertPage(count(), theWidget, theName);
}

int ModuleBase_WidgetSwitch::count() const
{
  return myCombo->count();
}

int ModuleBase_WidgetSwitch::currentIndex() const
{
  return myCombo->currentIndex();
}

QWidget* ModuleBase_WidgetSwitch::currentWidget() const
{
  int idx = currentIndex();
  return myCases[idx];
}

int ModuleBase_WidgetSwitch::indexOf(QWidget* theWidget) const
{
  return myCases.indexOf(theWidget);
}

int ModuleBase_WidgetSwitch::insertPage(int theIndex, QWidget* theWidget, const QString& theName)
{
  int index = theIndex < count() ? theIndex : count();
  if (count() == 0)
    myCombo->show();
  myCombo->insertItem(index, theName);
  myCases.insert(index, theWidget);
  myMainLay->addWidget(theWidget);
  setCurrentIndex(theIndex);
  return index;
}

bool ModuleBase_WidgetSwitch::isPageEnabled(int index) const
{
  return myCases[index]->isEnabled();
}

QString ModuleBase_WidgetSwitch::pageText(int index) const
{
  return myCombo->itemText(index);
}

QString ModuleBase_WidgetSwitch::pageToolTip(int index) const
{
  return myCases[index]->toolTip();
}

void ModuleBase_WidgetSwitch::removePage(int index)
{
  myCombo->removeItem(index);
  myCases.removeAt(index);
  if (count() == 0) {
    myCombo->hide();
  }
}

void ModuleBase_WidgetSwitch::setPageEnabled(int index, bool enabled)
{
  myCases[index]->setEnabled(enabled);
}

void ModuleBase_WidgetSwitch::setPageName(int index, const QString& theName)
{
  myCombo->setItemText(index, theName);
}

void ModuleBase_WidgetSwitch::setPageToolTip(int index, const QString& toolTip)
{
  myCases[index]->setToolTip(toolTip);
}

void ModuleBase_WidgetSwitch::setCurrentIndex(int index)
{
  myCombo->setCurrentIndex(index);
  refresh();
}

void ModuleBase_WidgetSwitch::refresh()
{
  foreach(QWidget* eachWidget, myCases)
  {
    eachWidget->setVisible(false);
  }
  if (currentIndex() >= myCases.count())
    return;
  myCases[currentIndex()]->setVisible(true);
}
