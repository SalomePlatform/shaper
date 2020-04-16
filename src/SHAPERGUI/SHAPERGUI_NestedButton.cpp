// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

/*
 * SHAPERGUI_NestedButton.cpp
 *
 *  Created on: Apr 13, 2015
 *      Author: sbh
 */

#include <SHAPERGUI_NestedButton.h>

#include <QAction>
#include <QFrame>
#include <QHBoxLayout>
#include <QToolButton>
#include <QToolBar>
#include <QEvent>

SHAPERGUI_NestedButton::SHAPERGUI_NestedButton(QObject* theParent,
                                           const QList<QAction*>& theNestedActions)
: QWidgetAction(theParent),
  myNestedActions(theNestedActions),
  myAdditionalButtonsWidget(0),
  myButtonFrame(0),
  myThisButton(0)
{
}

SHAPERGUI_NestedButton::~SHAPERGUI_NestedButton()
{
}

void SHAPERGUI_NestedButton::showAdditionalButtons(bool isShow)
{
  myAdditionalButtonsWidget->setVisible(isShow);
  if (isShow) {
    myButtonFrame->setFrameStyle(QFrame::WinPanel);
    myButtonFrame->setFrameShadow(QFrame::Sunken);
    myThisButton->setAutoRaise(false);
  } else {
    myButtonFrame->setFrameStyle(QFrame::NoFrame);
    myButtonFrame->setFrameShadow(QFrame::Plain);
    myThisButton->setAutoRaise(true);
  }
}

QWidget * SHAPERGUI_NestedButton::createWidget(QWidget * theParent)
{
  // the action has widget only in tool bar, in menu bar, the default
  // action presentation is shown
  QToolBar* aToolBar = dynamic_cast<QToolBar*>(theParent);
  if (!aToolBar)
    return 0;

  myButtonFrame = new QFrame(theParent);
  QHBoxLayout* aBoxLay = new QHBoxLayout(myButtonFrame);
  aBoxLay->setContentsMargins(2, 0, 0, 0);
  aBoxLay->setSpacing(1);
  QSizePolicy aSizePolicy;
  aSizePolicy.setControlType(QSizePolicy::ToolButton);
  myButtonFrame->setSizePolicy(aSizePolicy);

  myThisButton = new QToolButton(myButtonFrame);
  myThisButton->setDefaultAction(this);
  myThisButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  aBoxLay->addWidget(myThisButton, 1);

  myAdditionalButtonsWidget = new QWidget(myButtonFrame);
  QHBoxLayout* aAdditionalBoxLay = new QHBoxLayout(myAdditionalButtonsWidget);
  aAdditionalBoxLay->setContentsMargins(0, 0, 0, 0);
  aAdditionalBoxLay->setSpacing(1);
  foreach (QAction* eachAct, myNestedActions) {
    QToolButton* aButton = new QToolButton(myButtonFrame);
    aButton->setDefaultAction(eachAct);
    aButton->setAutoRaise(true);
    aAdditionalBoxLay->addWidget(aButton);
  }
  myAdditionalButtonsWidget->setLayout(aAdditionalBoxLay);
  aBoxLay->addWidget(myAdditionalButtonsWidget);

  myButtonFrame->setLayout(aBoxLay);

  showAdditionalButtons(false);
  connect(this, SIGNAL(toggled(bool)), this, SLOT(showAdditionalButtons(bool)));
  connect(this, SIGNAL(changed()), this, SLOT(actionStateChanged()));
  return myButtonFrame;
}

bool SHAPERGUI_NestedButton::event(QEvent* theEvent)
{
  if (theEvent->type() == QEvent::ActionChanged) {
    if (myThisButton) {
      myThisButton->setEnabled(isEnabled());
      return true;
    }
  }
  return QWidgetAction::event(theEvent);
}


void SHAPERGUI_NestedButton::actionStateChanged()
{
  if (isEnabled()) {
    QString s = "true";
  } else {
    QString s = "false";
  }

}
