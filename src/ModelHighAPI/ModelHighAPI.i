// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

/* ModelHighAPI.i */
%module(directors="1") ModelHighAPI
%feature("director:except") {
    if ($error != NULL) {
      PyErr_Print();
      std::cerr << std::endl;
      throw Swig::DirectorMethodException();
    }
}

%{
  #include "ModelHighAPI_swig.h"

  // fix for SWIG v2.0.4
  #define SWIGPY_SLICE_ARG(obj) ((PyObject*)(obj))
%}

%include "doxyhelp.i"

// import other modules
%import "GeomAPI.i"
%import "ModelAPI.i"
%import "GeomDataAPI.i"

// to avoid error on this
#define MODELHIGHAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_list.i"
%include "std_pair.i"
%include "std_string.i"
%include "std_shared_ptr.i"

// directors
%feature("director") ModelHighAPI_Dumper;

// shared pointers
%shared_ptr(ModelHighAPI_Interface)
%shared_ptr(ModelHighAPI_Folder)

// typemaps

%typemap(in) const ModelHighAPI_Double & (ModelHighAPI_Double temp) {
  if (PyFloat_Check($input) || PyLong_Check($input)) {
    temp = ModelHighAPI_Double(PyFloat_AsDouble($input));
    $1 = &temp;
  } else if (PyUnicode_Check($input)) {
    temp = ModelHighAPI_Double(PyUnicode_AsUTF8($input));
    $1 = &temp;
  } else if ((SWIG_ConvertPtr($input, (void **)&$1, $1_descriptor, SWIG_POINTER_EXCEPTION)) == 0) {
  } else {
    PyErr_SetString(PyExc_ValueError, "argument must be ModelHighAPI_Double, float, int or string.");
    return NULL;
  }
}
%typecheck(SWIG_TYPECHECK_POINTER) ModelHighAPI_Double, const ModelHighAPI_Double & {
  $1 = ((PyFloat_Check($input) || PyLong_Check($input) || PyUnicode_Check($input)) && !PyBool_Check($input)) ? 1 : 0;
}

%typemap(in) const ModelHighAPI_Integer & (ModelHighAPI_Integer temp) {
  if (PyLong_Check($input)) {
    temp = ModelHighAPI_Integer(static_cast<int>(PyLong_AsLong($input)));
    $1 = &temp;
  } else if (PyUnicode_Check($input)) {
    temp = ModelHighAPI_Integer(PyUnicode_AsUTF8($input));
    $1 = &temp;
  } else if ((SWIG_ConvertPtr($input, (void **)&$1, $1_descriptor, SWIG_POINTER_EXCEPTION)) == 0) {
  } else {
    PyErr_SetString(PyExc_ValueError, "argument must be ModelHighAPI_Integer, int or string.");
    return NULL;
  }
}
%typecheck(SWIG_TYPECHECK_POINTER) ModelHighAPI_Integer, const ModelHighAPI_Integer & {
  $1 = ((PyLong_Check($input) || PyUnicode_Check($input)) && !PyBool_Check($input)) ? 1 : 0;
}

