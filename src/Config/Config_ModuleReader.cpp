// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include <pyconfig.h>

#include <Config_Keywords.h>
#include <Config_Common.h>
#include <Config_ModuleReader.h>
#include <Config_FeatureReader.h>
#include <Config_PluginMessage.h>
#include <Events_InfoMessage.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

// Have to be included before std headers
#include <Python.h>
#include <structmember.h>

//Necessary for cerr
#include <iostream>
#include <algorithm>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable : 4996) // for getenv
#else
#include <dlfcn.h>
#endif

std::map<std::string, Config_ModuleReader::PluginType> Config_ModuleReader::myPluginTypes;
std::set<std::string> Config_ModuleReader::myDependencyModules;

Config_ModuleReader::Config_ModuleReader(const char* theEventGenerated)
    : Config_XMLReader(PLUGIN_FILE),
      myEventGenerated(theEventGenerated)
{
}

Config_ModuleReader::~Config_ModuleReader()
{
}

const std::map<std::string, std::string>& Config_ModuleReader::featuresInFiles() const
{
  return myFeaturesInFiles;
}

const std::set<std::string>& Config_ModuleReader::modulePluginFiles() const
{
  return myPluginFiles;
}

/*!
 * Get module name from plugins.xml
 * (property "module")
 */
std::string Config_ModuleReader::getModuleName()
{
  xmlNodePtr aRoot = findRoot();
  return getProperty(aRoot, PLUGINS_MODULE);
}


void Config_ModuleReader::addFeature(const std::string& theFeatureName,
                                     const std::string& thePluginConfig)
{
  if (myFeaturesInFiles.count(theFeatureName)) {
    std::string anErrorMsg = "Can not register feature '%1' in plugin '%2'."
      " There is a feature with the same ID.";
    Events_InfoMessage("Config_ModuleReader", anErrorMsg)
      .arg(theFeatureName).arg(thePluginConfig).send();
    return;
  }

  myFeaturesInFiles[theFeatureName] = thePluginConfig;
}

void Config_ModuleReader::processNode(xmlNodePtr theNode)
{
  if (isNode(theNode, NODE_PLUGIN, NULL)) {
    if (!hasRequiredModules(theNode))
      return;
    std::string aPluginConf = getProperty(theNode, PLUGIN_CONFIG);
    if (!aPluginConf.empty())
      myPluginFiles.insert(aPluginConf);
    std::string aPluginLibrary = getProperty(theNode, PLUGIN_LIBRARY);
    std::string aPluginScript = getProperty(theNode, PLUGIN_SCRIPT);
    std::string aPluginName = addPlugin(aPluginLibrary, aPluginScript, aPluginConf);
    std::string aPluginDocSection = getProperty(theNode, PLUGIN_DOCSECTION);
    std::string aUsesPlugin = getProperty(theNode, PLUGIN_USES);
    if (!aUsesPlugin.empty()) { // send information about the plugin dependencies
      std::shared_ptr<Config_PluginMessage> aMess(new Config_PluginMessage(
        Events_Loop::loop()->eventByName(Config_PluginMessage::EVENT_ID()), aPluginName));
      aMess->setUses(aUsesPlugin);
      Events_Loop::loop()->send(aMess);
    }

    std::list<std::string> aFeatures = importPlugin(aPluginName, aPluginConf, aPluginDocSection);
    std::list<std::string>::iterator it = aFeatures.begin();
    for (; it != aFeatures.end(); it++) {
      addFeature(*it, aPluginConf);
    }
  }
}

bool Config_ModuleReader::processChildren(xmlNodePtr theNode)
{
  return isNode(theNode, NODE_PLUGINS, NULL);
}

bool Config_ModuleReader::hasRequiredModules(xmlNodePtr theNode) const
{
  std::string aRequiredModule = normalize(getProperty(theNode, PLUGIN_DEPENDENCY));
  if(aRequiredModule.empty())
    return true;
  std::set<std::string>::iterator it = myDependencyModules.begin();
  for ( ; it != myDependencyModules.end(); it++ ) {
    if (*it == aRequiredModule) return true;
  }
  return false;
}

std::list<std::string> Config_ModuleReader::importPlugin(const std::string& thePluginLibrary,
                                                         const std::string& thePluginXmlConf,
                                                         const std::string& thePluginDocSection)
{
  if (thePluginXmlConf.empty()) {  //probably a third party library
    loadLibrary(thePluginLibrary);
    return std::list<std::string>();
  }

  Config_FeatureReader aReader = Config_FeatureReader(thePluginXmlConf,
                                                      thePluginLibrary,
                                                      thePluginDocSection,
                                                      myEventGenerated);
  aReader.readAll();
  return aReader.features();
}

std::string Config_ModuleReader::addPlugin(const std::string& aPluginLibrary,
                                           const std::string& aPluginScript,
                                           const std::string& aPluginConf)
{
  PluginType aType = Config_ModuleReader::Binary;
  std::string aPluginName;
  if (!aPluginLibrary.empty()) {
    aPluginName = aPluginLibrary;
    if (aPluginConf.empty()) {
      aType = Config_ModuleReader::Intrenal;
    }
  } else if (!aPluginScript.empty()) {
    aPluginName = aPluginScript;
    aType = Config_ModuleReader::Python;
  }
  if(!aPluginName.empty()) {
    myPluginTypes[aPluginName] = aType;
  }
  addDependencyModule(aPluginName);
  return aPluginName;
}

