
#include "PyInterp_Interp.h"  // !!! WARNING !!! THIS INCLUDE MUST BE THE VERY FIRST !!!
#include <pythread.h>

#include <cStringIO.h>
#include <structmember.h>

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>

#define TOP_HISTORY_PY   "--- top of history ---"
#define BEGIN_HISTORY_PY "--- begin of history ---"

// a map to store python thread states that have been created for a given system thread (key=thread id,value=thread state)
std::map<long,PyThreadState*> currentThreadMap;

/*!
  \class PyLockWrapper
  \brief Python GIL wrapper.
*/

/*!
  \brief Constructor. Automatically acquires GIL.
  \param theThreadState python thread state
*/
PyLockWrapper::PyLockWrapper(PyThreadState* theThreadState):
  myThreadState(theThreadState),
  mySaveThreadState(0)
{
  if (myThreadState->interp == PyInterp_Interp::_interp)
    _savestate = PyGILState_Ensure();
  else
    PyEval_AcquireThread(myThreadState);
}

/*!
  \brief Destructor. Automatically releases GIL.
*/
PyLockWrapper::~PyLockWrapper()
{
  if (myThreadState->interp == PyInterp_Interp::_interp)
    PyGILState_Release(_savestate);
  else
    PyEval_ReleaseThread(myThreadState);
}

/*!
  \brief Get Python GIL wrapper.
  \return GIL lock wrapper (GIL is automatically acquired here)
*/
PyLockWrapper PyInterp_Interp::GetLockWrapper()
{
  if (_tstate->interp == PyInterp_Interp::_interp)
    return _tstate;

  // If we are here, we have a secondary python interpreter. Try to get a thread state synchronized with the system thread
  long currentThreadid=PyThread_get_thread_ident(); // the system thread id
  PyThreadState* theThreadState;
  if(currentThreadMap.count(currentThreadid) != 0)
    {
      //a thread state exists for this thread id
      PyThreadState* oldThreadState=currentThreadMap[currentThreadid];
      if(_tstate->interp ==oldThreadState->interp)
        {
          //The old thread state has the same python interpreter as this one : reuse the threadstate
          theThreadState=oldThreadState;
        }
      else
        {
          //The old thread state has not the same python interpreter as this one : delete the old threadstate and create a new one
          PyEval_AcquireLock();
          PyThreadState_Clear(oldThreadState);
          PyThreadState_Delete(oldThreadState);
          PyEval_ReleaseLock();
          theThreadState=PyThreadState_New(_tstate->interp);
          currentThreadMap[currentThreadid]=theThreadState;
        }
    }
  else
    {
      // no old thread state for this thread id : create a new one
      theThreadState=PyThreadState_New(_tstate->interp);
      currentThreadMap[currentThreadid]=theThreadState;
    }
  return theThreadState;
}

/*
  The following functions are used to hook the Python
  interpreter output.
*/

static void
PyStdOut_dealloc(PyStdOut *self)
{
  PyObject_Del(self);
}

