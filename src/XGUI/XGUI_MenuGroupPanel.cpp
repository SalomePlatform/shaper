#include "XGUI_MenuGroupPanel.h"
#include "XGUI_Command.h"

#include <QLayout>
#include <QPushButton>
#include <QAction>
#include <QResizeEvent>

#include <math.h>

XGUI_MenuGroupPanel::XGUI_MenuGroupPanel(QWidget *parent)
    : QWidget(parent), myNewRow(0), myNewCol(0), myMaxRow(1)
{
  myLayout = new QGridLayout(this);
  myLayout->setSpacing(0);
  myLayout->setMargin(0);
  myLayout->setContentsMargins(0, 0, 0, 0);
}

void XGUI_MenuGroupPanel::addCommand(XGUI_Command* theAction)
{
  myActions[theAction] = theAction->requestWidget(this);
  addWidget(myActions[theAction]);
}

void XGUI_MenuGroupPanel::placeWidget(QWidget* theWgt)
{
  if (myMaxRow == myNewRow) {
    myNewRow = 0;
    myNewCol++;
  }
  myLayout->addWidget(theWgt, myNewRow, myNewCol, Qt::AlignLeft);
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
  QListIterator<QWidget*> aIt(myActions.values());
  myNewRow = 0;
  myNewCol = 0;
  while(aIt.hasNext()) {
    placeWidget(aIt.next());
  }
}

XGUI_Command* XGUI_MenuGroupPanel::addFeature(const QString& theId, const QString& theTitle,
                                              const QString& theTip, const QIcon& theIcon,
                                              const QKeySequence& theKeys)
{
  XGUI_Command* aCommand = new XGUI_Command(theId, theIcon, theTitle, this);
  aCommand->setToolTip(theTip);
  if (!theKeys.isEmpty())
    aCommand->setShortcut(theKeys);

  addCommand(aCommand);
  return aCommand;
}
