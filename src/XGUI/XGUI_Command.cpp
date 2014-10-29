#include "XGUI_Command.h"
#include <QEvent>
#include <QToolButton>
#include <QVariant>
#include <QDebug>

XGUI_Command::XGUI_Command(const QString& theId,
                           const QString& theDocumentKind,
                           QObject * parent,
                           bool isCheckable)
    : QWidgetAction(parent),
      myCheckable(isCheckable),
      myDocumentKind(theDocumentKind)
{
  setData(theId);
}

XGUI_Command::XGUI_Command(const QString& theId,
                           const QString& theDocumentKind,
                           const QIcon& icon,
                           const QString& text,
                           QObject* parent,
                           bool isCheckable)
    : QWidgetAction(parent),
      myCheckable(isCheckable),
      myDocumentKind(theDocumentKind)
{
  setIcon(icon);
  setText(text);
  setData(theId);
}

XGUI_Command::~XGUI_Command()
{
}

const QString& XGUI_Command::documentKind() const
{
  return myDocumentKind;
}

QWidget* XGUI_Command::createWidget(QWidget* theParent)
{
  if (theParent->inherits("XGUI_MenuGroupPanel")) {
    QToolButton* aButton = new QToolButton(theParent);
    aButton->setIcon(icon());
    aButton->setText(text());
    QKeySequence aKeys = shortcut();
    QString aToolTip = toolTip();
    if (!aKeys.isEmpty()) {
      aToolTip = QString("%1 (%2)").arg(aToolTip).arg(aKeys.toString());
      aButton->setShortcut(aKeys);
    }
    if (!aToolTip.isEmpty()) {
      aButton->setToolTip(aToolTip);
    }
    aButton->setCheckable(myCheckable);
    aButton->setAutoRaise(true);
    aButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    aButton->setMinimumSize(MIN_BUTTON_WIDTH, MIN_BUTTON_HEIGHT);
    aButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    connect(aButton, SIGNAL(clicked()), this, SLOT(trigger()));
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
