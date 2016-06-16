/* SketchAPI.i */

%module SketchAPI

%{
  #include "SketchAPI_swig.h"
%}

%include "doxyhelp.i"

// import other modules
%import "ModelHighAPI.i"

// to avoid error on this
#define SKETCHAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_list.i"
%include "std_shared_ptr.i"

// shared pointers
%shared_ptr(SketchAPI_Arc)
%shared_ptr(SketchAPI_Circle)
%shared_ptr(SketchAPI_IntersectionPoint)
%shared_ptr(SketchAPI_Line)
%shared_ptr(SketchAPI_Mirror)
%shared_ptr(SketchAPI_Sketch)
%shared_ptr(SketchAPI_SketchEntity)
%shared_ptr(SketchAPI_Point)
%shared_ptr(SketchAPI_Rotation)
%shared_ptr(SketchAPI_Translation)

// TODO(spo): move typemaps into ModelHighAPI package

// fix compilarion error: ‘res*’ was not declared in this scope
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

// all supported interfaces (the order is very important according dependencies: base class first)
%include "SketchAPI_SketchEntity.h"
%include "SketchAPI_Point.h"
%include "SketchAPI_IntersectionPoint.h"
%include "SketchAPI_Line.h"
%include "SketchAPI_Circle.h"
%include "SketchAPI_Arc.h"
%include "SketchAPI_Mirror.h"
%include "SketchAPI_Translation.h"
%include "SketchAPI_Rotation.h"
%include "SketchAPI_Sketch.h"
