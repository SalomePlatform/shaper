
#include "PyEvent_EventFilter.h"
#include "PyEvent_Event.h"

#include <QApplication>

PyEvent_EventFilter* PyEvent_EventFilter::myFilter = NULL;

/*!Constructor.*/
PyEvent_EventFilter::PyEvent_EventFilter()
: QObject()
{
  /* VSR 13/01/03 : installing global event filter for the application */
  qApp->installEventFilter( this );
}

/*!Destructor.*/
PyEvent_EventFilter::~PyEvent_EventFilter()
{
  qApp->removeEventFilter( this );
}

/*!
  Custom event filter
*/
bool PyEvent_EventFilter::eventFilter( QObject* o, QEvent* e )
{
  if ( e->type() == PyEvent_EVENT )
  { 
    PyEvent_Event* aSE = (PyEvent_Event*)((PyEvent_CustomEvent*)e)->data();
    processEvent(aSE);
    ((PyEvent_CustomEvent*)e)->setData( 0 );
    return true;
  }
  return QObject::eventFilter( o, e );
}

/*!Process event.*/
void PyEvent_EventFilter::processEvent( PyEvent_Event* theEvent )
{
  if(theEvent)
    theEvent->ExecutePostedEvent();
}

/*!Create new instance of PyEvent_EventFilter*/
void PyEvent_EventFilter::Init()
{
  if( myFilter==NULL )
    myFilter = new PyEvent_EventFilter();
}

/*!Destroy filter.*/
void PyEvent_EventFilter::Destroy()
{
  if( myFilter )
  {
    delete myFilter;
    myFilter = NULL;
  }
}
