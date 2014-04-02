
/*!
  \class PyConsole_Editor
  \brief Python command line interpreter front-end GUI widget.
  
  This class provides simple GUI interface to the Python interpreter, including basic 
  navigation operations, executing commands (both interactively and programmatically), 
  copy-paste operations, history of the commands and so on.

  Here below is the shortcut keyboard boundings used for navigation and other operations:
  - <Enter>              : execute current command
  - <Ctrl><Break>        : clear current command
  - <Escape>             : clear current command
  - <Up>                 : previous command in the history
  - <Shift><Up>          : move cursor one row up with selection
  - <Ctrl><Up>           : move cursor one row up without selection
  - <Ctrl><Shift><Up>    : move cursor one row up with selection
  - <Down>               : next command in the history
  - <Shift><Down>        : move cursor one row down with selection
  - <Ctrl><Down>         : move cursor one row down without selection
  - <Ctrl><Shift><Down>  : move cursor one row down with selection
  - <Left>               : move one symbol left without selection
  - <Shift><Left>        : move one symbol left with selection
  - <Ctrl><Left>         : move one word left without selection
  - <Ctrl><Shift><Left>  : move one word left with selection
  - <Right>              : move one symbol right without selection
  - <Shift><Right>       : move one symbol right with selection
  - <Ctrl><Right>        : move one word right without selection
  - <Ctrl><Shift><Right> : move one word right with selection
  - <PgUp>               : first command in the history
  - <Shift><PgUp>        : move one page up with selection
  - <Ctrl><PgUp>         : move one page up without selection
  - <Ctrl><Shift><PgUp>  : scroll one page up
  - <PgDn>               : last command in the history
  - <Shift><PgDn>        : move one page down with selection
  - <Ctrl><PgDn>         : move one page down without selection
  - <Ctrl><Shift><PgDn>  : scroll one page down
  - <Home>               : move to the beginning of the line without selection
  - <Shift><Home>        : move to the beginning of the line with selection
  - <Ctrl><Home>         : move to the very first symbol without selection
  - <Ctrl><Shift><Home>  : move to the very first symbol with selection
  - <End>                : move to the end of the line without selection
  - <Shift><End>         : move to the end of the line with selection
  - <Ctrl><End>          : move to the very last symbol without selection
  - <Ctrl><Shift><End>   : move to the very last symbol with selection
  - <Backspace>          : delete symbol before the cursor
                           / remove selected text and put it to the clipboard (cut)
  - <Shift><Backspace>   : delete previous word
                           / remove selected text and put it to the clipboard (cut)
  - <Ctrl><Backspace>    : delete text from the cursor to the beginning of the line 
                           / remove selected text and put it to the clipboard (cut)
  - <Delete>             : delete symbol after the cursor 
                           / remove selected text and put it to the clipboard (cut)
  - <Shift><Delete>      : delete next word
                           / remove selected text and put it to the clipboard (cut)
  - <Ctrl><Delete>       : delete text from the cursor to the end of the line
                           / remove selected text and put it to the clipboard (cut)
  - <Ctrl><Insert>       : copy
  - <Shift><Insert>      : paste
  - <Ctrl><V>            : paste
  - <Ctrl><C>            : copy
  - <Ctrl><X>            : cut
  - <Ctrl><V>            : paste
*/

#include "PyConsole_Interp.h"   // !!! WARNING !!! THIS INCLUDE MUST BE THE VERY FIRST !!!
#include "PyConsole_Editor.h"
#include "PyConsole_Event.h"
#include "PyInterp_Event.h"
#include "PyInterp_Dispatcher.h"
#include "PyConsole_Request.h"

//#include <SUIT_Tools.h>
//#include <SUIT_FileDlg.h>
//#include <SUIT_MessageBox.h>
//#include <SUIT_FileValidator.h>

#include <QApplication>
#include <QClipboard>
#include <QDropEvent>
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextStream>
#include <QChar>

static QString READY_PROMPT = ">>> ";
static QString DOTS_PROMPT  = "... ";

/*class DumpCommandsFileValidator : public SUIT_FileValidator
{
 public:
  DumpCommandsFileValidator( QWidget* parent = 0 ) : SUIT_FileValidator ( parent ) {};
  virtual ~DumpCommandsFileValidator() {};
  virtual bool canSave( const QString& file, bool permissions );
};

bool DumpCommandsFileValidator::canSave(const QString& file, bool permissions)
{
  QFileInfo fi( file );
  if ( !QRegExp( "[A-Za-z_][A-Za-z0-9_]*" ).exactMatch( fi.completeBaseName() ) ) {
    SUIT_MessageBox::critical( parent(),
                               QObject::tr("WRN_WARNING"),
                               QObject::tr("WRN_FILE_NAME_BAD") );
    return false;
  }
  return SUIT_FileValidator::canSave( file, permissions);
}
*/
void staticCallbackStdout( void* data, char* c )
{
  if(!((PyConsole_Editor*)data)->isSuppressOutput())
    QApplication::postEvent( (PyConsole_Editor*)data, new PrintEvent( c, false ) );
}

