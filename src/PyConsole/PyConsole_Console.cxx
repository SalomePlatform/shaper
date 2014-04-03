
/*!
  \class PyConsole_Console
  \brief Python console widget.
*/  

#include "PyConsole_Interp.h"   /// !!! WARNING !!! THIS INCLUDE MUST BE VERY FIRST !!!
#include "PyConsole_Console.h"
#include "PyConsole_EnhEditor.h"
#include "PyConsole_EnhInterp.h"

//#include <Qtx.h>

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QEvent>
#include <QMenu>
#include <QVBoxLayout>

/*!
  \brief Constructor.

  Creates new python console widget.
  \param parent parent widget
  \param interp python interpreter
*/
PyConsole_Console::PyConsole_Console( QWidget* parent, PyConsole_Interp* interp )
: QWidget( parent )
{
  // create python interpreter
  myInterp = interp;
  if ( !myInterp )
    myInterp = new PyConsole_Interp();
  
  // initialize Python interpretator
  myInterp->initialize();
  
  // create editor console
  QVBoxLayout* lay = new QVBoxLayout( this );
  lay->setMargin( 0 );
  myEditor = new PyConsole_Editor( myInterp, this );
  char* synchronous = getenv("PYTHON_CONSOLE_SYNC");
  if (synchronous && atoi(synchronous))
  {
      myEditor->setIsSync(true);
  }
  myEditor->viewport()->installEventFilter( this );
  lay->addWidget( myEditor );

  createActions();
}

/**
 * Protected constructor.
 */
PyConsole_Console::PyConsole_Console( QWidget* parent, PyConsole_Interp* i,  PyConsole_Editor* e)
  : QWidget (parent), myEditor(e), myInterp(i)
{}

/*!
  \brief Destructor.

  Does nothing for the moment.
*/
PyConsole_Console::~PyConsole_Console()
{
}

/*!
  \brief Execute python command in the interpreter.
  \param command string with command and arguments
*/
void PyConsole_Console::exec( const QString& command )
{
  if ( myEditor )
    myEditor->exec( command );
}

/*!
  \brief Execute python command in the interpreter 
         and wait until it is finished.
  
  Block execution of main application until the python command is executed.
  \param command string with command and arguments
*/
void PyConsole_Console::execAndWait( const QString& command )
{
  if ( myEditor )
    myEditor->execAndWait( command );
}

/*!
  \brief Get synchronous mode flag value.
  
  \sa setIsSync()
  \return True if python console works in synchronous mode
*/
bool PyConsole_Console::isSync() const
{
  return myEditor->isSync();
}

/*!
  \brief Set synchronous mode flag value.

  In synhronous mode the Python commands are executed in the GUI thread
  and the GUI is blocked until the command is finished. In the asynchronous
  mode each Python command is executed in the separate thread that does not
  block the main GUI loop.

  \param on synhronous mode flag
*/
void PyConsole_Console::setIsSync( const bool on )
{
  myEditor->setIsSync( on );
}

/*!
  \brief Get suppress output flag value.
  
  \sa setIsSuppressOutput()
  \return True if python console output is suppressed.
*/
bool PyConsole_Console::isSuppressOutput() const
{
  return myEditor->isSuppressOutput();
}

/*!
  \brief Set suppress output flag value.

  In case if suppress output flag is true, the python 
  console output suppressed.

  \param on suppress output flag
*/
void PyConsole_Console::setIsSuppressOutput( const bool on )
{
  myEditor->setIsSuppressOutput(on);
}

/*!
  \brief Get 'show banner' flag value.
  
  \sa setIsShowBanner()
  \return \c true if python console shows banner
*/
bool PyConsole_Console::isShowBanner() const
{
  return myEditor->isShowBanner();
}

/*!
  \brief Set 'show banner' flag value.

  The banner is shown in the top of the python console window.

  \sa isShowBanner()
  \param on 'show banner' flag
*/
void PyConsole_Console::setIsShowBanner( const bool on )
{
  myEditor->setIsShowBanner( on );
}

/*!
  \brief Change the python console's font.
  \param f new font
*/
void PyConsole_Console::setFont( const QFont& f )
{
  if( myEditor )
    myEditor->setFont( f );
}

/*!
  \brief Get python console font.
  \return current python console's font
*/
QFont PyConsole_Console::font() const
{
  QFont res;
  if( myEditor )
    res = myEditor->font();
  return res;
}

/*!
  \brief Event handler.

  Handles context menu request event.

  \param o object
  \param e event
  \return True if the event is processed and further processing should be stopped
*/
bool PyConsole_Console::eventFilter( QObject* o, QEvent* e )
{
  if ( o == myEditor->viewport() && e->type() == QEvent::ContextMenu )
  {
    //contextMenuRequest( (QContextMenuEvent*)e );
    return true;
  }
  return QWidget::eventFilter( o, e );
}

