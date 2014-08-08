#include <XGUI_MainMenu.h>
#include <XGUI_Workbench.h>
#include <XGUI_MenuGroupPanel.h>
#include <XGUI_MainWindow.h>
#include <XGUI_Command.h>

#include <QLayout>
#include <QTabWidget>
#include <QLabel>
#include <QDockWidget>
#include <QEvent>
#include <QPushButton>
#include <QTabBar>

XGUI_MainMenu::XGUI_MainMenu(XGUI_MainWindow *parent)
    : QWidget(parent), myDesktop(parent)
{
  myGeneralPage = new XGUI_MenuGroupPanel(this);
  myGeneralPage->setObjectName("Default");
  myGeneralPage->parentWidget()->setMaximumWidth(200);
  myGeneralPage->installEventFilter(this);
  myGeneralPage->setFrameStyle(QFrame::StyledPanel);

  myMenuTabs = new QTabWidget(this);
  myMenuTabs->setStyleSheet("QTabBar::tab {height: 24px;} QTabWidget:pane {border: 0px;}");
  QHBoxLayout* aMainLayout = new QHBoxLayout(this);
  aMainLayout->addWidget(myGeneralPage);
  aMainLayout->addWidget(myMenuTabs);
  aMainLayout->setContentsMargins(0, 2, 2, 0);
  aMainLayout->setSpacing(2);
  setLayout(aMainLayout);
  setFixedHeight(menuHeight());
}

XGUI_MainMenu::~XGUI_MainMenu(void)
{
}

XGUI_Workbench* XGUI_MainMenu::addWorkbench(const QString& theId, const QString& theTitle)
{
  QString aTitle = theTitle;
  if (aTitle.isEmpty()) {
    aTitle = tr(theId.toLatin1().constData());
  }
  XGUI_Workbench* aPage = new XGUI_Workbench(myMenuTabs);
  aPage->setObjectName(theId);
  myMenuTabs->addTab(aPage, aTitle);
  return aPage;
}

/*
 * Searches for already created workbench with given name.
 */
XGUI_Workbench* XGUI_MainMenu::findWorkbench(const QString& theObjName) const
{
  return myDesktop->findChild<XGUI_Workbench*>(theObjName);
}


bool XGUI_MainMenu::eventFilter(QObject *theWatched, QEvent *theEvent)
{
  if (theWatched == myGeneralPage) {
    if (theEvent->type() == QEvent::Show) {
      myGeneralPage->parentWidget()->setMaximumWidth(16777215);
      myGeneralPage->removeEventFilter(this);
    }
  }
  return QObject::eventFilter(theWatched, theEvent);
}

void XGUI_MainMenu::insertConsole(QWidget* theConsole)
{
  int aConsoleTabId = myMenuTabs->addTab(theConsole, "Console");

  QTabBar* aTabBar = myMenuTabs->findChild<QTabBar*>();
  QPushButton* aCloseTabButton = new QPushButton();
  aCloseTabButton->setFixedSize(16, 16);
  aCloseTabButton->setIcon(QIcon(":pictures/wnd_close.png"));
  aCloseTabButton->setFlat(true);
  aTabBar->setTabButton(aConsoleTabId,
                        QTabBar::RightSide,
                        aCloseTabButton);

  connect(aCloseTabButton, SIGNAL(clicked()),
          myDesktop, SLOT(dockPythonConsole()));
}

void XGUI_MainMenu::removeConsole()
{
  const int kLastTab = myMenuTabs->count() - 1;
  myMenuTabs->removeTab(kLastTab);
}

XGUI_Command* XGUI_MainMenu::feature(const QString& theId) const
{
  XGUI_Command* result;
  result = myGeneralPage->feature(theId);
  if (!result) {
    for (int aTabIdx = 0; aTabIdx < myMenuTabs->count(); ++aTabIdx) {
      XGUI_Workbench* aWbn = static_cast<XGUI_Workbench*>(myMenuTabs->widget(aTabIdx));
      result = aWbn->feature(theId);
      if (result) break;
    }
  }
  return result;
}

QList<XGUI_Command*> XGUI_MainMenu::features() const
{
  QList<XGUI_Command*> aList = myGeneralPage->features();
  for (int aTabIdx = 0; aTabIdx < myMenuTabs->count(); ++aTabIdx) {
    XGUI_Workbench* aWbn = static_cast<XGUI_Workbench*>(myMenuTabs->widget(aTabIdx));
    aList.append(aWbn->features());
  }
  return aList;
}

QList<XGUI_Workbench*> XGUI_MainMenu::workbenches() const
{
  QList<XGUI_Workbench*> aList;
  for (int aTabIdx = 0; aTabIdx < myMenuTabs->count(); ++aTabIdx) {
    XGUI_Workbench* aWb = dynamic_cast<XGUI_Workbench*>(myMenuTabs->widget(aTabIdx));
    if (aWb) {
      aList.append(aWb);
    }
  }
  return aList;
}

int XGUI_MainMenu::menuItemSize() const
{
  //TODO(sbh, vsv): get this value from the preferences
  static const ItemSize DEFAULT_ITEM_SIZE = Medium;
  return (int) DEFAULT_ITEM_SIZE;
}

int XGUI_MainMenu::menuHeight() const
{
  // Default group has no tabs above --> one extra row
  int rows = menuItemRowsCount() + 1;
  const int kMarginsSpacings = 4;
  return rows * menuItemSize() + kMarginsSpacings;
}

int XGUI_MainMenu::menuItemRowsCount() const
{
  //TODO(sbh, vsv): get this value from the preferences
  static const int DEFAULT_ITEM_ROWS_COUNT = 3;
  return DEFAULT_ITEM_ROWS_COUNT;
}