void staticCallbackStderr( void* data, char* c )
{
  if(!((PyConsole_Editor*)data)->isSuppressOutput())
    QApplication::postEvent( (PyConsole_Editor*)data, new PrintEvent( c, true ) );
}


/*!
  \brief Constructor. 
  
  Creates python editor window.
  \param theInterp python interper
  \param theParent parent widget
*/
PyConsole_Editor::PyConsole_Editor( PyConsole_Interp* theInterp, 
                                    QWidget*          theParent )
: QTextEdit( theParent ),
  myInterp( 0 ),
  myCmdInHistory( -1 ),
  myEventLoop( 0 ),
  myShowBanner( true ),
  myIsSync( false ),
  myIsSuppressOutput( false )
{
  //QString fntSet( "" );
  QFont aFont = QFont( "Courier", 11 );//SUIT_Tools::stringToFont( fntSet );
  setFont( aFont );
  setUndoRedoEnabled( false );

  myPrompt = READY_PROMPT;
  setLineWrapMode( QTextEdit::WidgetWidth );
  setWordWrapMode( QTextOption::WrapAnywhere );
  setAcceptRichText( false );

  theInterp->setvoutcb( staticCallbackStdout, this );
  theInterp->setverrcb( staticCallbackStderr, this );

  // san - This is necessary for troubleless initialization
  onPyInterpChanged( theInterp );
}

/*!
  \brief Destructor.

  Does nothing for the moment.
*/
PyConsole_Editor::~PyConsole_Editor()
{
}

/*!
  \brief Get synchronous mode flag value.
  
  \sa setIsSync()
  \return True if python console works in synchronous mode
*/
bool PyConsole_Editor::isSync() const
{
  return myIsSync;
}

/*!
  \brief Set synchronous mode flag value.

  In synhronous mode the Python commands are executed in the GUI thread
  and the GUI is blocked until the command is finished. In the asynchronous
  mode each Python command is executed in the separate thread that does not
  block the main GUI loop.

  \param on synhronous mode flag
*/
void PyConsole_Editor::setIsSync( const bool on )
{
  myIsSync = on;
}

/*!
  \brief Get suppress output flag value.
  
  \sa setIsSuppressOutput()
  \return \c true if python console output is suppressed.
*/
bool PyConsole_Editor::isSuppressOutput() const
{
  return myIsSuppressOutput;
}

/*!
  \brief Set suppress output flag value.

  In case if suppress output flag is true, the python 
  console output suppressed.

  \param on suppress output flag
*/
void PyConsole_Editor::setIsSuppressOutput( const bool on )
{
  myIsSuppressOutput = on;
}

/*!
  \brief Get 'show banner' flag value.
  
  \sa setIsShowBanner()
  \return \c true if python console shows banner
*/
bool PyConsole_Editor::isShowBanner() const
{
  return myShowBanner;
}

/*!
  \brief Set 'show banner' flag value.

  The banner is shown in the top of the python console window.

  \sa isShowBanner()
  \param on 'show banner' flag
*/
void PyConsole_Editor::setIsShowBanner( const bool on )
{
  if ( myShowBanner != on ) {
    myShowBanner = on;
    clear();
  }
}

/*!
  \brief Get size hint for the Python console window
  \return size hint value
*/
QSize PyConsole_Editor::sizeHint() const
{
  QFontMetrics fm( font() );
  int nbLines = ( isShowBanner() ? myBanner.split("\n").count() : 0 ) + 1;
  QSize s(100, fm.lineSpacing()*nbLines);
  return s;
}

/*!
  \brief Put the string \a str to the python editor.
  \param str string to be put in the command line of the editor
  \param newBlock if True, then the string is printed on a new line
  \param isError if true, the text is printed in dark red
*/
void PyConsole_Editor::addText( const QString& str, 
                                const bool     newBlock,
                                const bool    isError)
{
  QTextCursor theCursor(textCursor());
  QTextCharFormat cf;

  moveCursor( QTextCursor::End );
  if ( newBlock )
    theCursor.insertBlock();
  if (isError)
      cf.setForeground(QBrush(Qt::red));
  else
      cf.setForeground(QBrush(Qt::black));
  theCursor.insertText( str, cf);
  moveCursor( QTextCursor::End );
  ensureCursorVisible();
}

