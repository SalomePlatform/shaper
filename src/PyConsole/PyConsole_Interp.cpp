
#include "PyConsole_Interp.h"

/*!
  \class PyConsole_Interp
  \brief Python interpreter to be embedded to the SALOME study's GUI.

  Python interpreter is created one per SALOME study.

  Call initialize method defined in the base class PyInterp_Interp,
  to intialize interpreter after instance creation.

  The method initialize() calls virtuals methods
  - initPython()  to initialize global Python interpreter
  - initState()   to initialize embedded interpreter state
  - initContext() to initialize interpreter internal context
  - initRun()     to prepare interpreter for running commands

  /EDF-CCAR/
  When SALOME uses multi Python interpreter feature, 
  every study has its own interpreter and thread state (_tstate = Py_NewInterpreter()).
  This is fine because every study has its own modules (sys.modules) stdout and stderr.

  <b>But</b> some Python modules must be imported only once. In multi interpreter 
  context Python modules (*.py) are imported several times.
  For example, the PyQt module must be imported only once because 
  it registers classes in a C module.

  It's quite the same with omniorb modules (internals and generated with omniidl).

  This problem is handled with "shared modules" defined in salome_shared_modules.py.
  These "shared modules" are imported only once and only copied in all 
  the other interpreters.

  <b>But</b> it's not the only problem. Every interpreter has its own 
  __builtin__ module. That's fine but if we have copied some modules 
  and imported others problems may arise with operations that are not allowed
  in restricted execution environment. So we must impose that all interpreters
  have identical __builtin__ module.
*/

/*!
  \brief Constructor.

  Creates new python interpreter.
*/
PyConsole_Interp::PyConsole_Interp(): PyInterp_Interp()
{
}

/*!
  \brief Destructor.

  Does nothing for the moment.
*/
PyConsole_Interp::~PyConsole_Interp()
{
}
 
/*!
  \brief Initialize internal Python interpreter state.

  When calling initState the GIL is not held
  It must not be held on exit

  \return \c true on success
*/
bool PyConsole_Interp::initState()
{
  PyEval_AcquireLock();
  _tstate = Py_NewInterpreter(); // create an interpreter and save current state
  PySys_SetArgv(PyInterp_Interp::_argc,PyInterp_Interp::_argv); // initialize sys.argv
  
  if(!builtinmodule) // PAL18041: deepcopy function don't work in Salome
  {
    //builtinmodule is static member of PyInterp class
    //If it is not NULL (initialized to the builtin module of the main interpreter
    //all the sub interpreters will have the same builtin
    //_interp is a static member and is the main interpreter
    //The first time we initialized it to the builtin of main interpreter
    builtinmodule=PyDict_GetItemString(_interp->modules, "__builtin__");
  }

  //If builtinmodule has been initialized all the sub interpreters
  // will have the same __builtin__ module
  if(builtinmodule){ 
    PyObject *m = PyImport_GetModuleDict();
    PyDict_SetItemString(m, "__builtin__", builtinmodule);
    _tstate->interp->builtins = PyModule_GetDict(builtinmodule);
    Py_INCREF(_tstate->interp->builtins);
  }
  PyEval_ReleaseThread(_tstate);
  return true;
}

/*!
  \brief Initialize python interpeter context.

  The GIL is assumed to be held.
  It is the caller responsability to acquire the GIL.
  It must still be held on initContext() exit.

  \return \c true on success
*/
bool PyConsole_Interp::initContext()
{
  PyObject *m = PyImport_AddModule("__main__");  // interpreter main module (module context)
  if(!m){
    PyErr_Print();
    return false;
  }  
  _g = PyModule_GetDict(m);          // get interpreter dictionnary context

  if(builtinmodule){
    PyDict_SetItemString(_g, "__builtins__", builtinmodule); // assign singleton __builtin__ module
  }
  return true;
}
