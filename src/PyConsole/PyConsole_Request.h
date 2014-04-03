


#ifndef PYCONSOLE_REQUEST_H_
#define PYCONSOLE_REQUEST_H_

#include "PyConsole.h"
#include "PyInterp_Request.h"

#include <vector>
#include <QString>
#include <QEvent>

class PyInterp_Interp;
class PyConsole_Editor;

/*!
  \class ExecCommand
  \brief Python command execution request.
  \internal
*/
class ExecCommand : public PyInterp_LockRequest
{
public:
  /*!
    \brief Constructor.

    Creates new python command execution request.
    \param theInterp   python interpreter
    \param theCommand  python command
    \param theListener widget to get the notification messages
    \param sync        if True the request is processed synchronously
  */
  ExecCommand( PyInterp_Interp*        theInterp,
               const QString&          theCommand,
               PyConsole_Editor*       theListener,
               bool                    sync = false );

protected:
  /*!
    \brief Execute the python command in the interpreter and
           get its execution status.
  */
  virtual void execute();

  /*!
    \brief Create and return a notification event.
    \return new notification event
  */
  virtual QEvent* createEvent();

private:
  QString myCommand;   //!< Python command
  int     myState;     //!< Python command execution status
};

class PyConsole_EnhInterp;
class PyConsole_EnhEditor;

class CompletionCommand : public PyInterp_LockRequest
{
public:
  CompletionCommand( PyConsole_EnhInterp*      theInterp,
               const QString&          input,
               const QString&          startMatch,
               PyConsole_EnhEditor*    theListener,
               bool                    sync = false );


protected:
  /** List of separators identifying the last parsable token for completion */
  static const std::vector<QString> SEPARATORS;

  /** String to be passed to the dir() command */
  QString _dirArg;
  /** Begining of the command (as typed by the user) */
  QString _startMatch;
  /** was the completion command successful */
  bool _tabSuccess;

  virtual void execute();
  virtual QEvent* createEvent();

};

#endif /* PYCONSOLE_REQUEST_H_ */
