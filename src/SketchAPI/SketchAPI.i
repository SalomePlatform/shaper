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

/* SketchAPI.i */

%module SketchAPI

%{
  #include "SketchAPI_swig.h"
  #include "ModelHighAPI_swig.h"

  // fix for SWIG v2.0.4
  #define SWIGPY_SLICE_ARG(obj) ((PySliceObject*)(obj))
%}

%include "doxyhelp.i"

// import other modules
%import "ModelAPI.i"
%import "ModelHighAPI.i"

// to avoid error on this
#define SKETCHAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_list.i"
%include "std_shared_ptr.i"

// shared pointers
%shared_ptr(SketchAPI_Arc)
%shared_ptr(SketchAPI_MacroArc)
%shared_ptr(SketchAPI_Circle)
%shared_ptr(SketchAPI_MacroCircle)
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

// std::list -> []
%template(InterfaceList) std::list<std::shared_ptr<ModelHighAPI_Interface> >;
%template(EntityList)    std::list<std::shared_ptr<SketchAPI_SketchEntity> >;

%typecheck(SWIG_TYPECHECK_POINTER) std::shared_ptr<ModelAPI_Feature>, const std::shared_ptr<ModelAPI_Feature> & {
  std::shared_ptr<ModelAPI_Feature> * temp_feature;
  std::shared_ptr<ModelHighAPI_Interface> * temp_interface;
  int newmem = 0;
  if ((SWIG_ConvertPtrAndOwn($input, (void **)&temp_feature, $descriptor(std::shared_ptr<ModelAPI_Feature> *), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
    if (temp_feature) {
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
  } else
    $1 = 0;
}

%typemap(in) const std::shared_ptr<ModelAPI_Feature> & (std::shared_ptr<ModelAPI_Feature> temp) {
  std::shared_ptr<ModelAPI_Feature> * temp_feature;
  std::shared_ptr<ModelHighAPI_Interface> * temp_interface;
  int newmem = 0;
  if ((SWIG_ConvertPtrAndOwn($input, (void **)&temp_feature, $descriptor(std::shared_ptr<ModelAPI_Feature> *), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
    if (!temp_feature) {
      PyErr_SetString(PyExc_TypeError, "argument must be ModelHighAPI_Interface.");
      return NULL;
    }
    temp = (*temp_feature);
    if (newmem & SWIG_CAST_NEW_MEMORY) {
      delete temp_feature;
    }
    $1 = &temp;
  } else
  if ((SWIG_ConvertPtrAndOwn($input, (void **)&temp_interface, $descriptor(std::shared_ptr<ModelHighAPI_Interface> *), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
    if (!temp_interface) {
      PyErr_SetString(PyExc_TypeError, "argument must be ModelHighAPI_Interface.");
      return NULL;
    }
    temp = (*temp_interface)->feature();
    if (newmem & SWIG_CAST_NEW_MEMORY) {
      delete temp_interface;
    }
    $1 = &temp;
  } else
  if ((SWIG_ConvertPtr($input, (void **)&$1, $1_descriptor, SWIG_POINTER_EXCEPTION)) == 0) {
  } else {
    PyErr_SetString(PyExc_ValueError, "argument must be ModelHighAPI_Interface.");
    return NULL;
  }
}

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
    temp = ModelHighAPI_RefAttr(std::shared_ptr<ModelAPI_Object>(temp_selection->resultSubShapePair().first));
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

%typemap(in) const std::list<std::shared_ptr<ModelAPI_Object> > & (std::list<std::shared_ptr<ModelAPI_Object> > temp) {
  std::shared_ptr<ModelAPI_Object> * temp_object;
  std::shared_ptr<ModelHighAPI_Interface> * temp_interface;
  ModelHighAPI_Selection* temp_selection;
  int newmem = 0;
  if (PySequence_Check($input)) {
    for (Py_ssize_t i = 0; i < PySequence_Size($input); ++i) {
      PyObject * item = PySequence_GetItem($input, i);
      if ((SWIG_ConvertPtrAndOwn(item, (void **)&temp_selection, $descriptor(ModelHighAPI_Selection*), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
        if (!temp_selection) {
          PyErr_SetString(PyExc_TypeError, "argument must be ModelHighAPI_Interface, ModelHighAPI_Selection or ModelAPI_Object.");
          return NULL;
        }
        temp.push_back(temp_selection->resultSubShapePair().first);
        if (newmem & SWIG_CAST_NEW_MEMORY) {
          delete temp_selection;
        }
      } else
      if ((SWIG_ConvertPtrAndOwn(item, (void **)&temp_object, $descriptor(std::shared_ptr<ModelAPI_Object> *), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
        if (!temp_object) {
          PyErr_SetString(PyExc_TypeError, "argument must be list of ModelHighAPI_Interface, ModelHighAPI_Selection or ModelAPI_Object.");
          return NULL;
        }
        temp.push_back(*temp_object);
        if (newmem & SWIG_CAST_NEW_MEMORY) {
          delete temp_object;
        }
      } else
      if ((SWIG_ConvertPtrAndOwn(item, (void **)&temp_interface, $descriptor(std::shared_ptr<ModelHighAPI_Interface> *), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
        if (!temp_interface) {
          PyErr_SetString(PyExc_TypeError, "argument must be list of ModelHighAPI_Interface, ModelHighAPI_Selection or ModelAPI_Object.");
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
%include "SketchAPI_MacroCircle.h"
%include "SketchAPI_Arc.h"
%include "SketchAPI_MacroArc.h"
%include "SketchAPI_Projection.h"
%include "SketchAPI_Mirror.h"
%include "SketchAPI_Translation.h"
%include "SketchAPI_Rectangle.h"
%include "SketchAPI_Rotation.h"
%include "SketchAPI_Sketch.h"
%include "SketchAPI_Constraint.h"
