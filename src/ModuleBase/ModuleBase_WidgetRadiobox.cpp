// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <ModuleBase_WidgetRadiobox.h>
#include <ModuleBase_PageBase.h>

#include <Config_WidgetAPI.h>

#include <QFormLayout>
#include <QRadioButton>
#include <QFrame>
#include <QButtonGroup>

ModuleBase_WidgetRadiobox::ModuleBase_WidgetRadiobox(QWidget* theParent,
                                                     const Config_WidgetAPI* theData)
  : ModuleBase_PagedContainer(theParent, theData)
{
  myLayout = new QFormLayout(this);
  ModuleBase_Tools::adjustMargins(myLayout);
  myGroup = new QButtonGroup(this);
  myGroup->setExclusive(true);

  myVerticalAlignment = theData->getProperty("align_subs").find("vert") == 0;

  connect(myGroup, SIGNAL(buttonToggled(int, bool)), SLOT(onPageChanged()));
}

ModuleBase_WidgetRadiobox::~ModuleBase_WidgetRadiobox()
{
}

int ModuleBase_WidgetRadiobox::addPage(ModuleBase_PageBase* thePage,
                                        const QString& theName,
                                        const QString& theCaseId,
                                        const QPixmap& theIcon,
                                        const QString& theTooltip)
{
  ModuleBase_PagedContainer::addPage(thePage, theName, theCaseId, theIcon, theTooltip);
  QWidget* aWgt = new QWidget(this);
  QVBoxLayout* aLay = new QVBoxLayout(aWgt);
  aLay->setContentsMargins(0, 0, 0, 0);

  QRadioButton* aButton;
  if (theIcon.isNull()) {
    aButton = new QRadioButton(theName, aWgt);
    aButton->setToolTip(translate(theTooltip.toStdString()));
  }
  else {
    aButton = new QRadioButton(aWgt);
    aButton->setToolTip(translate(theName.toStdString()));
  }
  aLay->addStretch();
  aLay->addWidget(aButton);
  aLay->addStretch();

  //QFrame* aFrame = dynamic_cast<QFrame*>(thePage);
  QWidget* aPage = thePage->pageWidget();
  if (myVerticalAlignment) {
    myLayout->addRow(aWgt);
    myLayout->addRow(aPage);
  } else
    myLayout->addRow(aWgt, aPage);
  myGroup->addButton(aButton, myGroup->buttons().count());

  bool isDefault = theCaseId.toStdString() == getDefaultValue();
  aButton->setChecked(isDefault);
  aPage->setEnabled(isDefault);
  connect(aButton, SIGNAL(toggled(bool)), aPage, SLOT(setEnabled(bool)));

  return myGroup->buttons().count();
}

int ModuleBase_WidgetRadiobox::currentPageIndex() const
{
  return myGroup->checkedId();
}

void ModuleBase_WidgetRadiobox::setCurrentPageIndex(int theIndex)
{
  bool isSignalsBlocked = myGroup->blockSignals(true);
  myGroup->button(theIndex)->setChecked(true);
  myGroup->blockSignals(isSignalsBlocked);
}