/*!
  \brief Convenient method for executing a Python command,
  as if the user typed it manually.
  \param command python command to be executed

  !!! WARNING: doesn't work properly with multi-line commands. !!!
*/
void PyConsole_Editor::exec( const QString& command )
{
  if ( isReadOnly() ) {
    // some interactive command is being executed in this editor...
    // shedule the command to the queue
    myQueue.push_back( command );
    return;
  }
  // remove last line
  moveCursor( QTextCursor::End );
  moveCursor( QTextCursor::StartOfBlock, QTextCursor::KeepAnchor );
  textCursor().removeSelectedText();
  // set "ready" prompt
  myPrompt = READY_PROMPT;
  // clear command buffer
  myCommandBuffer.truncate( 0 );
  // unset history browsing mode
  myCmdInHistory = -1;
  // print command line by line
  QString cmd = command;
  if ( !cmd.endsWith( "\n" ) ) cmd += "\n";
  QStringList lines = command.split( "\n" );
  for ( int i = 0; i < lines.size(); i++ ) {
    if ( !lines[i].trimmed().isEmpty() )
      myHistory.push_back( lines[i] );
    addText( ( i == 0 ? READY_PROMPT : DOTS_PROMPT ) + lines[i], i != 0 );
  }
  // IPAL20182
  addText( "", true );
  // set read-only mode
  setReadOnly( true );
  // set busy cursor
  setCursor( Qt::BusyCursor );
  
  // post a request to execute Python command;
  // editor will be informed via a custom event that execution has been completed
  PyInterp_Dispatcher::Get()->Exec( createRequest( cmd ) );
}

/*!
  \brief Create request to the python dispatcher for the command execution.

  \param command python command to be executed
 */
PyInterp_Request* PyConsole_Editor::createRequest( const QString& command )
{
  return new ExecCommand( myInterp, command, this, isSync() );
}

/*!
  \brief Execute command in the python interpreter
  and wait until it is finished.

  \param command python command to be executed
 */
void PyConsole_Editor::execAndWait( const QString& command )
{
  // already running ?
  if( myEventLoop )
    return;

  // create new event loop
  myEventLoop = new QEventLoop( this );
  // execute command
  exec( command );
  // run event loop
  myEventLoop->exec();
  // delete event loop after command is processed
  delete myEventLoop;
  myEventLoop = 0;
}

/*!
  \brief Process "Enter" key press event. 
  
  Execute the command entered by the user.
*/
void PyConsole_Editor::handleReturn()
{
  // Position cursor at the end
  QTextCursor curs(textCursor());
  curs.movePosition(QTextCursor::End);
  setTextCursor(curs);

  // get last line
  QTextBlock par = document()->end().previous();
  if ( !par.isValid() ) return;

  // get command
  QString cmd = par.text().remove( 0, promptSize() );
  // extend the command buffer with the current command 
  myCommandBuffer.append( cmd );
  // add command to the history
  if ( !cmd.trimmed().isEmpty() )
    myHistory.push_back( cmd );

  // IPAL19397
  addText( "", true ); 
  
  // set read-only mode
  setReadOnly( true );
  // set busy cursor
  setCursor( Qt::BusyCursor );
  
  // post a request to execute Python command;
  // editor will be informed via a custom event that execution has been completed
  PyInterp_Dispatcher::Get()->Exec( createRequest( myCommandBuffer ) );
}

/*!
  \brief Process drop event.

  Paste dragged text.
  \param event drop event
*/
void PyConsole_Editor::dropEvent( QDropEvent* event )
{
  // get the initial drop position
  QPoint pos = event->pos();
  QTextCursor cur = cursorForPosition( event->pos() );
  // if the position is not in the last line move it to the end of the command line
  if ( cur.position() < document()->end().previous().position() + promptSize() ) {
    moveCursor( QTextCursor::End );
    pos = cursorRect().center();
  }
  // create new drop event and use it instead of the original
  QDropEvent de( pos,
                 event->possibleActions(),
                 event->mimeData(),
                 event->mouseButtons(),
                 event->keyboardModifiers(),
                 event->type() );
  QTextEdit::dropEvent( &de );
  // accept the original event
  event->acceptProposedAction();
}

