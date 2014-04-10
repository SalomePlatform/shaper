#include "XGUI_Command.h"

#include <QPushButton>

XGUI_Command::XGUI_Command(const QString& theId, QObject * parent)
    : QWidgetAction(parent), myId(theId)
{
}

XGUI_Command::XGUI_Command(const QString& theId, const QIcon& icon, const QString& text,
                           QObject* parent)
    : QWidgetAction(parent), myId(theId)
{
  setIcon(icon);
  setText(text);
}

XGUI_Command::~XGUI_Command()
{
}

QWidget* XGUI_Command::createWidget(QWidget* theParent)
{
  if (theParent->inherits("XGUI_MenuGroupPanel")) {
    QPushButton* aBtn = new QPushButton(theParent);
    aBtn->setIcon(icon());
    aBtn->setText(text());
    aBtn->setStyleSheet("text-align: left");
    QKeySequence aKeys = shortcut();
    QString aToolTip = toolTip();
    if (!aKeys.isEmpty())
      aToolTip = aToolTip + " (" + aKeys.toString() + ")";
    if (!aToolTip.isEmpty())
      aBtn->setToolTip(aToolTip);

    aBtn->addAction(this);
    connect(aBtn, SIGNAL(clicked()), this, SLOT(trigger()));
    aBtn->setFlat(true);
    aBtn->setMinimumSize(MIN_BUTTON_WIDTH, MIN_BUTTON_HEIGHT);
    return aBtn;
  }
  return QWidgetAction::createWidget(theParent);
}

bool XGUI_Command::enabled() const
{
  return isEnabled();
}

void XGUI_Command::enable()
{
  setEnabled(true);
}

void XGUI_Command::disable()
{
  setEnabled(false);
}

void XGUI_Command::connectTo(const QObject* theResiver, const char* theSlot)
{
  connect(this, SIGNAL(triggered()), theResiver, theSlot);
}
