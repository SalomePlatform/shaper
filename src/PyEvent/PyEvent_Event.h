
#ifndef PyEvent_PYEVENT_H
#define PyEvent_PYEVENT_H

#include "PyEvent.h"

#include <QEvent>

//! SALOME custom event type
#define PyEvent_EVENT QEvent::Type( QEvent::User + 10000 )

class PYEVENT_EXPORT PyEvent_CustomEvent : public QEvent
{
public:
  PyEvent_CustomEvent( int type );
  PyEvent_CustomEvent( QEvent::Type type, void* data );

  void* data() const;
  void  setData( void* data );

private:
  void *d;   //!< internal data
};

class QSemaphore;

class PYEVENT_EXPORT PyEvent_Event
{
public:
  PyEvent_Event();
  virtual ~PyEvent_Event();

  void            ExecutePostedEvent();
  virtual void    Execute() = 0;

  static bool     IsSessionThread();
  void            process();

protected:
  void            processed();
  static void     GetSessionThread();

private:
  QSemaphore*     mySemaphore;     //!< internal semaphore
};

template<class TObject, typename TRes> class TMemFunEvent : public PyEvent_Event
{
public:
  typedef TRes TResult;
  TResult myResult;
  typedef TResult (TObject::* TAction)();
  TMemFunEvent(TObject* theObject, TAction theAction, 
               TResult theResult = TResult()):
    myObject(theObject),
    myAction(theAction),
    myResult(theResult)
  {}
  virtual void Execute()
  {
    myResult = (myObject->*myAction)();
  }
private:
  TObject* myObject;
  TAction  myAction;
};

template<class TObject> class TVoidMemFunEvent : public PyEvent_Event
{
public:
  typedef void (TObject::* TAction)();
  TVoidMemFunEvent(TObject* theObject, TAction theAction):
    myObject(theObject),
    myAction(theAction)
  {}
  virtual void Execute()
  {
    (myObject->*myAction)();
  }
private:
  TObject* myObject;
  TAction myAction;
};

template<class TObject, typename TRes, typename TArg, typename TStoreArg = TArg> 
class TMemFun1ArgEvent : public PyEvent_Event
{
public:
  typedef TRes TResult;
  TResult myResult;
  typedef TResult (TObject::* TAction)(TArg);
  TMemFun1ArgEvent(TObject* theObject, TAction theAction, TArg theArg, 
                   TResult theResult = TResult()):
    myObject(theObject),
    myAction(theAction),
    myResult(theResult),
    myArg(theArg)
  {}
  virtual void Execute()
  {
    myResult = (myObject->*myAction)(myArg);
  }
private:
  TObject* myObject;
  TAction myAction;
  TStoreArg myArg;
};

template<class TObject, typename TArg, typename TStoreArg = TArg> 
class TVoidMemFun1ArgEvent : public PyEvent_Event
{
public:
  typedef void (TObject::* TAction)(TArg);
  TVoidMemFun1ArgEvent(TObject* theObject, TAction theAction, TArg theArg):
    myObject(theObject),
    myAction(theAction),
    myArg(theArg)
  {}
  virtual void Execute()
  {
    (myObject->*myAction)(myArg);
  }
private:
  TObject* myObject;
  TAction myAction;
  TStoreArg myArg;
};

template<class TObject, typename TRes, typename TArg, typename TArg1, typename TStoreArg = TArg, typename TStoreArg1 = TArg1>
class TMemFun2ArgEvent: public PyEvent_Event
{
public:
  typedef TRes TResult;
  TResult myResult;
  typedef TResult (TObject::* TAction)(TArg,TArg1);
  TMemFun2ArgEvent(TObject* theObject, TAction theAction, 
                   TArg theArg, TArg1 theArg1,
                   TResult theResult = TResult()):
    myObject(theObject),
    myAction(theAction),
    myResult(theResult),
    myArg(theArg),
    myArg1(theArg1)
  {}
  virtual void Execute()
  {
    myResult = (myObject->*myAction)(myArg,myArg1);
  }
private:
  TObject* myObject;
  TAction myAction;
  TStoreArg myArg;
  TStoreArg1 myArg1;
};

template<class TObject, typename TArg, typename TArg1, typename TStoreArg = TArg, typename TStoreArg1 = TArg1>
class TVoidMemFun2ArgEvent : public PyEvent_Event
{
public:
  typedef void (TObject::* TAction)(TArg,TArg1);
  TVoidMemFun2ArgEvent(TObject* theObject, TAction theAction, TArg theArg, TArg1 theArg1):
    myObject(theObject),
    myAction(theAction),
    myArg(theArg),
    myArg1(theArg1)
  {}
  virtual void Execute()
  {
    (myObject->*myAction)(myArg,myArg1);
  }
private:
  TObject* myObject;
  TAction myAction;
  TStoreArg myArg;
  TStoreArg1 myArg1;
};

template<class TEvent> inline typename TEvent::TResult ProcessEvent(TEvent* theEvent)
{
  typename TEvent::TResult aResult;
  if(PyEvent_Event::IsSessionThread()) {
    theEvent->Execute();
    aResult = theEvent->myResult;
  }
  else {
    theEvent->process();
    aResult = theEvent->myResult;
  }
  delete theEvent;
  return aResult;
}

inline void ProcessVoidEvent(PyEvent_Event* theEvent)
{
  if(PyEvent_Event::IsSessionThread()) {
    theEvent->Execute();
  }
  else {
    theEvent->process();
  }
  delete theEvent;
}

#endif // PyEvent_PYEVENT_H