/*!
  \brief Process mouse button release event.

  Left mouse button: copy selection to the clipboard.
  Middle mouse button: paste clipboard's contents.
  \param event mouse event
*/
void PyConsole_Editor::mouseReleaseEvent( QMouseEvent* event )
{
  if ( event->button() == Qt::LeftButton ) {
    QTextEdit::mouseReleaseEvent( event );
    //copy();
  }
  else if ( event->button() == Qt::MidButton ) {
    QTextCursor cur = cursorForPosition( event->pos() );
    // if the position is not in the last line move it to the end of the command line
    if ( cur.position() < document()->end().previous().position() + promptSize() ) {
      moveCursor( QTextCursor::End );
    }
    else {
      setTextCursor( cur );
    }
    const QMimeData* md = QApplication::clipboard()->mimeData( QApplication::clipboard()->supportsSelection() ? 
							       QClipboard::Selection : QClipboard::Clipboard );
    if ( md )
      insertFromMimeData( md );
  }
  else {
    QTextEdit::mouseReleaseEvent( event );
  }
}

/*!
  \brief Check if the string is command.
  
  Return True if the string \a str is likely to be the command
  (i.e. it is started from the '>>>' or '...').
  \param str string to be checked
*/
bool PyConsole_Editor::isCommand( const QString& str ) const
{
  return str.startsWith( READY_PROMPT ) || str.startsWith( DOTS_PROMPT );
}

