/*
 * ParametersPlugin_PyInterp.cpp
 *
 *  Created on: Apr 2, 2015
 *      Author: sbh
 */

#include <ParametersPlugin_PyInterp.h>

#include <string>

ParametersPlugin_PyInterp::ParametersPlugin_PyInterp()
: PyInterp_Interp()
{
}

ParametersPlugin_PyInterp::~ParametersPlugin_PyInterp()
{
}

std::list<std::string> ParametersPlugin_PyInterp::compile(const std::string& theExpression)
{
  PyLockWrapper lck; // Acquire GIL until the end of the method
  std::list<std::string> aResult;
  PyObject *aCodeopModule = PyImport_AddModule("codeop");
  if(!aCodeopModule) { // Fatal error. No way to go on.
    PyErr_Print();
    return aResult;
  }

  PyObject *aCodePyObj =
      PyObject_CallMethod(aCodeopModule, "compile_command", "(s)", theExpression.c_str());

  if(!aCodePyObj || aCodePyObj == Py_None || !PyCode_Check(aCodePyObj)) {
    Py_XDECREF(aCodePyObj);
    return aResult;
  }

  PyCodeObject* aCodeObj = (PyCodeObject*) aCodePyObj;
  std::string aCodeName(PyString_AsString(aCodeObj->co_code));
  // co_names should be tuple, but can be changed in modern versions of python (>2.7.3)
  if(!PyTuple_Check(aCodeObj->co_names))
    return aResult;
  
  int params_size = PyTuple_Size(aCodeObj->co_names);
  if (params_size > 0) {
    for (int i = 0; i < params_size; i++) {
      PyObject* aParamObj = PyTuple_GetItem(aCodeObj->co_names, i);
      PyObject* aParamObjStr = PyObject_Str(aParamObj);
      std::string aParamName(PyString_AsString(aParamObjStr));
      aResult.push_back(aParamName);
      Py_XDECREF(aParamObjStr);
    }
  }
  Py_XDECREF(aCodeObj);
  return aResult;
}

void ParametersPlugin_PyInterp::extendLocalContext(const std::list<std::string>& theParameters)
{
  PyLockWrapper lck; // Acquire GIL until the end of the method
  if (theParameters.empty())
    return;
  std::list<std::string>::const_iterator it = theParameters.begin();
  for ( ; it != theParameters.cend(); it++) {
    std::string aParamValue = *it;
    simpleRun(aParamValue.c_str(), false);
  }
}


double ParametersPlugin_PyInterp::evaluate(const std::string& theExpression, std::string& theError)
{
  PyLockWrapper lck; // Acquire GIL until the end of the method
  PyCodeObject* anExprCode = (PyCodeObject *) Py_CompileString(theExpression.c_str(),
                                                               "<string>", Py_eval_input);
  if(!anExprCode) {
    theError = errorMessage();
    Py_XDECREF(anExprCode);
    return 0.;
  }

  PyObject* anEvalResult = PyEval_EvalCode(anExprCode, _global_context, _local_context);
  if(!anEvalResult) {
    theError = errorMessage();
    Py_XDECREF(anExprCode);
    Py_XDECREF(anEvalResult);
    return 0.;
  }

  PyObject* anEvalStrObj = PyObject_Str(anEvalResult);
  std::string anEvalStr(PyString_AsString(anEvalStrObj));
  Py_XDECREF(anExprCode);
  Py_XDECREF(anEvalResult);
  Py_XDECREF(anEvalStrObj);
  double result = 0.;
  try {
    result = std::stod(anEvalStr);
  } catch (const std::invalid_argument&) {
    theError = "Unable to eval " + anEvalStr;
  }
  
  return result;
}

std::string ParametersPlugin_PyInterp::errorMessage()
{
  std::string aPyError;
  if (PyErr_Occurred()) {
    PyObject *pstr, *ptype, *pvalue, *ptraceback;
    PyErr_Fetch(&ptype, &pvalue, &ptraceback);
    PyErr_NormalizeException(&ptype, &pvalue, &ptraceback);
    pstr = PyObject_Str(pvalue);
    aPyError = std::string(PyString_AsString(pstr));
    Py_XDECREF(pstr);
    Py_XDECREF(ptype);
    Py_XDECREF(pvalue);
    Py_XDECREF(ptraceback);
  }
  return aPyError;
}

bool ParametersPlugin_PyInterp::initContext()
{
  PyObject *m = PyImport_AddModule("__main__");  // interpreter main module (module context)
  if(!m){
    PyErr_Print();
    return false;
  }
  _global_context = PyModule_GetDict(m);          // get interpreter global variable context
  Py_INCREF(_global_context);
  _local_context = _global_context;

  return PyRun_SimpleString("from math import *") == 0;
}
