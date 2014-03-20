#include "XGUI_MainMenu.h"
#include "XGUI_Workbench.h"
#include "XGUI_MainWindow.h"

#include <QLayout>
#include <QTabWidget>
#include <QLabel>
#include <QDockWidget>

XGUI_MainMenu::XGUI_MainMenu(XGUI_MainWindow *parent) :
    QObject(parent), myDesktop(parent)
{
    parent->setTabPosition(Qt::TopDockWidgetArea, QTabWidget::North);
}


XGUI_MainMenu::~XGUI_MainMenu(void)
{
}

XGUI_Workbench* XGUI_MainMenu::addWorkbench(const QString& theTitle)
{
    QDockWidget* aDoc = new QDockWidget(myDesktop);
    QString workbenchObjName = theTitle + "_Workbench";
    aDoc->setObjectName(workbenchObjName);
    aDoc->setFeatures(QDockWidget::DockWidgetVerticalTitleBar);
    aDoc->setAllowedAreas(Qt::TopDockWidgetArea);
    aDoc->setWindowTitle(theTitle);
    aDoc->setMinimumHeight(30);
    aDoc->setContentsMargins(0, 0, 0, 0);

    XGUI_Workbench* aPage = new XGUI_Workbench(aDoc);
    aDoc->setWidget(aPage);

    myDesktop->addDockWidget(Qt::TopDockWidgetArea, aDoc);
    if (myMenuTabs.length() > 1) {
        myDesktop->tabifyDockWidget(myMenuTabs.last(), aDoc);
    }

    myMenuTabs.append(aDoc);
    return aPage;
}

/*
 * Searches for already created workbench with given name.
 */
XGUI_Workbench* XGUI_MainMenu::findWorkbench(const QString& theObjName)
{
  QDockWidget* aDoc = myDesktop->findChild<QDockWidget*>(theObjName);
  if(aDoc) {
    return dynamic_cast<XGUI_Workbench*>(aDoc->widget());
  }
  return NULL;
}


XGUI_MenuGroupPanel* XGUI_MainMenu::addGroup(int thePageId)
{
    XGUI_Workbench* aPage = dynamic_cast<XGUI_Workbench*>(myMenuTabs[thePageId]->widget());
    return aPage->addGroup();
}

