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
    IWorkbench* aPage = addWorkbench(tr("HOME_MENU_TITLE"));

    // File commands group
    IMenuGroup* aGroup = aPage->addGroup();

    IFeatureMenu* aCommand;

    aCommand = aGroup->addFeature("NEW_CMD", tr("NEW_MENU"), tr("NEW_MENU_TIP"),
                                  QIcon(":pictures/new.png"), QKeySequence::New);
    aCommand->connectTo(this, SLOT(onNew()));

    aCommand = aGroup->addFeature("OPEN_CMD", tr("OPEN_MENU"), tr("OPEN_MENU_TIP"),
                                  QIcon(":pictures/open.png"), QKeySequence::Open);
    aCommand->connectTo(this, SLOT(onOpen()));

    aCommand = aGroup->addFeature("SAVE_CMD", tr("SAVE_MENU"), tr("SAVE_MENU_TIP"),
                                  QIcon(":pictures/save.png"), QKeySequence::Save);
    aCommand->connectTo(this, SLOT(onSave()));
    aCommand->disable();


    aCommand = aGroup->addFeature("SAVEAS_CMD", tr("SAVEAS_MENU"), tr("SAVEAS_MENU_TIP"),
                                  QIcon(":pictures/save.png"));
    aCommand->connectTo(this, SLOT(onSaveAs()));
    aCommand->disable();

    // Edit commands group
    //aGroup = aPage->addGroup();

    aCommand = aGroup->addFeature("UNDO_CMD", tr("UNDO_MENU"), tr("UNDO_MENU_TIP"),
                                  QIcon(":pictures/undo.png"), QKeySequence::Undo);

    aCommand = aGroup->addFeature("REDO_CMD", tr("REDO_MENU"), tr("REDO_MENU_TIP"),
                                 QIcon(":pictures/redo.png"), QKeySequence::Redo);

    aCommand = aGroup->addFeature("REBUILD_CMD", tr("REBUILD_MENU"), tr("REBUILD_MENU_TIP"),
                                 QIcon(":pictures/rebuild.png"));

    aCommand = aGroup->addFeature("EXIT_CMD", tr("EXIT_MENU"), tr("EXIT_MENU_TIP"),
                                  QIcon(":pictures/close.png"), QKeySequence::Close);
    aCommand->connectTo(this, SLOT(onExit()));

}

//******************************************************
IWorkbench* XGUI_Workshop::addWorkbench(const QString& theName)
{
    XGUI_MainMenu* aMenuBar = myMainWindow->menuObject();
    return aMenuBar->addWorkbench(theName);
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

