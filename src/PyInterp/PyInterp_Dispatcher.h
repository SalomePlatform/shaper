
#ifndef PYINTERP_DISPATCHER_H
#define PYINTERP_DISPATCHER_H

#include "PyInterp.h"   // !!! WARNING !!! THIS INCLUDE MUST BE THE VERY FIRST !!!

#include "PyInterp_Request.h"   // full include instead of forward declaration
                                // everyone inc'ing the Dispatcher will get the requests for free.

#include <QMutex>
#include <QThread>
#include <QQueue>

class QObject;
class PyInterp_Watcher;

//PYINTERP_EXPORT
class PyInterp_Dispatcher : protected QThread
{
  PyInterp_Dispatcher(); // private constructor

public:
  static PyInterp_Dispatcher* Get();

  virtual                     ~PyInterp_Dispatcher();

  bool                        IsBusy() const;  
  void                        Exec( PyInterp_Request* );

private:
  virtual void                run();
  void                        processRequest( PyInterp_Request* );
  void                        objectDestroyed( const QObject* );

private:
  typedef PyInterp_Request*   RequestPtr;

  QQueue<RequestPtr>          myQueue;
  QMutex                      myQueueMutex;
  PyInterp_Watcher*           myWatcher;

  static PyInterp_Dispatcher* myInstance;

  friend class PyInterp_Watcher;
};

#endif // PYINTERP_DISPATCHER_H
