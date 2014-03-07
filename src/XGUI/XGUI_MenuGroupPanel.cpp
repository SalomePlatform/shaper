#include "XGUI_MenuGroupPanel.h"
#include "XGUI_Command.h"

#include <QLayout>
#include <QPushButton>
#include <QAction>
#include <QResizeEvent>

#include <math.h>

XGUI_MenuGroupPanel::XGUI_MenuGroupPanel(QWidget *parent) :
    QWidget(parent), myNewRow(0), myNewCol(0), myMaxRow(1)
{
    myLayout = new QGridLayout(this);
    myLayout->setSpacing(0);
    myLayout->setMargin(0);
    myLayout->setContentsMargins(0,0,0,0);
}


void XGUI_MenuGroupPanel::addCommand(XGUI_Command* theAction)
{
    addWidget(theAction->requestWidget(this));
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
    myWidgets.append(theWgt);
}


void XGUI_MenuGroupPanel::resizeEvent(QResizeEvent* theEvent)
{
    QWidget::resizeEvent(theEvent);
    if (myWidgets.size() == 0)
        return;

    int aH = theEvent->size().height();
    int aMaxRow = (int) floor(double(aH / MIN_BUTTON_HEIGHT));
    if (aMaxRow == myMaxRow)
        return;

    myMaxRow = aMaxRow;
    QListIterator<QWidget*> aIt(myWidgets);
    myNewRow = 0;
    myNewCol = 0;
    while (aIt.hasNext()) {
       placeWidget(aIt.next());
    }
}
