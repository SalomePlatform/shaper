/*
 * NewGeom_NestedButton.cpp
 *
 *  Created on: Apr 13, 2015
 *      Author: sbh
 */

#include <NewGeom_NestedButton.h>

#include <QAction>
#include <QFrame>
#include <QHBoxLayout>
#include <QToolButton>

NewGeom_NestedButton::NewGeom_NestedButton(QObject* theParent,
                                           const QList<QAction*>& theNestedActions)
: QWidgetAction(theParent),
  myNestedActions(theNestedActions),
  myAdditionalButtonsWidget(0),
  myButtonFrame(0),
  myThisButton(0)
{
}

NewGeom_NestedButton::~NewGeom_NestedButton()
{
}

void NewGeom_NestedButton::showAdditionalButtons(bool isShow)
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

QWidget * NewGeom_NestedButton::createWidget(QWidget * theParent)
{
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

void NewGeom_NestedButton::actionStateChanged()
{
  if (isEnabled()) {
    QString s = "true";
  } else {
    QString s = "false";
  }
  
}
