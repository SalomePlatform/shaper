
#ifndef PyEvent_EVENTFILTER_H
#define PyEvent_EVENTFILTER_H

#include "PyEvent.h"
#include <QObject>

#if defined WIN32
#pragma warning( disable: 4251 )
#endif

class PyEvent_Event;

/*!
  Event filter class for QApplication object that handles custom events posted by PyEvent_Event objects.
  It assumes that such custom events are alwys posted, not sent. 
  This event filter can be installed by any application that intends to use PyEvent_Event mechanism asynchronously.
  This class replaced SalomeApp_EventFilter.
*/
//PYEVENT_EXPORT
class PyEvent_EventFilter: public QObject 
{
public:
  static void Init();
  static void Destroy();

protected:
  PyEvent_EventFilter();
  virtual ~PyEvent_EventFilter();

private:
  /*! global event filter for qapplication */
  virtual bool eventFilter( QObject* o, QEvent* e );
  void processEvent( PyEvent_Event* );

private:
  static PyEvent_EventFilter* myFilter;
};

#if defined WIN32
#pragma warning( default: 4251 )
#endif

#endif