static PyObject*
PyStdOut_write(PyStdOut *self, PyObject *args)
{
  char *c;
  int l;
  if (!PyArg_ParseTuple(args, "t#:write",&c, &l))
    return NULL;
  if(self->_cb==NULL) {
    if ( self->_iscerr )
      std::cerr << c ;
    else
      std::cout << c ;
  }
  else {
    self->_cb(self->_data,c);
  }
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject*
PyStdOut_flush(PyStdOut *self)
{
  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef PyStdOut_methods[] = {
  {"write",  (PyCFunction)PyStdOut_write,  METH_VARARGS, PyDoc_STR("write(string) -> None")},
  {"flush",  (PyCFunction)PyStdOut_flush,  METH_NOARGS,  PyDoc_STR("flush() -> None")},
  {NULL,    NULL}   /* sentinel */
};

static PyMemberDef PyStdOut_memberlist[] = {
  {(char*)"softspace", T_INT,  offsetof(PyStdOut, softspace), 0,
   (char*)"flag indicating that a space needs to be printed; used by print"},
  {NULL} /* Sentinel */
};

static PyTypeObject PyStdOut_Type = {
  /* The ob_type field must be initialized in the module init function
   * to be portable to Windows without using C++. */
  PyObject_HEAD_INIT(NULL)
  0,                            /*ob_size*/
  "PyOut",                      /*tp_name*/
  sizeof(PyStdOut),             /*tp_basicsize*/
  0,                            /*tp_itemsize*/
  /* methods */
  (destructor)PyStdOut_dealloc, /*tp_dealloc*/
  0,                            /*tp_print*/
  0,                            /*tp_getattr*/
  0,                            /*tp_setattr*/
  0,                            /*tp_compare*/
  0,                            /*tp_repr*/
  0,                            /*tp_as_number*/
  0,                            /*tp_as_sequence*/
  0,                            /*tp_as_mapping*/
  0,                            /*tp_hash*/
  0,                            /*tp_call*/
  0,                            /*tp_str*/
  PyObject_GenericGetAttr,      /*tp_getattro*/
  /* softspace is writable:  we must supply tp_setattro */
  PyObject_GenericSetAttr,      /* tp_setattro */
  0,                            /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT,           /*tp_flags*/
  0,                            /*tp_doc*/
  0,                            /*tp_traverse*/
  0,                            /*tp_clear*/
  0,                            /*tp_richcompare*/
  0,                            /*tp_weaklistoffset*/
  0,                            /*tp_iter*/
  0,                            /*tp_iternext*/
  PyStdOut_methods,             /*tp_methods*/
  PyStdOut_memberlist,          /*tp_members*/
  0,                            /*tp_getset*/
  0,                            /*tp_base*/
  0,                            /*tp_dict*/
  0,                            /*tp_descr_get*/
  0,                            /*tp_descr_set*/
  0,                            /*tp_dictoffset*/
  0,                            /*tp_init*/
  0,                            /*tp_alloc*/
  0,                            /*tp_new*/
  0,                            /*tp_free*/
  0,                            /*tp_is_gc*/
};

#define PyStdOut_Check(v)  ((v)->ob_type == &PyStdOut_Type)

static PyStdOut* newPyStdOut( bool iscerr )
{
  PyStdOut *self;
  self = PyObject_New(PyStdOut, &PyStdOut_Type);
  if (self == NULL)
    return NULL;
  self->softspace = 0;
  self->_cb = NULL;
  self->_iscerr = iscerr;
  return self;
}

/*!
  \class PyInterp_Interp
  \brief Generic embedded Python interpreter.
*/

int   PyInterp_Interp::_argc   = 1;
char* PyInterp_Interp::_argv[] = {(char*)""};
PyObject*           PyInterp_Interp::builtinmodule = NULL;
PyThreadState*      PyInterp_Interp::_gtstate      = NULL;
PyInterpreterState* PyInterp_Interp::_interp       = NULL;

/*!
  \brief Basic constructor.

  After construction the interpreter instance successor classes
  must call virtual method initalize().
*/
PyInterp_Interp::PyInterp_Interp():
  _tstate(0), _vout(0), _verr(0), _g(0)
{
}

/*!
  \brief Destructor.
*/
PyInterp_Interp::~PyInterp_Interp()
{
}

/*!
  \brief Initialize embedded interpreter.

  This method shoud be called after construction of the interpreter.
  The method initialize() calls virtuals methods
  - initPython()  to initialize global Python interpreter
  - initState()   to initialize embedded interpreter state
  - initContext() to initialize interpreter internal context
  - initRun()     to prepare interpreter for running commands
  which should be implemented in the successor classes, according to the
  embedded Python interpreter policy (mono or multi interpreter, etc).
*/
void PyInterp_Interp::initialize()
{
  _history.clear();       // start a new list of user's commands
  _ith = _history.begin();

  initPython();
  // Here the global lock is released

  initState();

  PyEval_AcquireThread(_tstate);

  initContext();

  // used to interpret & compile commands
  PyObjWrapper m(PyImport_ImportModule("codeop"));
  if(!m) {
    PyErr_Print();
    PyEval_ReleaseThread(_tstate);
    return;
  }

  // Create python objects to capture stdout and stderr
  _vout=(PyObject*)newPyStdOut( false ); // stdout
  _verr=(PyObject*)newPyStdOut( true );  // stderr

  // All the initRun outputs are redirected to the standard output (console)
  initRun();
  PyEval_ReleaseThread(_tstate);
}

/*!
  \brief Initialize Python interpreter.

  In case if Python is not initialized, it sets program name, initializes the interpreter, sets program arguments,
  initializes threads.
  Otherwise, it just obtains the global interpreter and thread states. This is important for light SALOME configuration,
  as in full SALOME this is done at SalomeApp level.
  \sa SalomeApp_PyInterp class
 */
void PyInterp_Interp::initPython()
{
  if (!Py_IsInitialized()){
    // Python is not initialized
    Py_SetProgramName(_argv[0]);
    Py_Initialize(); // Initialize the interpreter
    PySys_SetArgv(_argc, _argv);
    PyEval_InitThreads(); // Create (and acquire) the interpreter lock
  }

  if ( _interp == NULL )
    _interp = PyThreadState_Get()->interp;
  if (PyType_Ready(&PyStdOut_Type) < 0) {
    PyErr_Print();
  }
  if ( _gtstate == NULL )
    _gtstate = PyEval_SaveThread(); // Release global thread state
}

/*!
  \brief Get embedded Python interpreter banner.
  \return banner string
 */
std::string PyInterp_Interp::getbanner()
{
 // Should we take the lock ?
 // PyEval_RestoreThread(_tstate);
  std::string aBanner("Python ");
  aBanner = aBanner + Py_GetVersion() + " on " + Py_GetPlatform() ;
  aBanner = aBanner + "\ntype help to get general information on environment\n";
  //PyEval_SaveThread();
  return aBanner;
}

/*!
  \brief Initialize run command.

  This method is used to prepare interpreter for running
  Python commands.

  \return \c true on success and \c false on error
*/
bool PyInterp_Interp::initRun()
{
  //
  // probably all below code isn't required
  //
  /*
  PySys_SetObject("stderr",_verr);
  PySys_SetObject("stdout",_vout);

  //PyObject *m = PyImport_GetModuleDict();

  PySys_SetObject("stdout",PySys_GetObject("__stdout__"));
  PySys_SetObject("stderr",PySys_GetObject("__stderr__"));
  */
  return true;
}

/*!
  \brief Compile Python command and evaluate it in the
         python dictionary context if possible.
  \internal
  \param command Python command string
  \param context Python context (dictionary)
  \return -1 on fatal error, 1 if command is incomplete and 0
         if command is executed successfully
 */
static int run_command(const char *command, PyObject *context)
{
  PyObject *m = PyImport_AddModule("codeop");
  if(!m) { // Fatal error. No way to go on.
    PyErr_Print();
    return -1;
  }
  PyObjWrapper v(PyObject_CallMethod(m,(char*)"compile_command",(char*)"s",command));
  if(!v) {
    // Error encountered. It should be SyntaxError,
    //so we don't write out traceback
    PyObjWrapper exception, value, tb;
    PyErr_Fetch(&exception, &value, &tb);
    PyErr_NormalizeException(&exception, &value, &tb);
    PyErr_Display(exception, value, NULL);
    return -1;
  }
  else if (v == Py_None) {
    // Incomplete text we return 1 : we need a complete text to execute
    return 1;
  }
  else {
    // Complete and correct text. We evaluate it.
    //#if PY_VERSION_HEX < 0x02040000 // python version earlier than 2.4.0
    //    PyObjWrapper r(PyEval_EvalCode(v,context,context));
    //#else
    PyObjWrapper r(PyEval_EvalCode((PyCodeObject *)(void *)v,context,context));
    //#endif
    if(!r) {
      // Execution error. We return -1
      PyErr_Print();
      return -1;
    }
    // The command has been successfully executed. Return 0
    return 0;
  }
}

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}
/*!
  \brief Compile Python command and evaluate it in the
         python dictionary context if possible. Command might correspond to
         the execution of a script with optional arguments.
         In this case, command is:
         execfile(r"/absolute/path/to/script.py [args:arg1,...,argn]")
  \internal
  \param command Python command string
  \param context Python context (dictionary)
  \return -1 on fatal error, 1 if command is incomplete and 0
         if command is executed successfully
 */
static int compile_command(const char *command,PyObject *context)
{
  // First guess if command is execution of a script with args, or a simple Python command
  std::string singleCommand = command;
  std::string commandArgs = "";

  std::size_t pos = std::string(command).find("args:");
  if (pos != std::string::npos) {
    commandArgs = singleCommand.substr(pos+5);
    commandArgs = commandArgs.substr(0, commandArgs.length()-3);
    singleCommand = singleCommand.substr(0, pos-1)+"\")";
  }

  if (commandArgs.empty()) {
    // process command: expression
    // process command: execfile(r"/absolute/path/to/script.py") (no args)
    return run_command(singleCommand.c_str(), context);
  }
  else {
    // process command: execfile(r"/absolute/path/to/script.py [args:arg1,...,argn]")
    std::string script = singleCommand.substr(11); // remove leading execfile(r"
    script = script.substr(0, script.length()-2); // remove trailing ")

    std::string preCommandBegin = "import sys; save_argv = sys.argv; sys.argv=[";
    std::string preCommandEnd = "];";
    replaceAll(commandArgs, ",", "\",\"");
    commandArgs = "\""+commandArgs+"\"";
    std::string completeCommand = preCommandBegin+"\""+script+"\","+commandArgs+preCommandEnd+singleCommand+";sys.argv=save_argv";
    return run_command(completeCommand.c_str(), context);
  }
}

/*!
  \brief Run Python command.
  \param command Python command
  \return command status
*/
int PyInterp_Interp::run(const char *command)
{
  beforeRun();
  return simpleRun(command);
}

/*!
  \brief Run Python command (used internally).
  \param command Python command
  \param addToHistory if \c true (default), the command is added to the commands history
  \return command status
*/
int PyInterp_Interp::simpleRun(const char *command, const bool addToHistory)
{
  if( addToHistory && strcmp(command,"") != 0 ) {
    _history.push_back(command);
    _ith = _history.end();
  }

  // We come from C++ to enter Python world
  // We need to acquire the Python global lock
  //PyLockWrapper aLock(_tstate); // san - lock is centralized now

  // Reset redirected outputs before treatment
  PySys_SetObject((char*)"stderr",_verr);
  PySys_SetObject((char*)"stdout",_vout);

  int ier = compile_command(command,_g);

  // Outputs are redirected on standards outputs (console)
  PySys_SetObject((char*)"stdout",PySys_GetObject((char*)"__stdout__"));
  PySys_SetObject((char*)"stderr",PySys_GetObject((char*)"__stderr__"));

  return ier;
}

/*!
  \brief Get previous command in the commands history.
  \return previous command
*/
const char * PyInterp_Interp::getPrevious()
{
  if(_ith != _history.begin()){
    _ith--;
    return (*_ith).c_str();
  }
  else
    return BEGIN_HISTORY_PY;
}

/*!
  \brief Get next command in the commands history.
  \return next command
*/
const char * PyInterp_Interp::getNext()
{
  if(_ith != _history.end()){
    _ith++;
  }
  if (_ith == _history.end())
    return TOP_HISTORY_PY;
  else
    return (*_ith).c_str();
}

/*!
  \brief Set Python standard output device hook.
  \param cb callback function
  \param data callback function parameters
*/
void PyInterp_Interp::setvoutcb(PyOutChanged* cb, void* data)
{
  ((PyStdOut*)_vout)->_cb=cb;
  ((PyStdOut*)_vout)->_data=data;
}

/*!
  \brief Set Python standard error device hook.
  \param cb callback function
  \param data callback function parameters
*/
void PyInterp_Interp::setverrcb(PyOutChanged* cb, void* data)
{
  ((PyStdOut*)_verr)->_cb=cb;
  ((PyStdOut*)_verr)->_data=data;
}