/*!
  \brief Handle keyboard event.

  Implement navigation, history browsing, copy/paste and other common
  operations.
  \param event keyboard event
*/
void PyConsole_Editor::keyPressEvent( QKeyEvent* event )
{
  // get cursor position
  QTextCursor cur = textCursor();
  int curLine = cur.blockNumber();
  int curCol  = cur.columnNumber();

  // get last edited line
  int endLine = document()->blockCount()-1;

  // get pressed key code
  int aKey = event->key();

  // check if <Ctrl> is pressed
  bool ctrlPressed = event->modifiers() & Qt::ControlModifier;
  // check if <Shift> is pressed
  bool shftPressed = event->modifiers() & Qt::ShiftModifier;

  if ( aKey == Qt::Key_Escape || ( ctrlPressed && aKey == -1 ) ) {
    // process <Ctrl>+<Break> key-binding and <Escape> key: clear current command
    myCommandBuffer.truncate( 0 );
    myPrompt = READY_PROMPT;
    addText( myPrompt, true );
    horizontalScrollBar()->setValue( horizontalScrollBar()->minimum() );
    return;
  }
  else if ( ctrlPressed && aKey == Qt::Key_C ) {
    // process <Ctrl>+<C> key-binding : copy
    copy();
    return;
  }
  else if ( ctrlPressed && aKey == Qt::Key_X ) {
    // process <Ctrl>+<X> key-binding : cut
    cut();
    return;
  }
  else if ( ctrlPressed && aKey == Qt::Key_V ) {
    // process <Ctrl>+<V> key-binding : paste
    paste();
    return;
  }

  // check for printed key
  // #### aKey = ( aKey < Qt::Key_Space || aKey > Qt::Key_ydiaeresis ) ? aKey : 0;
  // Better:
  aKey = !(QChar(aKey).isPrint()) ? aKey : 0;

  switch ( aKey ) {
  case 0 :
    // any printed key: just print it
    {
      if ( curLine < endLine || curCol < promptSize() ) {
        moveCursor( QTextCursor::End );
      }
      QTextEdit::keyPressEvent( event );
      break;
    }
  case Qt::Key_Return:
  case Qt::Key_Enter:
    // <Enter> key: process the current command
    {
      handleReturn();
      break;
    }
  case Qt::Key_Up:
    // <Up> arrow key: process as follows:
    // - without <Ctrl>, <Shift> modifiers: previous command in history
    // - with <Ctrl> modifier key pressed:  move cursor one row up without selection
    // - with <Shift> modifier key pressed: move cursor one row up with selection
    // - with <Ctrl>+<Shift> modifier keys pressed: scroll one row up
    {
      if ( ctrlPressed && shftPressed ) {
        int value   = verticalScrollBar()->value();
        int spacing = fontMetrics().lineSpacing();
        verticalScrollBar()->setValue( value > spacing ? value-spacing : 0 );
      }
      else if ( shftPressed || ctrlPressed ) {
        if ( curLine > 0 )
          moveCursor( QTextCursor::Up, 
                      shftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor );
      }
      else { 
        if ( myCmdInHistory < 0 && myHistory.count() > 0 ) {
          // set history browsing mode
          myCmdInHistory = myHistory.count();
          // remember current command
          QTextBlock par = document()->end().previous();
          myCurrentCommand = par.text().remove( 0, promptSize() );
        }
        if ( myCmdInHistory > 0 ) {
          myCmdInHistory--;
          // get previous command in the history
          QString previousCommand = myHistory.at( myCmdInHistory );
          // print previous command
          moveCursor( QTextCursor::End );
          moveCursor( QTextCursor::StartOfBlock, QTextCursor::KeepAnchor );
          textCursor().removeSelectedText();
          addText( myPrompt + previousCommand ); 
          // move cursor to the end
          moveCursor( QTextCursor::End );
        }
      }
      break;
    }
  case Qt::Key_Down:
    // <Down> arrow key: process as follows:
    // - without <Ctrl>, <Shift> modifiers: next command in history
    // - with <Ctrl> modifier key pressed:  move cursor one row down without selection
    // - with <Shift> modifier key pressed: move cursor one row down with selection
    // - with <Ctrl>+<Shift> modifier keys pressed: scroll one row down
    {
      if ( ctrlPressed && shftPressed ) {
        int value   = verticalScrollBar()->value();
        int maxval  = verticalScrollBar()->maximum();
        int spacing = fontMetrics().lineSpacing();
        verticalScrollBar()->setValue( value+spacing < maxval ? value+spacing : maxval );
      }
      else if ( shftPressed || ctrlPressed) {
        if ( curLine < endLine )
          moveCursor( QTextCursor::Down, 
                      shftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor );
      }
      else { 
        if ( myCmdInHistory >= 0 ) {
          // get next command in the history
          myCmdInHistory++;
          QString nextCommand;
          if ( myCmdInHistory < myHistory.count() ) {
            // next command in history
            nextCommand = myHistory.at( myCmdInHistory );
          }
          else {
            // end of history is reached
            // last printed command
            nextCommand = myCurrentCommand;
            // unset history browsing mode
            myCmdInHistory = -1;
          }
          // print next or current command
          moveCursor( QTextCursor::End );
          moveCursor( QTextCursor::StartOfBlock, QTextCursor::KeepAnchor );
          textCursor().removeSelectedText();
          addText( myPrompt + nextCommand );
          // move cursor to the end
          moveCursor( QTextCursor::End );
        }
      }
      break;
    }
  case Qt::Key_Left:
    // <Left> arrow key: process as follows:
    // - without <Ctrl>, <Shift> modifiers: move one symbol left (taking into account prompt)
    // - with <Ctrl> modifier key pressed:  move one word left (taking into account prompt)
    // - with <Shift> modifier key pressed: move one symbol left with selection
    // - with <Ctrl>+<Shift> modifier keys pressed: move one word left with selection
    {
      QString txt = textCursor().block().text();
      if ( !shftPressed && isCommand( txt ) && curCol <= promptSize() ) {
        moveCursor( QTextCursor::Up );
        moveCursor( QTextCursor::EndOfBlock );
      }
      else {
        QTextEdit::keyPressEvent( event );
      }
      break;
    }
  case Qt::Key_Right:
    // <Right> arrow key: process as follows:
    // - without <Ctrl>, <Shift> modifiers: move one symbol right (taking into account prompt)
    // - with <Ctrl> modifier key pressed:  move one word right (taking into account prompt)
    // - with <Shift> modifier key pressed: move one symbol right with selection
    // - with <Ctrl>+<Shift> modifier keys pressed: move one word right with selection
    {
      QString txt = textCursor().block().text();
      if ( !shftPressed ) {
        if ( curCol < txt.length() ) {
          if ( isCommand( txt ) && curCol < promptSize() ) {
            cur.setPosition( cur.block().position() + promptSize() );
            setTextCursor( cur );
            break;
          }
        }
        else {
          if ( curLine < endLine && isCommand( textCursor().block().next().text() ) ) {
            cur.setPosition( cur.position() + promptSize()+1 );
            setTextCursor( cur );
            horizontalScrollBar()->setValue( horizontalScrollBar()->minimum() );
            break;
          }
        }
      }
      QTextEdit::keyPressEvent( event );
      break;
    }
  case Qt::Key_PageUp:
    // <PageUp> key: process as follows:
    // - without <Ctrl>, <Shift> modifiers: first command in history
    // - with <Ctrl> modifier key pressed:  move cursor one page up without selection
    // - with <Shift> modifier key pressed: move cursor one page up with selection
    // - with <Ctrl>+<Shift> modifier keys pressed: scroll one page up
    {
      if ( ctrlPressed && shftPressed ) {
        verticalScrollBar()->triggerAction(QAbstractSlider::SliderPageStepSub);
      }
      else if ( shftPressed || ctrlPressed ) {
        bool moved = false;
        qreal lastY = cursorRect( cur ).top();
        qreal distance = 0;
        // move using movePosition to keep the cursor's x
        do {
          qreal y = cursorRect( cur ).top();
          distance += qAbs( y - lastY );
          lastY = y;
          moved = cur.movePosition( QTextCursor::Up, 
                                    shftPressed ? QTextCursor::KeepAnchor : 
                                                  QTextCursor::MoveAnchor );
        } while ( moved && distance < viewport()->height() );
        if ( moved ) {
          cur.movePosition( QTextCursor::Down, 
                            shftPressed ? QTextCursor::KeepAnchor : 
                                          QTextCursor::MoveAnchor );
          verticalScrollBar()->triggerAction( QAbstractSlider::SliderPageStepSub );
        }
        setTextCursor( cur );
      }
      else { 
        if ( myCmdInHistory < 0 && myHistory.count() > 0 ) {
          // set history browsing mode
          myCmdInHistory = myHistory.count();
          // remember current command
          QTextBlock par = document()->end().previous();
          myCurrentCommand = par.text().remove( 0, promptSize() );
        }
        if ( myCmdInHistory > 0 ) {
          myCmdInHistory = 0;
          // get very first command in the history
          QString firstCommand = myHistory.at( myCmdInHistory );
          // print first command
          moveCursor( QTextCursor::End );
          moveCursor( QTextCursor::StartOfBlock, QTextCursor::KeepAnchor );
          textCursor().removeSelectedText();
          addText( myPrompt + firstCommand ); 
          // move cursor to the end
          moveCursor( QTextCursor::End );
        }
      }
      break;
    }
  case Qt::Key_PageDown:
    // <PageDown> key: process as follows:
    // - without <Ctrl>, <Shift> modifiers: last command in history
    // - with <Ctrl> modifier key pressed:  move cursor one page down without selection
    // - with <Shift> modifier key pressed: move cursor one page down with selection
    // - with <Ctrl>+<Shift> modifier keys pressed: scroll one page down
    {
      if ( ctrlPressed && shftPressed ) {
        verticalScrollBar()->triggerAction(QAbstractSlider::SliderPageStepAdd);
      }
      else if ( shftPressed || ctrlPressed ) {
        bool moved = false;
        qreal lastY = cursorRect( cur ).top();
        qreal distance = 0;
        // move using movePosition to keep the cursor's x
        do {
          qreal y = cursorRect( cur ).top();
          distance += qAbs( y - lastY );
          lastY = y;
          moved = cur.movePosition( QTextCursor::Down, 
                                    shftPressed ? QTextCursor::KeepAnchor : 
                                                  QTextCursor::MoveAnchor );
        } while ( moved && distance < viewport()->height() );
        if ( moved ) {
          cur.movePosition( QTextCursor::Up, 
                            shftPressed ? QTextCursor::KeepAnchor : 
                                          QTextCursor::MoveAnchor );
          verticalScrollBar()->triggerAction( QAbstractSlider::SliderPageStepSub );
        }
        setTextCursor( cur );
      }
      else { 
        if ( myCmdInHistory >= 0 ) {
          // unset history browsing mode
          myCmdInHistory = -1;
          // print current command
          moveCursor( QTextCursor::End );
          moveCursor( QTextCursor::StartOfBlock, QTextCursor::KeepAnchor );
          textCursor().removeSelectedText();
          addText( myPrompt + myCurrentCommand ); 
          // move cursor to the end
          moveCursor( QTextCursor::End );
        }
      }
      break;
    }
  case Qt::Key_Home: 
    // <Home> key: process as follows:
    // - without <Ctrl>, <Shift> modifiers: move cursor to the beginning of the current line without selection
    // - with <Ctrl> modifier key pressed:  move cursor to the very first symbol without selection
    // - with <Shift> modifier key pressed: move cursor to the beginning of the current line with selection
    // - with <Ctrl>+<Shift> modifier keys pressed: move cursor to the very first symbol with selection
    {
      if ( ctrlPressed ) { 
        moveCursor( QTextCursor::Start, 
                    shftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor );
      }
      else {
        QString txt = textCursor().block().text();
        if ( isCommand( txt ) ) {
          if ( shftPressed ) {
            if ( curCol > promptSize() ) {
              cur.movePosition( QTextCursor::StartOfLine, QTextCursor::KeepAnchor );
              cur.movePosition( QTextCursor::Right, QTextCursor::KeepAnchor, promptSize() );
            }
          }
          else {
            cur.movePosition( QTextCursor::StartOfLine );
            cur.movePosition( QTextCursor::Right, QTextCursor::MoveAnchor, promptSize() );
          }
          setTextCursor( cur );
        }
        else {
          moveCursor( QTextCursor::StartOfBlock, 
                      shftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor );
        }
        horizontalScrollBar()->setValue( horizontalScrollBar()->minimum() );
      }
      break;
    }
  case Qt::Key_End:
    // <End> key: process as follows:
    // - without <Ctrl>, <Shift> modifiers: move cursor to the end of the current line without selection
    // - with <Ctrl> modifier key pressed:  move cursor to the very last symbol without selection
    // - with <Shift> modifier key pressed: move cursor to the end of the current line with selection
    // - with <Ctrl>+<Shift> modifier keys pressed: move cursor to the very last symbol with selection
    {
      moveCursor( ctrlPressed ? QTextCursor::End : QTextCursor::EndOfBlock, 
                  shftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor );
      break;
    }  
  case Qt::Key_Backspace :
    // <Backspace> key: process as follows
    // - without any modifiers : delete symbol before the cursor / selection (taking into account prompt)
    // - with <Shift> modifier key pressed: delete previous word
    // - with <Ctrl> modifier key pressed: delete text from the cursor to the line beginning
    // works only for last (command) line
    {
      if ( cur.hasSelection() ) {
        cut();
      }
      else if ( cur.position() > document()->end().previous().position() + promptSize() ) {
        if ( shftPressed ) {
          moveCursor( QTextCursor::PreviousWord, QTextCursor::KeepAnchor );
          textCursor().removeSelectedText();
        }
        else if ( ctrlPressed ) {
          cur.setPosition( document()->end().previous().position() + promptSize(),
                           QTextCursor::KeepAnchor );
          setTextCursor( cur );
          textCursor().removeSelectedText();
        }
        else {
          QTextEdit::keyPressEvent( event );
        }
      }
      else {
        cur.setPosition( document()->end().previous().position() + promptSize() );
        setTextCursor( cur );
        horizontalScrollBar()->setValue( horizontalScrollBar()->minimum() );
      }
      break;
    }
  case Qt::Key_Delete :
    // <Delete> key: process as follows
    // - without any modifiers : delete symbol after the cursor / selection (taking into account prompt)
    // - with <Shift> modifier key pressed: delete next word
    // - with <Ctrl> modifier key pressed: delete text from the cursor to the end of line
    // works only for last (command) line
    {
      if ( cur.hasSelection() ) {
        cut();
      }
      else if ( cur.position() > document()->end().previous().position() + promptSize()-1 ) {
        if ( shftPressed ) {
          moveCursor( QTextCursor::NextWord, QTextCursor::KeepAnchor );
          textCursor().removeSelectedText();
        }
        else if ( ctrlPressed ) {
          moveCursor( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor );
          textCursor().removeSelectedText();
        }
        else {
          QTextEdit::keyPressEvent( event );
        }
      }
      else {
        cur.setPosition( document()->end().previous().position() + promptSize() );
        setTextCursor( cur );
        horizontalScrollBar()->setValue( horizontalScrollBar()->minimum() );
      }
      break;
    }
  case Qt::Key_Insert :
    // <Insert> key: process as follows
    // - with <Ctrl> modifier key pressed:  copy()
    // - with <Shift> modifier key pressed: paste() to the command line
    {
      if ( ctrlPressed ) {
        copy();
      }
      else if ( shftPressed ) {
        paste();
      }
      else
        QTextEdit::keyPressEvent( event );
      break;
    }
  }
}

