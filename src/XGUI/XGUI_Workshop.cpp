#include "XGUI_Workshop.h"
#include "XGUI_MainWindow.h"
#include "XGUI_MainMenu.h"
#include "XGUI_Command.h"
#include "XGUI_Tools.h"

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>

#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif


XGUI_Workshop::XGUI_Workshop() :
    QObject()
{
    myMainWindow = new XGUI_MainWindow();
}

//******************************************************
XGUI_Workshop::~XGUI_Workshop(void)
{
}

//******************************************************
void XGUI_Workshop::startApplication()
{
    initMenu();
    activateModule();
    myMainWindow->show();
}

//******************************************************
void XGUI_Workshop::initMenu()
{
    int aPageId = addWorkbench(tr("HOME_MENU_TITLE"));

    // File commands group
    int aGroupId = addGroup(aPageId);

    XGUI_Command* aCommand;

    //aCommand = createMenuCommand(aPageId, aGroupId, NEW_CMD, tr("NEW_MENU"), tr("NEW_MENU_TIP"),
    //                             QIcon(":pictures/new.png"), QKeySequence::New);
    //connect(aCommand, SIGNAL(triggered()), this, SLOT(onNew()));

    //aCommand = createMenuCommand(aPageId, aGroupId, OPEN_CMD, tr("OPEN_MENU"), tr("OPEN_MENU_TIP"),
    //                             QIcon(":pictures/open.png"), QKeySequence::Open);
    //connect(aCommand, SIGNAL(triggered()), this, SLOT(onOpen()));

    aCommand = createMenuCommand(aPageId, aGroupId, SAVE_CMD, tr("SAVE_MENU"), tr("SAVE_MENU_TIP"),
                                 QIcon(":pictures/save.png"), QKeySequence::Save);
    connect(aCommand, SIGNAL(triggered()), this, SLOT(onSave()));
    aCommand->setEnabled(false);

    aCommand = createMenuCommand(aPageId, aGroupId, SAVEAS_CMD, tr("SAVEAS_MENU"), tr("SAVEAS_MENU_TIP"),
                                 QIcon(":pictures/save.png"));
    connect(aCommand, SIGNAL(triggered()), this, SLOT(onSaveAs()));
    aCommand->setEnabled(false);


    // Edit commands group
    //aGroupId = addGroup(aPageId);

    aCommand = createMenuCommand(aPageId, aGroupId, UNDO_CMD, tr("UNDO_MENU"), tr("UNDO_MENU_TIP"),
                                 QIcon(":pictures/undo.png"), QKeySequence::Undo);

    aCommand = createMenuCommand(aPageId, aGroupId, REDO_CMD, tr("REDO_MENU"), tr("REDO_MENU_TIP"),
                                 QIcon(":pictures/redo.png"), QKeySequence::Redo);

    //aCommand = createMenuCommand(aPageId, aGroupId, CUT_CMD, tr("CUT_MENU"), tr("CUT_MENU_TIP"),
    //                             QIcon(":pictures/cut.png"), QKeySequence::Cut);

    //aCommand = createMenuCommand(aPageId, aGroupId, COPY_CMD, tr("COPY_MENU"), tr("COPY_MENU_TIP"),
    //                             QIcon(":pictures/copy.png"), QKeySequence::Copy);

    //aCommand = createMenuCommand(aPageId, aGroupId, PASTE_CMD, tr("PASTE_MENU"), tr("PASTE_MENU_TIP"),
    //                             QIcon(":pictures/paste.png"), QKeySequence::Paste);

    aCommand = createMenuCommand(aPageId, aGroupId, EXIT_CMD, tr("EXIT_MENU"), tr("EXIT_MENU_TIP"),
                                 QIcon(":pictures/close.png"), QKeySequence::Close);
    connect(aCommand, SIGNAL(triggered()), this, SLOT(onExit()));

    // Tests
    //aPageId = addWorkbench("Primitives");
    //aGroupId = addGroup(aPageId);
    //
    //aCommand = createMenuCommand(aPageId, aGroupId, LAST_CMD, "Box", "Create Box", QIcon(":pictures/box.png"));
    //aCommand = createMenuCommand(aPageId, aGroupId, LAST_CMD, "Cylinder", "Create Cylinder", QIcon(":pictures/cylinder.png"));
    //aCommand = createMenuCommand(aPageId, aGroupId, LAST_CMD, "Disk", "Create Disk", QIcon(":pictures/disk.png"));
    //aCommand = createMenuCommand(aPageId, aGroupId, LAST_CMD, "Torus", "Create Torus", QIcon(":pictures/torus.png"));

    //aPageId = addWorkbench("Operations");

}

