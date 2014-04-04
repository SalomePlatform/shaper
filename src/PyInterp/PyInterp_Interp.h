
#ifndef PYINTERP_INTERP_H
#define PYINTERP_INTERP_H

#include "PyInterp.h"   // !!! WARNING !!! THIS INCLUDE MUST BE THE VERY FIRST !!!

#include <list>
#include <string>

//PYINTERP_EXPORT
class PyLockWrapper
{
  PyThreadState* myThreadState;
  PyThreadState* mySaveThreadState;
  PyGILState_STATE _savestate;
public:
  PyLockWrapper(PyThreadState* theThreadState);
  ~PyLockWrapper();
};

typedef void PyOutChanged(void* data,char * c);

//PYINTERP_EXPORT
class PyInterp_Interp
{
public:
  static int _argc;
  static char* _argv[];
  static PyObject *builtinmodule;
  static PyThreadState *_gtstate;
  static PyInterpreterState *_interp;
  
  PyInterp_Interp();
  virtual ~PyInterp_Interp();
  
  void initialize();

  virtual int run(const char *command); 

  PyLockWrapper GetLockWrapper();

  std::string getbanner(); 
  void setverrcb(PyOutChanged*,void*);
  void setvoutcb(PyOutChanged*,void*);

  const char * getPrevious();
  const char * getNext();    

protected:
  PyThreadState * _tstate;
  PyObject * _vout;
  PyObject * _verr;
  PyObject * _g;
  PyObject * _codeop;
  std::list<std::string> _history;
  std::list<std::string>::iterator _ith;

  virtual int beforeRun() { return 0; }
  int simpleRun(const char* command, const bool addToHistory = true);

  virtual bool initRun();
  virtual void initPython();
  virtual bool initState() = 0;
  virtual bool initContext() = 0;  
};

//PYINTERP_EXPORT
class PyObjWrapper
{
  PyObject* myObject;
public:
  PyObjWrapper(PyObject* theObject) : myObject(theObject) {}
  PyObjWrapper() : myObject(0) {}
  virtual ~PyObjWrapper() { Py_XDECREF(myObject); }

  operator PyObject*()    { return myObject;  }
  PyObject* operator->()  { return myObject;  }
  PyObject* get()         { return myObject;  }
  bool operator!()        { return !myObject; }
  bool operator==(PyObject* theObject) { return myObject == theObject; }
  PyObject** operator&()  { return &myObject; }
  PyObjWrapper& operator=(PyObjWrapper* theObjWrapper)
  {
    Py_XDECREF(myObject);
    myObject = theObjWrapper->myObject;
    return *this;
  }
};

typedef struct {
  PyObject_HEAD
  int softspace;
  PyOutChanged* _cb;
  void* _data;
  bool _iscerr;
} PyStdOut;

#endif // PYINTERP_INTERP_H
