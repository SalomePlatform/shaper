

#include "PyConsole_Request.h"

#include "PyInterp_Event.h"
#include "PyConsole_Event.h"
#include "PyConsole_EnhInterp.h"
#include "PyConsole_EnhEditor.h"

#include <QCoreApplication>

/**
 * Constructor.
 * @param theInterp interpreter that will execute the command
 * @param theCommand command text
 * @param theListener editor object that will receive the response events after execution
 * of the request
 * @param sync
 */
ExecCommand::ExecCommand( PyInterp_Interp*        theInterp,
               const QString&          theCommand,
               PyConsole_Editor*       theListener,
               bool                    sync )
    : PyInterp_LockRequest( theInterp, theListener, sync ),
      myCommand( theCommand ), myState( PyInterp_Event::ES_OK )
  {}

/**
 * Execute the command by calling the run() method of the embedded interpreter.
 */
void ExecCommand::execute()
{
  if ( myCommand != "" )
    {
      int ret = getInterp()->run( myCommand.toUtf8().data() );
      if ( ret < 0 )
        myState = PyInterp_Event::ES_ERROR;
      else if ( ret > 0 )
        myState = PyInterp_Event::ES_INCOMPLETE;
    }
}

/**
 * Create the event indicating the status of the request execution.
 * @return a QEvent
 */
QEvent* ExecCommand::createEvent()
{
  if ( IsSync() )
    QCoreApplication::sendPostedEvents( listener(), PrintEvent::EVENT_ID );
  return new PyInterp_Event( myState, this );
}


/*!
  Constructor.
  Creates a new python completion request.
  \param theInterp   python interpreter
  \param input  string containing the dir() command to be executed
  \param startMatch  part to be matched with the results of the dir() command
  \param theListener widget to get the notification messages
  \param sync        if True the request is processed synchronously
*/
CompletionCommand::CompletionCommand( PyConsole_EnhInterp*  theInterp,
               const QString&          input,
               const QString&         startMatch,
               PyConsole_EnhEditor*           theListener,
               bool                    sync)
     : PyInterp_LockRequest( theInterp, theListener, sync ),
       _tabSuccess(false), _dirArg(input), _startMatch(startMatch)
{}

/**
 * Execute the completion command by wrapping the runDirCommand() of the
 * embedded enhanced interpreter.
 */
void CompletionCommand::execute()
{
  PyConsole_EnhInterp * interp = static_cast<PyConsole_EnhInterp *>(getInterp());
    int ret = interp->runDirCommand( _dirArg,  _startMatch);
    if (ret == 0)
      _tabSuccess = true;
    else
      _tabSuccess = false;
}

/**
 * Create the event indicating the return value of the completion command.
 * @return
 */
QEvent* CompletionCommand::createEvent()
{
  int typ = _tabSuccess ? PyInterp_Event::ES_TAB_COMPLETE_OK : PyInterp_Event::ES_TAB_COMPLETE_ERR;

  return new PyInterp_Event( typ, this);
}



