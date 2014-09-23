#include "XGUI_MenuGroupPanel.h"
#include "XGUI_Command.h"

#include <QLayout>
#include <QPushButton>
#include <QAction>
#include <QResizeEvent>

#include <math.h>
#include <iostream>

XGUI_MenuGroupPanel::XGUI_MenuGroupPanel(QWidget *parent)
    : QFrame(parent),
      myNewRow(0),
      myNewCol(0),
      myMaxRow(1)
{
  myLayout = new QGridLayout(this);
  myLayout->setSpacing(0);
  myLayout->setMargin(0);
  myLayout->setContentsMargins(0, 0, 0, 0);
  setFrameShape(QFrame::NoFrame);
}

void XGUI_MenuGroupPanel::addCommand(XGUI_Command* theAction)
{
  myActions.append(theAction);
  QWidget* aWdg = theAction->requestWidget(this);
  myActionWidget.append(aWdg);
  addWidget(aWdg);
}

void XGUI_MenuGroupPanel::placeWidget(QWidget* theWgt)
{
  if (myMaxRow == myNewRow) {
    myNewRow = 0;
    myNewCol++;
  }
  myLayout->addWidget(theWgt, myNewRow, myNewCol);
  myLayout->setRowStretch(myNewRow, 0);
  myNewRow++;
}

void XGUI_MenuGroupPanel::addWidget(QWidget* theWgt)
{
  placeWidget(theWgt);
}

void XGUI_MenuGroupPanel::resizeEvent(QResizeEvent* theEvent)
{
  QWidget::resizeEvent(theEvent);
  if (myActions.size() == 0)
    return;

  int aH = theEvent->size().height();
  int aMaxRow = (int) floor(double(aH / MIN_BUTTON_HEIGHT));
  if (aMaxRow == myMaxRow)
    return;

  myMaxRow = aMaxRow;
  myNewRow = 0;
  myNewCol = 0;
  foreach(QWidget* eachWidget, myActionWidget)
  {
    placeWidget(eachWidget);
  }
  myLayout->setRowStretch(myMaxRow + 1, 1);
}

XGUI_Command* XGUI_MenuGroupPanel::addFeature(const QString& theId,
                                              const QString& theTip,
                                              const QString& theTitle,
                                              const QIcon& theIcon,
                                              const QKeySequence& theKeys)
{
  return addFeature(theId, theTip, theTitle, theIcon, QString(), theKeys, false);
}

XGUI_Command* XGUI_MenuGroupPanel::addFeature(const QString& theId,
                                              const QString& theTitle,
                                              const QString& theTip,
                                              const QIcon& theIcon,
                                              const QString& theDocumentKind,
                                              const QKeySequence& theKeys,
                                              bool isCheckable)
{
  XGUI_Command* aCommand = new XGUI_Command(theId, theDocumentKind, theIcon,
                                            theTitle, this, isCheckable);
  aCommand->setToolTip(theTip);
  if (!theKeys.isEmpty()) {
    aCommand->setShortcut(theKeys);
  }
  addCommand(aCommand);
  return aCommand;
}

XGUI_Command* XGUI_MenuGroupPanel::feature(const QString& theId) const
{
  foreach (XGUI_Command* aCmd, myActions)
  {
    if (aCmd->data().toString() == theId)
      return aCmd;
  }
  return 0;
}