//******************************************************
XGUI_Command* XGUI_Workshop::createMenuCommand(int thePageId, int theGroupId, XCommandId theCmdId, 
                                               const QString& theTitle, const QString& theTip, 
                                               const QIcon& theIcon, const QKeySequence& theKeys)
{
    XGUI_Command* aCommand = new XGUI_Command(theIcon, theTitle, this);
    aCommand->setToolTip(theTip);
    if (!theKeys.isEmpty())
        aCommand->setShortcut(theKeys);
    addCommand(theCmdId, thePageId, theGroupId, aCommand);
    return aCommand;
}

//******************************************************
int XGUI_Workshop::addWorkbench(const QString& theName)
{
    XGUI_MainMenu* aMenuBar = myMainWindow->menuObject();
    return aMenuBar->addWorkbench(theName);
}

//******************************************************
int XGUI_Workshop::addGroup(int thePageId)
{
    XGUI_MainMenu* aMenuBar = myMainWindow->menuObject();
    return aMenuBar->addGroup(thePageId);
}

//******************************************************
void XGUI_Workshop::addCommand(XCommandId theCommandId, int thePageId, int theGroupId, XGUI_Command* theCommand)
{
    XGUI_MainMenu* aMenuBar = myMainWindow->menuObject();
    aMenuBar->addCommand(thePageId, theGroupId, theCommand);
    myCommands[theCommandId] = theCommand;
}

//******************************************************
XGUI_Command* XGUI_Workshop::command(XCommandId theId) const
{
    if (myCommands.contains(theId))
        return myCommands[theId];
    return 0;
}

//******************************************************
void XGUI_Workshop::onExit()
{
    qApp->exit();
}

//******************************************************
void XGUI_Workshop::onNew()
{
    myMainWindow->showObjectBrowser();
}

//******************************************************
void XGUI_Workshop::onOpen()
{
    QString aFileName = QFileDialog::getOpenFileName(mainWindow());
}

//******************************************************
void XGUI_Workshop::onSave()
{
}

//******************************************************
void XGUI_Workshop::onSaveAs()
{
    QString aFileName = QFileDialog::getSaveFileName(mainWindow());
}

//******************************************************
IModule* XGUI_Workshop::loadModule(const QString& theModule)
{
  QString libName = library( theModule );
  if ( libName.isEmpty() )
  {
    qWarning( qPrintable( tr( "Information about module \"%1\" doesn't exist." ).arg( theModule ) ) );
    return 0;
  }

  QString err;
  CREATE_FUNC crtInst = 0;

#ifdef WIN32

  HINSTANCE modLib = ::LoadLibrary( (LPTSTR) qPrintable(libName) ); 
  if ( !modLib )
  {
    LPVOID lpMsgBuf;
    ::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                     FORMAT_MESSAGE_IGNORE_INSERTS, 0, ::GetLastError(), 0, (LPTSTR)&lpMsgBuf, 0, 0 );
    QString aMsg((char*)&lpMsgBuf);
    err = QString( "Failed to load  %1. %2" ).arg( libName ).arg( aMsg );
    ::LocalFree( lpMsgBuf );
  }
  else
  {
    crtInst = (CREATE_FUNC)::GetProcAddress( modLib, CREATE_MODULE );
    if ( !crtInst )
    {
      LPVOID lpMsgBuf;
      ::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                       FORMAT_MESSAGE_IGNORE_INSERTS, 0, ::GetLastError(), 0, (LPTSTR)&lpMsgBuf, 0, 0 );
    QString aMsg((char*)&lpMsgBuf);
    err = QString( "Failed to find  %1 function. %2" ).arg( CREATE_MODULE ).arg(aMsg );
    ::LocalFree( lpMsgBuf );
    }
  }
#else
  void* modLib = dlopen( libName.toLatin1(), RTLD_LAZY );
  if ( !modLib )
    err = QString( "Can not load library %1. %2" ).arg( libName ).arg( dlerror() );
  else
  {
    crtInst = (CREATE_FUNC)dlsym( modLib, GET_MODULE_NAME );
    if ( !crtInst )
      err = QString( "Failed to find function %1. %2" ).arg( CREATE_MODULE ).arg( dlerror() );
  }
#endif

  IModule* aModule = crtInst ? crtInst(this) : 0;

  if ( !err.isEmpty() ) {
    if ( mainWindow() && mainWindow()->isVisible() )
        QMessageBox::warning( mainWindow(), tr( "Error" ), err );
    else
      qWarning( qPrintable( err ) ); 
  }
  return aModule;
}

//******************************************************
bool XGUI_Workshop::activateModule()
{
    // Test of modules loading
    IModule* aModule = loadModule("GeomModule");
    if (!aModule)
        return false;
    aModule->createFeatures();
    return true;
}

//******************************************************
int XGUI_Workshop::addFeature(int thePageId, int theGroupId, 
                           const QString& theTitle, const QString& theTip, 
                           const QIcon& theIcon, 
                           const QKeySequence& theKeys)
{
    return (int) createMenuCommand(thePageId, theGroupId, LAST_CMD, theTitle, theTip, theIcon, theKeys);
}
