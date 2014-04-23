
#include "PyInterp_Dispatcher.h"   // !!! WARNING !!! THIS INCLUDE MUST BE THE VERY FIRST !!!
#include "PyInterp_Interp.h"
#include "PyInterp_Watcher.h"
#include "PyInterp_Request.h"

#include <QObject>
#include <QCoreApplication>

PyInterp_Dispatcher* PyInterp_Dispatcher::myInstance = 0;

void PyInterp_Request::process()
{
  safeExecute();

  bool isSync = IsSync();

  if ( !isSync )
    myMutex.lock();

  if ( listener() )
    processEvent( listener() );

  if ( !isSync )
    myMutex.unlock();
}

void PyInterp_Request::safeExecute()
{
  //ProcessVoidEvent( new PyInterp_ExecuteEvent( this ) );
  execute();
}

void PyInterp_Request::Destroy( PyInterp_Request* request )
{
  // Lock and unlock the mutex to avoid errors on its deletion
  request->myMutex.lock();
  request->myMutex.unlock();
  delete request;
}

QEvent* PyInterp_Request::createEvent()
{
  return new PyInterp_Event( PyInterp_Event::ES_NOTIFY, this );
}

void PyInterp_Request::processEvent( QObject* o )
{
  if ( !o )
    return;

  QEvent* e = createEvent();
  if ( !e )
    return;

  if ( !IsSync() )
    QCoreApplication::postEvent( o, e );
  else
  {
    QCoreApplication::sendEvent( o, e );
    delete e;
  }
}

void PyInterp_Request::setListener( QObject* o )
{
  myMutex.lock();
  myListener = o;
  myMutex.unlock();
}

void PyInterp_LockRequest::safeExecute()
{
  if ( getInterp() ){
    PyLockWrapper aLock = getInterp()->GetLockWrapper();
    //ProcessVoidEvent( new PyInterp_ExecuteEvent( this ) );
    execute();
  }
}

PyInterp_Event::~PyInterp_Event()
{
  PyInterp_Request::Destroy( myRequest );
  myRequest = 0;
}

PyInterp_Dispatcher* PyInterp_Dispatcher::Get()
{
  if ( !myInstance )
    myInstance = new PyInterp_Dispatcher();
  return myInstance;
}

PyInterp_Dispatcher::PyInterp_Dispatcher() 
: QThread()
{
  myWatcher = new PyInterp_Watcher();
}

PyInterp_Dispatcher::~PyInterp_Dispatcher()
{
  // Clear the request queue
  myQueueMutex.lock();

  QListIterator<RequestPtr> it( myQueue );
  while ( it.hasNext() )
    PyInterp_Request::Destroy( it.next() );
  myQueue.clear();

  myQueueMutex.unlock();

  // Wait for run() to finish
  wait();

  delete myWatcher;
  myWatcher = 0;
}

bool PyInterp_Dispatcher::IsBusy() const
{
  return isRunning();
}

void PyInterp_Dispatcher::Exec( PyInterp_Request* theRequest )
{
  if ( !theRequest )
    return;

  //if ( theRequest->IsSync() && !IsBusy() ) // synchronous processing - nothing is done if dispatcher is busy!
  if ( theRequest->IsSync() ) // synchronous processing
    processRequest( theRequest );
  else // asynchronous processing
  {
    myQueueMutex.lock();
    myQueue.enqueue( theRequest );
    if ( theRequest->listener() )
      QObject::connect( theRequest->listener(), SIGNAL( destroyed( QObject* ) ), myWatcher, SLOT( onDestroyed( QObject* ) ) );
    myQueueMutex.unlock();  

    if ( !IsBusy() )
      start();
  }
}

void PyInterp_Dispatcher::run()
{
//  MESSAGE("*** PyInterp_Dispatcher::run(): STARTED")
  PyInterp_Request* aRequest;

  // prepare for queue size check
  myQueueMutex.lock();

  while( myQueue.size() ) {
//    MESSAGE("*** PyInterp_Dispatcher::run(): next request taken from the queue")
    aRequest = myQueue.head();

    // let other threads append their requests to the end of the queue
    myQueueMutex.unlock();

    // processRequest() may delete a request, so this pointer must not be used
    // after request is processed!
    processRequest( aRequest );

    // prepare for removal of the first request in the queue
    myQueueMutex.lock();
    // IMPORTANT: the first item could have been removed by objectDestroyed() --> we have to check it
    if ( myQueue.head() == aRequest ) // It's still here --> remove it
      myQueue.dequeue();

//    MESSAGE("*** PyInterp_Dispatcher::run(): request processed")
  }

  myQueueMutex.unlock();
//  MESSAGE("*** PyInterp_Dispatcher::run(): FINISHED")
}

void PyInterp_Dispatcher::processRequest( PyInterp_Request* theRequest )
{
  theRequest->process();
}

void PyInterp_Dispatcher::objectDestroyed( const QObject* o )
{
  // prepare for modification of the queue
  myQueueMutex.lock();

  QMutableListIterator<RequestPtr> it( myQueue );
  while ( it.hasNext() )
  {
    RequestPtr r = it.next();
    if ( o == r->listener() )
    {
      r->setListener( 0 ); // to prevent event posting
      it.remove();
    }
  }

  myQueueMutex.unlock();
}
