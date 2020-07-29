// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <InitializationPlugin_PyInterp.h>

#include <Locale_Convert.h>

#include <string>
#include <stdexcept>
#include <clocale>

InitializationPlugin_PyInterp::InitializationPlugin_PyInterp()
: PyInterp_Interp()
{
}

InitializationPlugin_PyInterp::~InitializationPlugin_PyInterp()
{
}

const char* aSearchCode =
  "import ast\n"
  "class FindName(ast.NodeVisitor):\n"
  "    def __init__(self, name):\n"
  "        self.name = name\n"
  "    def visit_Name(self, node):\n"
  "        if node.id == self.name:\n"
  "            positions.append((node.lineno, node.col_offset))\n"
  "FindName(name).visit(ast.parse(expression))";

// make the expression be correct for the python interpreter even for the
// beta=alfa*2 expressions
static std::wstring adjustExpression(const std::wstring& theExpression) {
  std::wstring anExpression = theExpression;
  if (!anExpression.empty() && anExpression.back() == L'=') {
    anExpression = anExpression.substr(0, anExpression.length() - 1);
  }
  return anExpression;
}

std::list<std::pair<int, int> >
InitializationPlugin_PyInterp::positions(const std::wstring& theExpression,
                                         const std::wstring& theName)
{
  PyLockWrapper lck; // Acquire GIL until the end of the method

  std::list<std::pair<int, int> > aResult;

  // prepare a context
  PyObject* aContext = PyDict_New();
  PyDict_SetItemString(aContext, "__builtins__", PyEval_GetBuiltins());

  std::wstring anExpression = adjustExpression(theExpression);
  // extend aContext with variables
  PyDict_SetItemString(aContext, "expression",
      PyUnicode_FromWideChar(anExpression.c_str(), anExpression.size()));
  PyDict_SetItemString(aContext, "name", PyUnicode_FromWideChar(theName.c_str(), theName.size()));
  PyDict_SetItemString(aContext, "positions", Py_BuildValue("[]"));

  // run the search code
  PyObject* aExecResult = PyRun_String(aSearchCode, Py_file_input, aContext, aContext);
  Py_XDECREF(aExecResult);

  // receive results from context
  PyObject* aPositions = PyDict_GetItemString(aContext, "positions");
  for (int anIndex = 0; anIndex < PyList_Size(aPositions); ++anIndex) {
    PyObject* aPosition = PyList_GetItem(aPositions, anIndex);
    PyObject* aLineNo = PyTuple_GetItem(aPosition, 0);
    PyObject* aColOffset = PyTuple_GetItem(aPosition, 1);

    aResult.push_back(
        std::pair<int, int>((int)PyLong_AsLong(aLineNo),
                            (int)PyLong_AsLong(aColOffset)));
  }

  // TODO(spo): after this refCount of the variable is not 0. Is there memory leak?
  Py_DECREF(aContext);

  return aResult;
}


std::list<std::wstring> InitializationPlugin_PyInterp::compile(const std::wstring& theExpression)
{
  PyLockWrapper lck; // Acquire GIL until the end of the method
  std::list<std::wstring> aResult;
  PyObject *aCodeopModule = PyImport_AddModule("codeop");
  if(!aCodeopModule) { // Fatal error. No way to go on.
    PyErr_Print();
    return aResult;
  }
  // support "variable_name=" expression as "variable_name"
  std::wstring anExpression = adjustExpression(theExpression);

  PyObject *aCodePyObj =
    PyObject_CallMethod(aCodeopModule, (char*)"compile_command", (char*)"(s)",
                        Locale::Convert::toString(anExpression).c_str());

  if(!aCodePyObj || aCodePyObj == Py_None || !PyCode_Check(aCodePyObj)) {
    Py_XDECREF(aCodePyObj);
    return aResult;
  }

  PyCodeObject* aCodeObj = (PyCodeObject*) aCodePyObj;
  std::string aCodeName(PyBytes_AsString(aCodeObj->co_code));
  // co_names should be tuple, but can be changed in modern versions of python (>2.7.3)
  if(!PyTuple_Check(aCodeObj->co_names)) {
    return aResult;
  }

  size_t params_size = PyTuple_Size(aCodeObj->co_names);
  if (params_size > 0) {
    for (size_t i = 0; i < params_size; i++) {
      PyObject* aParamObj = PyTuple_GetItem(aCodeObj->co_names, i);
      PyObject* aParamObjStr = PyObject_Str(aParamObj);
      Py_ssize_t aSize;
      std::wstring aParamName(PyUnicode_AsWideCharString(aParamObjStr, &aSize));
      aResult.push_back(aParamName);
      Py_XDECREF(aParamObjStr);
    }
  }
  Py_XDECREF(aCodeObj);
  return aResult;
}