void Config_ModuleReader::loadPlugin(const std::string& thePluginName)
{
  // informs model that plugin loading is started
  static const Events_ID kEVENT_ID =
    Events_Loop::loop()->eventByName(Config_PluginMessage::EVENT_ID());
  std::shared_ptr<Config_PluginMessage> aMess(new Config_PluginMessage(kEVENT_ID, thePluginName));
  Events_Loop::loop()->send(aMess);

  PluginType aType = Config_ModuleReader::Binary;
  if(myPluginTypes.find(thePluginName) != myPluginTypes.end()) {
    aType = myPluginTypes.at(thePluginName);
  }
  switch (aType) {
    case Config_ModuleReader::Python:
      loadScript(thePluginName);
      break;
    case Config_ModuleReader::Binary:
    case Config_ModuleReader::Intrenal:
    default:
      loadLibrary(thePluginName);
      break;
  }
}

namespace
{
  // Handle Python exception
  // Reuse code from KERNEL module
  
  typedef struct
  {
    PyObject_HEAD
    int softspace;
    std::string *out;
  } PyStdOut;

  static void
  PyStdOut_dealloc(PyStdOut *self)
  {
    PyObject_Del(self);
  }

  static PyObject*
  PyStdOut_write(PyStdOut* self, PyObject* args)
  {
    char *c;
    if (!PyArg_ParseTuple(args, "s", &c))
      return NULL;

    *(self->out) = *(self->out) + c;

    Py_INCREF(Py_None);
    return Py_None;
  }

  static PyMethodDef PyStdOut_methods[] =
  {
    {"write",  (PyCFunction)PyStdOut_write,  METH_VARARGS,
      PyDoc_STR("write(string) -> None")},
    {0, 0, 0, 0}  /* sentinel */
  };

  static PyMemberDef PyStdOut_memberlist[] =
  {
    {(char*)"softspace", T_INT, offsetof(PyStdOut, softspace), 0,
     (char*)"flag indicating that a space needs to be printed; used by print"},
    {0, 0, 0, 0, 0}   /* sentinel */
  };

  static PyTypeObject PyStdOut_Type =
  {
    /* The ob_type field must be initialized in the module init function
     * to be portable to Windows without using C++. */
    PyVarObject_HEAD_INIT(NULL, 0)
    /* 0, */                      /*ob_size*/
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
    0,                            /*tp_bases*/
    0,                            /*tp_mro*/
    0,                            /*tp_cache*/
    0,                            /*tp_subclasses*/
    0,                            /*tp_weaklist*/
    0,                            /*tp_del*/
    0,                            /*tp_version_tag*/
    0,                            /*tp_finalize*/
  };

  PyObject* newPyStdOut(std::string& out)
  {
    PyStdOut* self = PyObject_New(PyStdOut, &PyStdOut_Type);
    if (self) {
      self->softspace = 0;
      self->out=&out;
    }
    return (PyObject*)self;
  }

  std::string parseException()
  {
    std::string error;
    if (PyErr_Occurred())
    {
      PyObject* new_stderr = newPyStdOut(error);
      PyObject* old_stderr = PySys_GetObject((char*)"stderr");
      Py_INCREF(old_stderr);
      PySys_SetObject((char*)"stderr", new_stderr);
      PyErr_Print();
      PySys_SetObject((char*)"stderr", old_stderr);
      Py_DECREF(new_stderr);
    }
    return error;
  }
}

void Config_ModuleReader::loadScript(const std::string& theFileName, bool theSendErr)
{
  /* acquire python thread */
  PyGILState_STATE gstate = PyGILState_Ensure();

  PyObject* module = PyImport_ImportModule(theFileName.c_str());
  if (!module) {
    std::string anErrorMsg = "An error occurred while importing " + theFileName;
    // Get detailed error message (including traceback)
    if (PyErr_Occurred()) {
      std::string aPyError = parseException();
      if (!aPyError.empty())
        anErrorMsg += ":\n" + aPyError;
    }
    if (theSendErr)
      Events_InfoMessage("Config_ModuleReader", anErrorMsg).send();
  }

  /* release python thread */
  PyGILState_Release(gstate);
}

void Config_ModuleReader::loadLibrary(const std::string& theLibName)
{
  std::string aFileName = library(theLibName);
  if (aFileName.empty())
    return;

  #ifdef WIN32
  HINSTANCE aModLib = ::LoadLibraryA(aFileName.c_str());
  #else
  void* aModLib = dlopen( aFileName.c_str(), RTLD_LAZY | RTLD_GLOBAL );
  #endif
  if(!aModLib && theLibName != "DFBrowser") { // don't show error for internal debugging tool
// LCOV_EXCL_START
    std::string anErrorMsg = "Failed to load " + aFileName;
    #ifdef WIN32
    DWORD   dwLastError = ::GetLastError();
    LPSTR messageBuffer = NULL;
    size_t size = ::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                   FORMAT_MESSAGE_FROM_SYSTEM |
                                   FORMAT_MESSAGE_IGNORE_INSERTS,
                                   NULL,
                                   dwLastError,
                                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                   (LPSTR)&messageBuffer, 0, NULL);
    anErrorMsg += ": " +  std::string(messageBuffer, size);
    #else
    anErrorMsg += ": " + std::string(dlerror());
    #endif
    std::cerr << anErrorMsg << std::endl;
    Events_InfoMessage("Config_ModuleReader", anErrorMsg).send();
// LCOV_EXCL_STOP
  }
}

void Config_ModuleReader::addDependencyModule(const std::string& theModuleName)
{
  myDependencyModules.insert(normalize(theModuleName));
}