/*!
  \brief Create the context popup menu.

  Fill in the popup menu with the commands.

  \param menu context popup menu
*/
void PyConsole_Console::contextMenuPopup( QMenu* menu )
{
  if ( myEditor->isReadOnly() )
    return;

  menu->addAction( myActions[CopyId] );
  menu->addAction( myActions[PasteId] );
  menu->addAction( myActions[ClearId] );
  menu->addSeparator();
  menu->addAction( myActions[SelectAllId] );
  menu->addSeparator();
  menu->addAction( myActions[DumpCommandsId] );

  //Qtx::simplifySeparators( menu );

  updateActions();
}

/*!
  \brief Set actions to be visible in the context popup menu.
  
  Actions, which IDs are set in \a flags parameter, will be shown in the 
  context popup menu. Other actions will not be shown.

  \param flags ORed together actions flags
*/
void PyConsole_Console::setMenuActions( const int flags )
{
  myActions[CopyId]->setVisible( flags & CopyId );
  myActions[PasteId]->setVisible( flags & PasteId );
  myActions[ClearId]->setVisible( flags & ClearId );
  myActions[SelectAllId]->setVisible( flags & SelectAllId );
  myActions[DumpCommandsId]->setVisible( flags & DumpCommandsId );
}

/*!
  \brief Get menu actions which are currently visible in the context popup menu.
  \return ORed together actions flags
  \sa setMenuActions()
*/
int PyConsole_Console::menuActions() const
{
  int ret = 0;
  ret = ret | ( myActions[CopyId]->isVisible() ? CopyId : 0 );
  ret = ret | ( myActions[PasteId]->isVisible() ? PasteId : 0 );
  ret = ret | ( myActions[ClearId]->isVisible() ? ClearId : 0 );
  ret = ret | ( myActions[SelectAllId]->isVisible() ? SelectAllId : 0 );
  ret = ret | ( myActions[DumpCommandsId]->isVisible() ? DumpCommandsId : 0 );
  return ret;
}

/*!
  \brief Create menu actions.

  Create context popup menu actions.
*/
void PyConsole_Console::createActions()
{
  QAction* a = new QAction( tr( "EDIT_COPY_CMD" ), this );
  a->setStatusTip( tr( "EDIT_COPY_CMD" ) );
  connect( a, SIGNAL( triggered( bool ) ), myEditor, SLOT( copy() ) );
  myActions.insert( CopyId, a );

  a = new QAction( tr( "EDIT_PASTE_CMD" ), this );
  a->setStatusTip( tr( "EDIT_PASTE_CMD" ) );
  connect( a, SIGNAL( triggered( bool ) ), myEditor, SLOT( paste() ) );
  myActions.insert( PasteId, a );

  a = new QAction( tr( "EDIT_CLEAR_CMD" ), this );
  a->setStatusTip( tr( "EDIT_CLEAR_CMD" ) );
  connect( a, SIGNAL( triggered( bool ) ), myEditor, SLOT( clear() ) );
  myActions.insert( ClearId, a );

  a = new QAction( tr( "EDIT_SELECTALL_CMD" ), this );
  a->setStatusTip( tr( "EDIT_SELECTALL_CMD" ) );
  connect( a, SIGNAL( triggered( bool ) ), myEditor, SLOT( selectAll() ) );
  myActions.insert( SelectAllId, a );
  
  a = new QAction( tr( "EDIT_DUMPCOMMANDS_CMD" ), this );
  a->setStatusTip( tr( "EDIT_DUMPCOMMANDS_CMD" ) );
  connect( a, SIGNAL( triggered( bool ) ), myEditor, SLOT( dump() ) );
  myActions.insert( DumpCommandsId, a );
}

/*!
  \brief Update menu actions.

  Update context popup menu action state.
*/
void PyConsole_Console::updateActions()
{
  myActions[CopyId]->setEnabled( myEditor->textCursor().hasSelection() );
  myActions[PasteId]->setEnabled( !myEditor->isReadOnly() && !QApplication::clipboard()->text().isEmpty() );
  myActions[SelectAllId]->setEnabled( !myEditor->document()->isEmpty() );
}

/**
 * Similar to constructor of the base class but using enhanced objects.
 * TODO: this should really be done in a factory to avoid code duplication.
 * @param parent
 * @param interp
 */
PyConsole_EnhConsole::PyConsole_EnhConsole( QWidget* parent, PyConsole_EnhInterp* interp)
  : PyConsole_Console(parent, interp, 0)
{
  // create python interpreter
  myInterp = interp;
  if ( !myInterp )
    myInterp = new PyConsole_EnhInterp();

  // initialize Python interpretator
  myInterp->initialize();

  // create editor console
  QVBoxLayout* lay = new QVBoxLayout( this );
  lay->setMargin( 0 );
  myEditor = new PyConsole_EnhEditor( static_cast<PyConsole_EnhInterp*>(myInterp), this );
  char* synchronous = getenv("PYTHON_CONSOLE_SYNC");
  if (synchronous && atoi(synchronous))
  {
      myEditor->setIsSync(true);
  }
  myEditor->viewport()->installEventFilter( this );
  lay->addWidget( myEditor );

  createActions();
}