/*!
  \brief Handle notification event coming from Python dispatcher.
  \param event notification event
*/
void PyConsole_Editor::customEvent( QEvent* event )
{
  switch( event->type() )
  {
  case PrintEvent::EVENT_ID:
    {
      PrintEvent* pe=(PrintEvent*)event;
      addText( pe->text(), false, pe->isError());
      return;
    }
  case PyInterp_Event::ES_OK:
  case PyInterp_Event::ES_ERROR:
  {
    // clear command buffer
    myCommandBuffer.truncate( 0 );
    // add caret return line if necessary
    QTextBlock par = document()->end().previous();
    QString txt = par.text();
    txt.truncate( txt.length() - 1 );
    // IPAL19397 : addText moved to handleReturn() method
    //if ( !txt.isEmpty() )
    //  addText( "", true );
    // set "ready" prompt
    myPrompt = READY_PROMPT;
    addText( myPrompt );
    // unset busy cursor
    unsetCursor();
    // stop event loop (if running)
    if ( myEventLoop )
      myEventLoop->exit();
    break;
  }
  case PyInterp_Event::ES_INCOMPLETE:
  {
    // extend command buffer (multi-line command)
    myCommandBuffer.append( "\n" );
    // add caret return line if necessary
    QTextBlock par = document()->end().previous();
    QString txt = par.text();
    txt.truncate( txt.length() - 1 );
    // IPAL19397 : addText moved to handleReturn() method
    //if ( !txt.isEmpty() )
    //  addText( "", true );
    // set "dot" prompt
    myPrompt = DOTS_PROMPT;
    addText( myPrompt/*, true*/ ); // IPAL19397
    // unset busy cursor
    unsetCursor();
    // stop event loop (if running)
    if ( myEventLoop )
      myEventLoop->exit();
    break;
  }
  default:
    QTextEdit::customEvent( event );
  }
  
  // unset read-only state
  setReadOnly( false );
  // unset history browsing mode
  myCmdInHistory = -1;

  if ( (int)event->type() == (int)PyInterp_Event::ES_OK && myQueue.count() > 0 )
  {
    // process the next sheduled command from the queue (if there is any)
    QString nextcmd = myQueue[0];
    myQueue.pop_front();
    exec( nextcmd );
  }
}

