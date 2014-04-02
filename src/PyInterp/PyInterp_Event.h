

#ifndef PYINTERP_EVENT_H
#define PYINTERP_EVENT_H

#include "PyInterp.h"

#include <PyEvent_Event.h>

#include <QEvent>

class PyInterp_Request;

class PyInterp_ExecuteEvent: public PyEvent_Event
{
public:
  PyInterp_ExecuteEvent( PyInterp_Request* r )
    : myRequest( r ) {}

  virtual void Execute();

protected:
  PyInterp_Request* myRequest;
};

/**
 * Events thrown by the interpreter having executed a command and indicating
 * the return status.
 */
class PYINTERP_EXPORT PyInterp_Event : public QEvent
{
  PyInterp_Event();
  PyInterp_Event( const PyInterp_Event& );

public:
  //Execution state
  enum { ES_NOTIFY = QEvent::User + 5000, ES_OK, ES_ERROR, ES_INCOMPLETE,
         ES_TAB_COMPLETE_OK, ES_TAB_COMPLETE_ERR, ES_LAST };

  PyInterp_Event( int type, PyInterp_Request* request )
    : QEvent( (QEvent::Type)type ), myRequest( request ) {}

  virtual ~PyInterp_Event();

  PyInterp_Request* GetRequest() const { return myRequest; }
  operator PyInterp_Request*() const { return myRequest; }

private:
  PyInterp_Request* myRequest;
};

#endif // PYINTERP_EVENT_H
