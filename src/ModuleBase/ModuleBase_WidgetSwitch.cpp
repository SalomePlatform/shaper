// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ModuleBase_WidgetSwitch.cpp
 *
 *  Created on: Apr 16, 2014
 *      Author: sbh
 */

#include <ModuleBase_WidgetSwitch.h>
#include <ModuleBase_ModelWidget.h>
#include <ModuleBase_PageBase.h>
#include <ModuleBase_Tools.h>

#include <QComboBox>
#include <QFrame>
#include <QSpacerItem>
#include <QStackedLayout>
#include <QVBoxLayout>

ModuleBase_WidgetSwitch::ModuleBase_WidgetSwitch(QWidget* theParent, const Config_WidgetAPI* theData,
                                                 const std::string& theParentId)
: ModuleBase_PagedContainer(theParent, theData, theParentId)
{
  QVBoxLayout*  aMainLay = new QVBoxLayout(this);
  //aMainLay->setContentsMargins(2, 4, 2, 2);
  ModuleBase_Tools::adjustMargins(aMainLay);
  myCombo = new QComboBox(this);
  myCombo->hide();
  myPagesLayout = new QStackedLayout(this);
  aMainLay->addWidget(myCombo);
  aMainLay->addLayout(myPagesLayout, 1);
  setLayout(aMainLay);

  connect(myCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onPageChanged()));
  connect(myCombo, SIGNAL(activated(int)), myPagesLayout, SLOT(setCurrentIndex(int)));
}

ModuleBase_WidgetSwitch::~ModuleBase_WidgetSwitch()
{
}


int ModuleBase_WidgetSwitch::addPage(ModuleBase_PageBase* thePage, const QString& theName,
                                                                   const QString& theCaseId,
                                                                   const QPixmap& theIcon )
{
  int aSuperCount = ModuleBase_PagedContainer::addPage(thePage, theName, theCaseId, theIcon);
  myCombo->addItem(theName);
  int aResultCount = myCombo->count();
  if (aResultCount == 2)
    myCombo->show();
  QFrame* aFrame = dynamic_cast<QFrame*>(thePage);
  aFrame->setFrameShape(QFrame::Box);
  aFrame->setFrameStyle(QFrame::Sunken);
  myPagesLayout->addWidget(aFrame);
  return aResultCount;
}

int ModuleBase_WidgetSwitch::currentPageIndex() const
{
  int aComboIndex = myCombo->currentIndex();
  return aComboIndex;
}


void ModuleBase_WidgetSwitch::setCurrentPageIndex(int theIndex)
{
  bool isComboSignalsBlocked = myCombo->blockSignals(true);
  bool isPagesLayoutSignalsBlocked = myPagesLayout->blockSignals(true);
  myCombo->setCurrentIndex(theIndex);
  myPagesLayout->setCurrentIndex(theIndex);
  myCombo->blockSignals(isComboSignalsBlocked);
  myPagesLayout->blockSignals(isPagesLayoutSignalsBlocked);
}