/*!
  \brief Handle Python interpreter change.

  Perform initialization actions if the interpreter is changed.
  \param interp python interpreter is being set
*/
void PyConsole_Editor::onPyInterpChanged( PyConsole_Interp* interp )
{
  if ( myInterp != interp 
       // Force read-only state and wait cursor when myInterp is NULL
      || !myInterp ) {
    myInterp = interp;
    if ( myInterp ) {
      // print banner
      myBanner = myInterp->getbanner().c_str();
      if ( isShowBanner() )
	addText( myBanner );
      // clear command buffer
      myCommandBuffer.truncate(0);
      // unset read-only state
      setReadOnly( false );
      // unset history browsing mode
      myCmdInHistory = -1;
      // add prompt
      addText( myPrompt );
      // unset busy cursor
      viewport()->unsetCursor();
      // stop event loop (if running)
      if( myEventLoop)
        myEventLoop->exit();
    }
    else {
      // clear contents
      clear();
      // set read-only state
      setReadOnly( true );
      // set busy cursor
      setCursor( Qt::WaitCursor );
    }
  }
}

/*!
  \brief "Copy" operation.
  
  Reimplemented from Qt.
  Warning! In Qt4 this method is not virtual.
 */
void PyConsole_Editor::cut()
{
  QTextCursor cur = textCursor();
  if ( cur.hasSelection() ) {
    QApplication::clipboard()->setText( cur.selectedText() );
    int startSelection = cur.selectionStart();
    if ( startSelection < document()->end().previous().position() + promptSize() )
      startSelection = document()->end().previous().position() + promptSize();
    int endSelection = cur.selectionEnd();
    if ( endSelection < document()->end().previous().position() + promptSize() )
      endSelection = document()->end().previous().position() + promptSize();
    cur.setPosition( startSelection );
    cur.setPosition( endSelection, QTextCursor::KeepAnchor );
    horizontalScrollBar()->setValue( horizontalScrollBar()->minimum() );
    setTextCursor( cur );
    textCursor().removeSelectedText();
  }
}

