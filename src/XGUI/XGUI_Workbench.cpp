#include "XGUI_Workbench.h"
#include "XGUI_MenuGroupPanel.h"

#include <QLayout>
#include <QResizeEvent>
#include <QPushButton>


#define SCROLL_STEP 20

//**************************************************
class CommandsArea : public QScrollArea
{
public:
    CommandsArea(QWidget* theParent) : QScrollArea(theParent) {}

protected:
    virtual void resizeEvent(QResizeEvent * theEvent);
};

void CommandsArea::resizeEvent(QResizeEvent* theEvent)
{
    QScrollArea::resizeEvent(theEvent);
    QRect aRect = widget()->childrenRect();
    QSize aNewSize = theEvent->size();
    if (aRect.width() > aNewSize.width())
        aNewSize.setWidth(aRect.width() * 2);
    widget()->resize(aNewSize);
}


//**************************************************
XGUI_Workbench::XGUI_Workbench(QWidget *theParent) :
    QWidget(theParent)
{
    setMinimumHeight(30);
    QHBoxLayout* aMainLayout = new QHBoxLayout(this);
    aMainLayout->setSpacing(0);
    aMainLayout->setMargin(0);
    aMainLayout->setContentsMargins(0,0,0,0);

    myLeftButton = new QPushButton("<", this);
    myLeftButton->setMaximumWidth(14);
    //myLeftButton->setEnabled(false);
    myLeftButton->setVisible(false);
    connect(myLeftButton,SIGNAL(clicked()), this, SLOT(onLeftScroll()));
    aMainLayout->addWidget(myLeftButton);

    myCommandsArea = new CommandsArea(this);
    aMainLayout->addWidget(myCommandsArea);

    myChildWidget = new QWidget(myCommandsArea);
    myCommandsArea->setWidget(myChildWidget);
    myCommandsArea->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    myCommandsArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    myCommandsArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    myLayout = new QHBoxLayout(myChildWidget);
    myLayout->setSpacing(0);
    myLayout->setMargin(0);
    myLayout->setContentsMargins(0,0,0,0);

    myRightButton = new QPushButton(">", this);
    myRightButton->setMaximumWidth(14);
    //myRightButton->setEnabled(false);
    myRightButton->setVisible(false);
    connect(myRightButton,SIGNAL(clicked()), this, SLOT(onRightScroll()));
    aMainLayout->addWidget(myRightButton);

}

int XGUI_Workbench::addGroup()
{
    if (!myLayout->isEmpty()) {
        int aNb = myLayout->count();
        QLayoutItem* aItem = myLayout->itemAt(aNb - 1);
        myLayout->removeItem(aItem);
    }
    XGUI_MenuGroupPanel* aGroup = new XGUI_MenuGroupPanel(myChildWidget);
    myLayout->addWidget(aGroup);
    addSeparator();
    myLayout->addStretch();
    myGroups.append(aGroup);
    return myGroups.size() - 1;
}

void XGUI_Workbench::addSeparator()
{
    QFrame* aLine = new QFrame(myChildWidget);
    aLine->setFrameShape(QFrame::VLine);
    aLine->setFrameShadow(QFrame::Sunken);
    myLayout->addWidget(aLine);
}

void XGUI_Workbench::addCommand(int theGroupId, XGUI_Command* theCommand)
{
    XGUI_MenuGroupPanel* aGroup = myGroups.at(theGroupId);
    aGroup->addCommand(theCommand);
}


void XGUI_Workbench::resizeEvent(QResizeEvent* theEvent)
{
    QWidget::resizeEvent(theEvent);
    QSize aSize = theEvent->size();
    myLeftButton->setMinimumHeight(aSize.height() - 2);
    myRightButton->setMinimumHeight(aSize.height() - 2);

    QSize aS = myChildWidget->sizeHint();
	int aW = myChildWidget->width();
	if (aW < aS.width())
		myChildWidget->resize(aS.width(), myChildWidget->height());

    //myLeftButton->setEnabled(isExceedsLeft());
    //myRightButton->setEnabled(isExceedsRight());
    myLeftButton->setVisible(isExceedsLeft());
    myRightButton->setVisible(isExceedsRight());
}


void XGUI_Workbench::onLeftScroll()
{
    if (!isExceedsLeft())
		return;
    myChildWidget->move( myChildWidget->pos().x() + SCROLL_STEP, 0 );
    myLeftButton->setVisible(isExceedsLeft());
    myRightButton->setVisible(isExceedsRight());
    //myLeftButton->setEnabled(isExceedsLeft());
    //myRightButton->setEnabled(isExceedsRight());
}


void XGUI_Workbench::onRightScroll()
{
    if (!isExceedsRight())
		return;
    myChildWidget->move( myChildWidget->pos().x() - SCROLL_STEP, 0 );
    //myLeftButton->setEnabled(isExceedsLeft());
    //myRightButton->setEnabled(isExceedsRight());
    myLeftButton->setVisible(isExceedsLeft());
    myRightButton->setVisible(isExceedsRight());
}


bool XGUI_Workbench::isExceedsLeft()
{
    QPoint aPos = myChildWidget->pos();
    return (aPos.x() < 0);
}

bool XGUI_Workbench::isExceedsRight()
{
    QPoint aPos = myChildWidget->pos();
    int aVPWidth = myCommandsArea->viewport()->rect().width();
    int aWgtWidth = myChildWidget->childrenRect().width();
    return ((aVPWidth - aPos.x()) < aWgtWidth);
}
