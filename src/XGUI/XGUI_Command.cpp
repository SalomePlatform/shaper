#include "XGUI_Command.h"

#include <QPushButton>

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
    QPushButton* aButton = new QPushButton(theParent);
    aButton->setIcon(icon());
    aButton->setText(text());
    aButton->setStyleSheet("text-align: left");
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
    aButton->setFlat(true);
    aButton->setCheckable(myCheckable);
    this->setCheckable(myCheckable);
    aButton->setMinimumSize(MIN_BUTTON_WIDTH, MIN_BUTTON_HEIGHT);

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