void InitializationPlugin_PyInterp::extendLocalContext(const std::list<std::wstring>& theParameters)
{
  PyLockWrapper lck; // Acquire GIL until the end of the method
  if (theParameters.empty())
    return;
  std::list<std::wstring>::const_iterator it = theParameters.begin();
  for ( ; it != theParameters.cend(); it++) {
    std::string aParamValue = Locale::Convert::toString(*it);
    simpleRun(aParamValue.c_str(), false);
  }
}

void InitializationPlugin_PyInterp::clearLocalContext()
{
  PyLockWrapper lck;
  PyDict_Clear(_local_context);
}

double InitializationPlugin_PyInterp::evaluate(const std::wstring& theExpression,
                                               std::string& theError)
{
  // support "variable_name=" expression as "variable_name"
  std::wstring anExpression = adjustExpression(theExpression);

  PyLockWrapper lck; // Acquire GIL until the end of the method
  PyCompilerFlags aFlags = {CO_FUTURE_DIVISION};
  aFlags.cf_flags = CO_FUTURE_DIVISION;
  PyCodeObject* anExprCode = (PyCodeObject *) Py_CompileStringFlags(
                                Locale::Convert::toString(anExpression).c_str(),
                                "<string>", Py_eval_input, &aFlags);
  if(!anExprCode) {
    theError = errorMessage();
    Py_XDECREF(anExprCode);
    return 0.;
  }

  PyObject* anEvalResult = PyEval_EvalCode((PyObject *)anExprCode, _global_context, _local_context);
  if(!anEvalResult) {
    theError = errorMessage();
    Py_XDECREF(anExprCode);
    Py_XDECREF(anEvalResult);
    return 0.;
  }

  PyObject* anEvalStrObj = PyObject_Str(anEvalResult);
  std::string anEvalStr(PyUnicode_AsUTF8(anEvalStrObj));
  Py_XDECREF(anExprCode);
  Py_XDECREF(anEvalResult);
  Py_XDECREF(anEvalStrObj);
  double result = 0.;
  try {
    // set locale due to the #2485
    std::string aCurLocale = std::setlocale(LC_NUMERIC, 0);
    std::setlocale(LC_NUMERIC, "C");
    result = std::stod(anEvalStr);
    std::setlocale(LC_NUMERIC, aCurLocale.c_str());
  }
  catch (const std::invalid_argument&) {
    theError = "Unable to eval " + anEvalStr;
  }

  return result;
}

std::string InitializationPlugin_PyInterp::errorMessage()
{
  std::string aPyError;
  if (PyErr_Occurred()) {
    PyObject *pstr, *ptype, *pvalue, *ptraceback;
    PyErr_Fetch(&ptype, &pvalue, &ptraceback);
    PyErr_NormalizeException(&ptype, &pvalue, &ptraceback);
    pstr = PyObject_Str(pvalue);
    aPyError = std::string(PyUnicode_AsUTF8(pstr));
    Py_XDECREF(pstr);
    Py_XDECREF(ptype);
    Py_XDECREF(pvalue);
    Py_XDECREF(ptraceback);
  }
  return aPyError;
}

bool InitializationPlugin_PyInterp::initContext()
{
  PyObject *m = PyImport_AddModule("__main__");  // interpreter main module (module context)
  if(!m){
    PyErr_Print();
    return false;
  }
  _global_context = PyModule_GetDict(m);          // get interpreter global variable context
  Py_INCREF(_global_context);
  _local_context = PyDict_New();
  Py_INCREF(_local_context);

  // to avoid "help()" hang in the python console
  const static char* aHelpTxt = "def help(): print(\"Available modules:\\n"
    "  salome.shaper.model : higher level access to features and data model\\n"
    "  BuildAPI            : Build plugin features allowing to build shapes\\n"
    "  ConfigAPI           : configuration management: preferences and XML properties\\n"
    "  ConstructionAPI     : Construction plugin for auxiliary features creation\\n"
    "  EventsAPI           : application events receiving and emitting manager\\n"
    "  ExchangeAPI         : Exchange plugin with import/export features\\n"
    "  FeaturesAPI         : Features plugin with general 3D features\\n"
    "  GeomAlgoAPI         : geometrical algorithms\\n"
    "  GeomAPI             : geometrical data structures\\n"
    "  GeomDataAPI         : specific geometrical data structures stored in the data model\\n"
    "  ModelAPI            : general low-level interface to access data model\\n"
    "  ModelHighAPI        : general high-level interface to access data model\\n"
    "  ParametersAPI       : Parameters plugin for parameters feature management\\n"
    "  PartSetAPI          : PartSet plugin for management Parts features\\n"
    "  SketchAPI           : Sketch plugin with all sketch features\")";

  PyRun_SimpleString(aHelpTxt);

  return PyRun_SimpleString("from math import *") == 0;
}

void InitializationPlugin_PyInterp::closeContext()
{
  Py_XDECREF(_local_context);
  PyInterp_Interp::closeContext();
}

bool InitializationPlugin_PyInterp::runString(std::string theString)
{
  PyLockWrapper lck; // Acquire GIL until the end of the method
  return PyRun_SimpleString(theString.c_str());
}
