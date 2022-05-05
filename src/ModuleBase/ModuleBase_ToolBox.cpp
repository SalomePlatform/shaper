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

#include <ModuleBase_ToolBox.h>
#include <ModuleBase_ModelWidget.h>

#include <QButtonGroup>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>

#include <ModuleBase_PagedContainer.h>

const QString AStyle = "QToolButton:checked {border: 1px solid black; background-color:#C0DCF3}";


ModuleBase_ToolBox::ModuleBase_ToolBox(QWidget* theParent, const bool theUseFrameStyleBox)
: QFrame(theParent)
{
  QVBoxLayout* aMainLayout = new QVBoxLayout(this);
  aMainLayout->setMargin(0);
  aMainLayout->setSpacing(2);

  if (theUseFrameStyleBox) {
    setFrameStyle(QFrame::Box | QFrame::Raised);
    aMainLayout->setMargin(2);
  }

  myButtonsFrame = new QFrame(this);

  myStack = new QStackedWidget(this);

  aMainLayout->addWidget(myButtonsFrame, 0);
  aMainLayout->addWidget(myStack, 1);

  myButtonsGroup = new QButtonGroup(this);
  myButtonsGroup->setExclusive(true);
  myButtonsLayout = new QHBoxLayout(myButtonsFrame);
  myButtonsLayout->setMargin(0);
  myButtonsLayout->setSpacing(5);
  myButtonsLayout->addStretch(1);

  connect(myStack, SIGNAL(currentChanged(int)), this, SIGNAL(currentChanged(int)));
  connect(myButtonsGroup, SIGNAL(buttonPressed(int)), this, SLOT(onButton(int)));
}

ModuleBase_ToolBox::~ModuleBase_ToolBox()
{
}

void ModuleBase_ToolBox::addItem(QWidget* thePage, const QString& theName, const QPixmap& theIcon)
{
  int anOldCount = myStack->count();

  myStack->addWidget(thePage);

  QToolButton* aButton = new QToolButton(myButtonsFrame);
  aButton->setFocusPolicy(Qt::StrongFocus);
  aButton->setCheckable(true);
  aButton->setStyleSheet(AStyle);
  if (theIcon.isNull())
    aButton->setText(theName);
  else {
    aButton->setIcon(theIcon);
    aButton->setIconSize(theIcon.size());
  }
  aButton->setToolTip(theName);
  aButton->setObjectName(theName);
  myButtonsGroup->addButton(aButton, anOldCount);
  myButtonsLayout->insertWidget(anOldCount, aButton);
}

int ModuleBase_ToolBox::count() const
{
  return myStack->count();
}

int ModuleBase_ToolBox::currentIndex() const
{
  return myStack->currentIndex();
}

void ModuleBase_ToolBox::setCurrentIndex(const int theIndex)
{
  myStack->setCurrentIndex(theIndex);
  myButtonsGroup->button(theIndex)->setChecked(true);
}

void ModuleBase_ToolBox::onButton(int theIndex)
{
  myStack->setCurrentIndex(theIndex);
}

bool ModuleBase_ToolBox::isOffToolBoxParent(ModuleBase_ModelWidget* theWidget)
{
  bool isOffToolBox = false;

  QList<QWidget*> aControls = theWidget->getControls();
  if (aControls.size() > 0) {
    QWidget* aFirstControl = aControls.first();

    QWidget* aWidget = aFirstControl;
    QWidget* aParent = (QWidget*)aFirstControl->parent();
    while (aParent) {
      QStackedWidget* aStackedWidget = dynamic_cast<QStackedWidget*>(aParent);
      if (aStackedWidget) {
        isOffToolBox = aStackedWidget->currentWidget() != aWidget;
        break;
      }
      aWidget = aParent;
      aParent = (QWidget*)aWidget->parent();
    }
  }
  return isOffToolBox;
}
