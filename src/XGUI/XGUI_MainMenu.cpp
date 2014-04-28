#include <XGUI_MainMenu.h>
#include <XGUI_Workbench.h>
#include <XGUI_MainWindow.h>
#include <XGUI_Command.h>

#include <QLayout>
#include <QTabWidget>
#include <QLabel>
#include <QDockWidget>
#include <QEvent>

XGUI_MainMenu::XGUI_MainMenu(XGUI_MainWindow *parent)
    : QObject(parent), myDesktop(parent)
{
  parent->setTabPosition(Qt::TopDockWidgetArea, QTabWidget::North);
  myGeneralPage = addWorkbench(tr("General"));
  myGeneralPage->parentWidget()->setMaximumWidth(200);
  myGeneralPage->installEventFilter(this);
}

XGUI_MainMenu::~XGUI_MainMenu(void)
{
}

XGUI_Workbench* XGUI_MainMenu::addWorkbench(const QString& theId, const QString& theTitle)
{
  QDockWidget* aDock = new QDockWidget(myDesktop);
  aDock->setFeatures(QDockWidget::DockWidgetVerticalTitleBar);
  aDock->setAllowedAreas(Qt::TopDockWidgetArea);
  QString aTitle = theTitle;
  if (aTitle.isEmpty()) {
    aTitle = tr(theId.toLatin1().constData());
  }
  aDock->setWindowTitle(aTitle);
  aDock->setMinimumHeight(30);
  aDock->setContentsMargins(0, 0, 0, 0);

  XGUI_Workbench* aPage = new XGUI_Workbench(aDock);
  aPage->setObjectName(theId);
  aDock->setWidget(aPage);

  myDesktop->addDockWidget(Qt::TopDockWidgetArea, aDock);
  if (myMenuTabs.length() > 1) {
    myDesktop->tabifyDockWidget(myMenuTabs.last(), aDock);
  }

  myMenuTabs.append(aDock);
  return aPage;
}

/*
 * Searches for already created workbench with given name.
 */
XGUI_Workbench* XGUI_MainMenu::findWorkbench(const QString& theObjName)
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

XGUI_Command* XGUI_MainMenu::feature(const QString& theId) const
{
  QList<QDockWidget*>::const_iterator aIt;
  for (aIt = myMenuTabs.constBegin(); aIt != myMenuTabs.constEnd(); ++aIt) {
    XGUI_Workbench* aWbn = static_cast<XGUI_Workbench*>((*aIt)->widget());
    XGUI_Command* aCmd = aWbn->feature(theId);
    if (aCmd)
      return aCmd;
  }
  return 0;
}

QList<XGUI_Command*> XGUI_MainMenu::features() const
{
  QList<XGUI_Command*> aList;
  QList<QDockWidget*>::const_iterator aIt;
  for (aIt = myMenuTabs.constBegin(); aIt != myMenuTabs.constEnd(); ++aIt) {
    XGUI_Workbench* aWbn = static_cast<XGUI_Workbench*>((*aIt)->widget());
    aList.append(aWbn->features());
  }
  return aList;
}

