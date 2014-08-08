#include "XGUI_Command.h"
#include <QEvent>


XGUI_MenuButton::XGUI_MenuButton(const QIcon& theIcon,
                                 const QString& theText,
                                 QWidget * theParent)
    : QPushButton(theIcon, theText, theParent)

{
  setFlat(true);
  setMinimumSize(MIN_BUTTON_WIDTH, MIN_BUTTON_HEIGHT);
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  QString aStyleSheet = "QPushButton { text-align: left; }";
  //aStyleSheet += "QPushButton:hover { border: 1px solid gray; border-radius: 3px; }";
  setStyleSheet(aStyleSheet);
  installEventFilter(this);
}

//void XGUI_MenuButton::enterEvent(QEvent * event)
//{
//  if(isEnabled()) {
//    setFlat(false);
//  }
//}

//void XGUI_MenuButton::leaveEvent(QEvent * event)
//{
//  setFlat(true);
//}

XGUI_Command::XGUI_Command(const QString& theId, QObject * parent, bool isCheckable)
    : QWidgetAction(parent), myCheckable(isCheckable)
{
  setData(theId);
}

XGUI_Command::XGUI_Command(const QString& theId, const QIcon& icon, const QString& text,
                           QObject* parent, bool isCheckable)
    : QWidgetAction(parent), myCheckable(isCheckable)
{
  setIcon(icon);
  setText(text);
  setData(theId);
}

XGUI_Command::~XGUI_Command()
{
}

QWidget* XGUI_Command::createWidget(QWidget* theParent)
{
  if (theParent->inherits("XGUI_MenuGroupPanel")) {
    XGUI_MenuButton* aButton = new XGUI_MenuButton(icon(), text(), theParent);
    aButton->setCheckable(myCheckable);
    QKeySequence aKeys = shortcut();
    QString aToolTip = toolTip();
    if (!aKeys.isEmpty())
      aToolTip = aToolTip + " (" + aKeys.toString() + ")";
    if (!aToolTip.isEmpty())
      aButton->setToolTip(aToolTip);

    aButton->addAction(this);
    connect(aButton, SIGNAL(clicked()), this, SLOT(trigger()));
    connect(this, SIGNAL(toggled(bool)), aButton, SLOT(setChecked(bool)));
    connect(this, SIGNAL(toggled(bool)), aButton, SLOT(setChecked(bool)));
    this->setCheckable(myCheckable);

    return aButton;
  }
  return QWidgetAction::createWidget(theParent);
}

void XGUI_Command::connectTo(const QObject* theResiver, const char* theSlot)
{
    connect(this, SIGNAL(triggered(bool)), theResiver, theSlot);
}

const QStringList& XGUI_Command::nestedCommands() const
{
  return myNestedCommands;
}

void XGUI_Command::setNestedCommands(const QStringList& myUnblockableCommands)
{
  this->myNestedCommands = myUnblockableCommands;
}