/*!
  \brief "Paste" operation.

  Reimplemented from Qt.
  Warning! In Qt4 this method is not virtual.
 */
void PyConsole_Editor::paste()
{
  QTextCursor cur = textCursor();
  if ( cur.hasSelection() ) {
    int startSelection = cur.selectionStart();
    if ( startSelection < document()->end().previous().position() + promptSize() )
      startSelection = document()->end().previous().position() + promptSize();
    int endSelection = cur.selectionEnd();
    if ( endSelection < document()->end().previous().position() + promptSize() )
      endSelection = document()->end().previous().position() + promptSize();
    cur.setPosition( startSelection );
    cur.setPosition( endSelection, QTextCursor::KeepAnchor );
    horizontalScrollBar()->setValue( horizontalScrollBar()->minimum() );
    setTextCursor( cur );
    textCursor().removeSelectedText();
  }
  if ( textCursor().position() < document()->end().previous().position() + promptSize() )
    moveCursor( QTextCursor::End );
  QTextEdit::paste();
}

/*!
  \brief "Clear" operation.

  Reimplemented from Qt.
  Warning! In Qt4 this method is not virtual.
 */
void PyConsole_Editor::clear()
{
  QTextEdit::clear();
  if ( isShowBanner() )
    addText( myBanner );
  myPrompt = READY_PROMPT;
  addText( myPrompt );
}

/*!
  \brief "Dump commands" operation.
 */
/*void PyConsole_Editor::dump()
{
  QStringList aFilters;
  aFilters.append( tr( "PYTHON_FILES_FILTER" ) );
  
  QString fileName = SUIT_FileDlg::getFileName( this, QString(),
  	  	     		     aFilters, tr( "TOT_DUMP_PYCOMMANDS" ),
 	    			     false, true, new DumpCommandsFileValidator( this ) );
  if ( fileName != "" ) {
    QFile file( fileName ); 
    if ( !file.open( QFile::WriteOnly ) )
      return;

    QTextStream out (&file);
  
    for( int i=0; i<myHistory.count(); i++ ) {
         out<<myHistory[i]<<endl;
    }
    file.close();
  }
}*/