%typemap(in) const ModelHighAPI_RefAttr & (ModelHighAPI_RefAttr temp) {
  std::shared_ptr<ModelAPI_Attribute> * temp_attribute;
  std::shared_ptr<ModelAPI_Object> * temp_object;
  std::shared_ptr<ModelHighAPI_Interface> * temp_interface;
  int newmem = 0;
  if ((SWIG_ConvertPtrAndOwn($input, (void **)&temp_attribute, $descriptor(std::shared_ptr<ModelAPI_Attribute> *), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
    if (!temp_attribute) {
      PyErr_SetString(PyExc_TypeError, "argument must be ModelHighAPI_RefAttr, ModelHighAPI_Interface, ModelAPI_Attribute or ModelAPI_Object.");
      return NULL;
    }
    temp = ModelHighAPI_RefAttr(*temp_attribute);
    if (newmem & SWIG_CAST_NEW_MEMORY) {
      delete temp_attribute;
    }
    $1 = &temp;
  } else
  if ((SWIG_ConvertPtrAndOwn($input, (void **)&temp_object, $descriptor(std::shared_ptr<ModelAPI_Object> *), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
    if (!temp_object) {
      PyErr_SetString(PyExc_TypeError, "argument must be ModelHighAPI_RefAttr, ModelHighAPI_Interface, ModelAPI_Attribute or ModelAPI_Object.");
      return NULL;
    }
    temp = ModelHighAPI_RefAttr(*temp_object);
    if (newmem & SWIG_CAST_NEW_MEMORY) {
      delete temp_object;
    }
    $1 = &temp;
  } else
  if ((SWIG_ConvertPtrAndOwn($input, (void **)&temp_interface, $descriptor(std::shared_ptr<ModelHighAPI_Interface> *), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
    if (!temp_interface) {
      PyErr_SetString(PyExc_TypeError, "argument must be ModelHighAPI_RefAttr, ModelHighAPI_Interface, ModelAPI_Attribute or ModelAPI_Object.");
      return NULL;
    }
    temp = ModelHighAPI_RefAttr(*temp_interface);
    if (newmem & SWIG_CAST_NEW_MEMORY) {
      delete temp_interface;
    }
    $1 = &temp;
  } else
  if ((SWIG_ConvertPtr($input, (void **)&$1, $1_descriptor, SWIG_POINTER_EXCEPTION)) == 0) {
  } else {
    PyErr_SetString(PyExc_ValueError, "argument must be ModelHighAPI_RefAttr, ModelHighAPI_Interface, ModelAPI_Attribute or ModelAPI_Object.");
    return NULL;
  }
}

%typecheck(SWIG_TYPECHECK_POINTER) ModelHighAPI_RefAttr, const ModelHighAPI_RefAttr & {
  std::shared_ptr<ModelAPI_Attribute> * temp_attribute;
  std::shared_ptr<ModelAPI_Object> * temp_object;
  std::shared_ptr<ModelHighAPI_Interface> * temp_interface;
  ModelHighAPI_Selection* temp_selection;
  int newmem = 0;
  if ((SWIG_ConvertPtrAndOwn($input, (void **)&temp_attribute, $descriptor(std::shared_ptr<ModelAPI_Attribute> *), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
    if (temp_attribute) {
      $1 = 1;
    } else {
      $1 = 0;
    }
  } else if ((SWIG_ConvertPtrAndOwn($input, (void **)&temp_object, $descriptor(std::shared_ptr<ModelAPI_Object> *), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
    if (temp_object) {
      $1 = 1;
    } else {
      $1 = 0;
    }
  } else if ((SWIG_ConvertPtrAndOwn($input, (void **)&temp_interface, $descriptor(std::shared_ptr<ModelHighAPI_Interface> *), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
    if (temp_interface) {
      $1 = 1;
    } else {
      $1 = 0;
    }
  } else if ((SWIG_ConvertPtrAndOwn($input, (void **)&temp_selection, $descriptor(ModelHighAPI_Selection*), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
    if (temp_selection) {
      $1 = 1;
    } else {
      $1 = 0;
    }
  } else {
    $1 = 0;
  }
}

%typemap(in) const ModelHighAPI_Reference & (ModelHighAPI_Reference temp) {
  std::shared_ptr<ModelAPI_Object> * temp_object;
  std::shared_ptr<ModelHighAPI_Interface> * temp_interface;
  int newmem = 0;
  if ((SWIG_ConvertPtrAndOwn($input, (void **)&temp_object, $descriptor(std::shared_ptr<ModelAPI_Object> *), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
    if (!temp_object) {
      PyErr_SetString(PyExc_TypeError, "argument must be ModelHighAPI_Interface or ModelAPI_Object.");
      return NULL;
    }
    temp = ModelHighAPI_Reference(*temp_object);
    if (newmem & SWIG_CAST_NEW_MEMORY) {
      delete temp_object;
    }
    $1 = &temp;
  } else
  if ((SWIG_ConvertPtrAndOwn($input, (void **)&temp_interface, $descriptor(std::shared_ptr<ModelHighAPI_Interface> *), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
    if (!temp_interface) {
      PyErr_SetString(PyExc_TypeError, "argument must be ModelHighAPI_Interface or ModelAPI_Object.");
      return NULL;
    }
    temp = ModelHighAPI_Reference(*temp_interface);
    if (newmem & SWIG_CAST_NEW_MEMORY) {
      delete temp_interface;
    }
    $1 = &temp;
  } else
  if ((SWIG_ConvertPtr($input, (void **)&$1, $1_descriptor, SWIG_POINTER_EXCEPTION)) == 0) {
  } else {
    PyErr_SetString(PyExc_TypeError, "argument must be ModelHighAPI_Interface or ModelAPI_Object.");
    return NULL;
  }
}

%typecheck(SWIG_TYPECHECK_POINTER) ModelHighAPI_Reference, const ModelHighAPI_Reference & {
  std::shared_ptr<ModelAPI_Object> * temp_object;
  std::shared_ptr<ModelHighAPI_Interface> * temp_interface;
  int newmem = 0;
  if ((SWIG_ConvertPtrAndOwn($input, (void **)&temp_object, $descriptor(std::shared_ptr<ModelAPI_Object> *), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
    if (temp_object) {
      $1 = 1;
    } else {
      $1 = 0;
    }
  } else
  if ((SWIG_ConvertPtrAndOwn($input, (void **)&temp_interface, $descriptor(std::shared_ptr<ModelHighAPI_Interface> *), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
    if (temp_interface) {
      $1 = 1;
    } else {
      $1 = 0;
    }
  } else {
    $1 = 0;
  }
}

%typemap(out) const ModelHighAPI_Selection & {
  $1_basetype * ptr = new $1_basetype(*$1);
  $result = SWIG_NewPointerObj( (void*) ptr, $1_descriptor, 1 );
}

%typemap(out) const ModelHighAPI_Reference & {
  $1_basetype * ptr = new $1_basetype(*$1);
  $result = SWIG_NewPointerObj( (void*) ptr, $1_descriptor, 1 );
}

// std::list -> []
%template(SelectionList) std::list<ModelHighAPI_Selection>;
%template(RefAttrList) std::list<ModelHighAPI_RefAttr>;
%template(RefList) std::list<ModelHighAPI_Reference>;

// std::pair -> []
%template(ResultSubShapePair) std::pair<std::shared_ptr<ModelAPI_Result>, std::shared_ptr<GeomAPI_Shape> >;


// fix compilarion error: 'res*' was not declared in this scope
%typemap(freearg) const std::list<ModelHighAPI_RefAttr> & {}
%typemap(freearg) const std::list<std::shared_ptr<ModelAPI_Object> > & {}

%typemap(in) const std::list<ModelHighAPI_RefAttr> & (std::list<ModelHighAPI_RefAttr> temp) {
  std::shared_ptr<ModelAPI_Attribute> * temp_attribute;
  int newmem = 0;
  if (PySequence_Check($input)) {
    for (Py_ssize_t i = 0; i < PySequence_Size($input); ++i) {
      PyObject * item = PySequence_GetItem($input, i);
      if ((SWIG_ConvertPtrAndOwn(item, (void **)&temp_attribute, $descriptor(std::shared_ptr<ModelAPI_Attribute> *), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
        if (!temp_attribute) {
          PyErr_SetString(PyExc_TypeError, "argument must be list of ModelHighAPI_RefAttr, ModelHighAPI_Interface, ModelAPI_Attribute or ModelAPI_Object.");
          return NULL;
        }
        temp.push_back(ModelHighAPI_RefAttr(*temp_attribute));
        if (newmem & SWIG_CAST_NEW_MEMORY) {
          delete temp_attribute;
        }
      }
      Py_DECREF(item);
    }
    $1 = &temp;
  } else {
    PyErr_SetString(PyExc_ValueError, "argument must be list of ModelHighAPI_RefAttr, ModelHighAPI_Interface, ModelAPI_Attribute or ModelAPI_Object.");
    return NULL;
  }
}

%typemap(in) const std::list<std::shared_ptr<ModelAPI_Object> > & (std::list<std::shared_ptr<ModelAPI_Object> > temp) {
  std::shared_ptr<ModelAPI_Object> * temp_object;
  std::shared_ptr<ModelHighAPI_Interface> * temp_interface;
  int newmem = 0;
  if (PySequence_Check($input)) {
    for (Py_ssize_t i = 0; i < PySequence_Size($input); ++i) {
      PyObject * item = PySequence_GetItem($input, i);
      if ((SWIG_ConvertPtrAndOwn(item, (void **)&temp_object, $descriptor(std::shared_ptr<ModelAPI_Object> *), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
        if (!temp_object) {
          PyErr_SetString(PyExc_TypeError, "argument must be list of ModelHighAPI_Interface or ModelAPI_Object.");
          return NULL;
        }
        temp.push_back(*temp_object);
        if (newmem & SWIG_CAST_NEW_MEMORY) {
          delete temp_object;
        }
      } else
      if ((SWIG_ConvertPtrAndOwn(item, (void **)&temp_interface, $descriptor(std::shared_ptr<ModelHighAPI_Interface> *), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
        if (!temp_interface) {
          PyErr_SetString(PyExc_TypeError, "argument must be list of ModelHighAPI_Interface or ModelAPI_Object.");
          return NULL;
        }
        temp.push_back((*temp_interface)->defaultResult());
        if (newmem & SWIG_CAST_NEW_MEMORY) {
          delete temp_interface;
        }
      }
      Py_DECREF(item);
    }
    $1 = &temp;
  } else {
    PyErr_SetString(PyExc_ValueError, "argument must be list of ModelHighAPI_Interface or ModelAPI_Object.");
    return NULL;
  }
}

%typecheck(SWIG_TYPECHECK_POINTER) std::list<std::shared_ptr<ModelAPI_Object> >, const std::list<std::shared_ptr<ModelAPI_Object> >& {
  std::shared_ptr<ModelAPI_Object> * temp_object;
  std::shared_ptr<ModelHighAPI_Interface> * temp_interface;
  int newmem = 0;
  if (PySequence_Check($input)) {
    for (Py_ssize_t i = 0; i < PySequence_Size($input); ++i) {
      PyObject * item = PySequence_GetItem($input, i);
      if ((SWIG_ConvertPtrAndOwn(item, (void **)&temp_object, $descriptor(std::shared_ptr<ModelAPI_Object> *), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
        if (temp_object) {
          $1 = 1;
        } else {
          $1 = 0;
          break;
        }
      } else
      if ((SWIG_ConvertPtrAndOwn(item, (void **)&temp_interface, $descriptor(std::shared_ptr<ModelHighAPI_Interface> *), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
        if (temp_interface) {
          $1 = 1;
        } else {
          $1 = 0;
          break;
        }
      }
      Py_DECREF(item);
    }
  } else {
    $1 = 0;
  }
}

// all supported interfaces
%include "ModelHighAPI_Double.h"
%include "ModelHighAPI_Dumper.h"
%include "ModelHighAPI_Integer.h"
%include "ModelHighAPI_Interface.h"
%include "ModelHighAPI_Folder.h"
%include "ModelHighAPI_RefAttr.h"
%include "ModelHighAPI_Reference.h"
%include "ModelHighAPI_Selection.h"
%include "ModelHighAPI_Services.h"
%include "ModelHighAPI_Macro.h"
%include "ModelHighAPI_Tools.h"
