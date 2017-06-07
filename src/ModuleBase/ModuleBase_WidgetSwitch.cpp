// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include <ModuleBase_WidgetSwitch.h>
#include <ModuleBase_ModelWidget.h>
#include <ModuleBase_PageBase.h>
#include <ModuleBase_Tools.h>

#include <QComboBox>
#include <QFrame>
#include <QSpacerItem>
#include <QStackedLayout>
#include <QVBoxLayout>

ModuleBase_WidgetSwitch::ModuleBase_WidgetSwitch(QWidget* theParent,
  const Config_WidgetAPI* theData)
: ModuleBase_PagedContainer(theParent, theData)
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
