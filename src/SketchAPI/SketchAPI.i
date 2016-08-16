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
%shared_ptr(SketchAPI_Constraint)
%shared_ptr(SketchAPI_IntersectionPoint)
%shared_ptr(SketchAPI_Line)
%shared_ptr(SketchAPI_Mirror)
%shared_ptr(SketchAPI_Sketch)
%shared_ptr(SketchAPI_SketchEntity)
%shared_ptr(SketchAPI_Point)
%shared_ptr(SketchAPI_Projection)
%shared_ptr(SketchAPI_Rectangle)
%shared_ptr(SketchAPI_Rotation)
%shared_ptr(SketchAPI_Translation)

%typemap(in) const ModelHighAPI_RefAttr & (ModelHighAPI_RefAttr temp) {
  std::shared_ptr<ModelAPI_Attribute> * temp_attribute;
  std::shared_ptr<ModelAPI_Object> * temp_object;
  std::shared_ptr<ModelHighAPI_Interface> * temp_interface;
  ModelHighAPI_Selection* temp_selection;
  int newmem = 0;
  if ((SWIG_ConvertPtrAndOwn($input, (void **)&temp_selection, $descriptor(ModelHighAPI_Selection*), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
    if (!temp_selection) {
      PyErr_SetString(PyExc_TypeError, "argument must be ModelHighAPI_RefAttr, ModelHighAPI_Selection, ModelHighAPI_Interface, ModelAPI_Attribute or ModelAPI_Object.");
      return NULL;
    }
    temp = ModelHighAPI_RefAttr(temp_selection->resultSubShapePair().first);
    if (newmem & SWIG_CAST_NEW_MEMORY) {
      delete temp_selection;
    }
    $1 = &temp;
  } else
  if ((SWIG_ConvertPtrAndOwn($input, (void **)&temp_attribute, $descriptor(std::shared_ptr<ModelAPI_Attribute> *), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
    if (!temp_attribute) {
      PyErr_SetString(PyExc_TypeError, "argument must be ModelHighAPI_RefAttr, ModelHighAPI_Selection, ModelHighAPI_Interface, ModelAPI_Attribute or ModelAPI_Object.");
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
      PyErr_SetString(PyExc_TypeError, "argument must be ModelHighAPI_RefAttr, ModelHighAPI_Selection, ModelHighAPI_Interface, ModelAPI_Attribute or ModelAPI_Object.");
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
      PyErr_SetString(PyExc_TypeError, "argument must be ModelHighAPI_RefAttr, ModelHighAPI_Selection, ModelHighAPI_Interface, ModelAPI_Attribute or ModelAPI_Object.");
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
    PyErr_SetString(PyExc_TypeError, "argument must be ModelHighAPI_RefAttr, ModelHighAPI_Selection, ModelHighAPI_Interface, ModelAPI_Attribute or ModelAPI_Object.");
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
%include "SketchAPI_Projection.h"
%include "SketchAPI_Mirror.h"
%include "SketchAPI_Translation.h"
%include "SketchAPI_Rectangle.h"
%include "SketchAPI_Rotation.h"
%include "SketchAPI_Sketch.h"
%include "SketchAPI_Constraint.h"
