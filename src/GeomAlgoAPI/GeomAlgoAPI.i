// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

/* GeomAPI.i */
%module GeomAlgoAPI
%{
  #include "GeomAlgoAPI_swig.h"
%}

// import other modules
%import "GeomAPI.i"

// to avoid error on this
#define GEOMALGOAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_string.i"
%include "std_list.i"
%include "std_shared_ptr.i"
%include "std_vector.i"

%exceptionclass GeomAlgoAPI_Exception;

%exception {
  try {
    $action
  }
  catch (GeomAlgoAPI_Exception exc) {
    // Reraise with SWIG_Python_Raise
    SWIG_Python_Raise(SWIG_NewPointerObj((new GeomAlgoAPI_Exception(static_cast< const GeomAlgoAPI_Exception& >(exc))), SWIGTYPE_p_GeomAlgoAPI_Exception, SWIG_POINTER_OWN), "GeomAlgoAPI_Exception", SWIGTYPE_p_GeomAlgoAPI_Exception);
    SWIG_fail;
  }
}

%typemap(out) std::list< std::shared_ptr< GeomAPI_Shape > >::value_type & {
  $result = SWIG_NewPointerObj(SWIG_as_voidptr(new std::shared_ptr<GeomAPI_Shape>(*$1)), $descriptor(std::shared_ptr<GeomAPI_Shape> *), SWIG_POINTER_OWN | 0 );
}

%template(ShapeList) std::list<std::shared_ptr<GeomAPI_Shape> >;
%template(ValuesList) std::list<GeomAlgoAPI_InfoValue>;
%template(VectorOfDouble) std::vector<double>;

%typemap(in) std::vector<double>& (std::vector<double> temp) {
  if (PyList_Check($input)) {
    for (Py_ssize_t i = 0; i < PyList_Size($input); ++i) {
      PyObject * item = PySequence_GetItem($input, i);
      if (PyNumber_Check(item)) {
        temp.push_back(PyFloat_AsDouble(item));
      } else {
        PyErr_SetString(PyExc_TypeError, "argument must be double value.");
        return NULL;
      }
      Py_DECREF(item);
    }
    $1 = &temp;
  } else {
    PyErr_SetString(PyExc_ValueError, "argument must be a list of double values.");
    return NULL;
  }
}

%typemap(argout) std::vector<double>& {
    PyObject* outList = PyList_New(0);
    int error;
    std::vector<double>::iterator it;
    for ( it=$1->begin() ; it != $1->end(); it++ )
    {
        error = PyList_Append(outList, PyFloat_FromDouble(*it));
        if (error)
            SWIG_fail;
    }
    if ((!$result) || ($result == Py_None)) {
        $result = outList;
    } else {
        PyObject *aObj1, *aObj2;
        if (!PyTuple_Check($result)) {
            PyObject* aObj = $result;
            $result = PyTuple_New(1);
            PyTuple_SetItem($result,0,aObj);
        }
        aObj2 = PyTuple_New(1);
        PyTuple_SetItem(aObj2,0,outList);
        aObj1 = $result;
        $result = PySequence_Concat(aObj1,aObj2);
        Py_DECREF(aObj1);
        Py_DECREF(aObj2);
    }
}

%typecheck(SWIG_TYPECHECK_POINTER) std::vector<double>, const std::vector<double>& {
  if (PyTuple_Check($input)) {
    for (Py_ssize_t i = 0; i < PyTuple_Size($input); ++i) {
      PyObject * item = PySequence_GetItem($input, i);
      if (PyNumber_Check(item)) {
        $1 = 1;
      } else {
        $1 = 0;
        break;
      }
      Py_DECREF(item);
    }
  } else {
    $1 = 0;
  }
}

%typemap(in) double& (double temp) {
  if (PyFloat_Check($input)) {
    temp = PyFloat_AsDouble($input);
    $1 = &temp;
  } else {
    PyErr_SetString(PyExc_ValueError, "argument must be a double value.");
    return NULL;
  }
}

%typemap(argout) double& {
    if ((!$result) || ($result == Py_None)) {
        $result = PyFloat_FromDouble(*$1);
    } else {
        PyObject *aObj1, *aObj2;
        if (!PyTuple_Check($result)) {
            PyObject* aObj = $result;
            $result = PyTuple_New(1);
            PyTuple_SetItem($result,0,aObj);
        }
        aObj2 = PyTuple_New(1);
        PyTuple_SetItem(aObj2,0,PyFloat_FromDouble(*$1));
        aObj1 = $result;
        $result = PySequence_Concat(aObj1,aObj2);
        Py_DECREF(aObj1);
        Py_DECREF(aObj2);
    }
}

// shared pointers
%shared_ptr(GeomAlgoAPI_Boolean)
%shared_ptr(GeomAlgoAPI_Intersection)
%shared_ptr(GeomAlgoAPI_MakeShape)
%shared_ptr(GeomAlgoAPI_MakeShapeCustom)
%shared_ptr(GeomAlgoAPI_MakeShapeList)
%shared_ptr(GeomAlgoAPI_MakeSweep)
%shared_ptr(GeomAlgoAPI_PaveFiller)
%shared_ptr(GeomAlgoAPI_Pipe)
%shared_ptr(GeomAlgoAPI_Placement)
%shared_ptr(GeomAlgoAPI_Prism)
%shared_ptr(GeomAlgoAPI_Revolution)
%shared_ptr(GeomAlgoAPI_Rotation)
%shared_ptr(GeomAlgoAPI_Sewing)
%shared_ptr(GeomAlgoAPI_SketchBuilder)
%shared_ptr(GeomAlgoAPI_ShapeBuilder)
%shared_ptr(GeomAlgoAPI_Translation)
%shared_ptr(GeomAlgoAPI_Transform)
%shared_ptr(GeomAlgoAPI_Box)
%shared_ptr(GeomAlgoAPI_ConeSegment)
%shared_ptr(GeomAlgoAPI_Copy)
%shared_ptr(GeomAlgoAPI_Symmetry)
%shared_ptr(GeomAlgoAPI_MapShapesAndAncestors)
%shared_ptr(GeomAlgoAPI_WireBuilder)

// all supported interfaces
%include "GeomAlgoAPI_MakeShape.h"
%include "GeomAlgoAPI_Boolean.h"
%include "GeomAlgoAPI_Circ2dBuilder.h"
%include "GeomAlgoAPI_CompoundBuilder.h"
%include "GeomAlgoAPI_CurveBuilder.h"
%include "GeomAlgoAPI_DFLoader.h"
%include "GeomAlgoAPI_EdgeBuilder.h"
%include "GeomAlgoAPI_FaceBuilder.h"
%include "GeomAlgoAPI_MakeShapeCustom.h"
%include "GeomAlgoAPI_MakeShapeList.h"
%include "GeomAlgoAPI_MakeSweep.h"
%include "GeomAlgoAPI_Transform.h"
%include "GeomAlgoAPI_Translation.h"
%include "GeomAlgoAPI_Placement.h"
%include "GeomAlgoAPI_PointBuilder.h"
%include "GeomAlgoAPI_Prism.h"
%include "GeomAlgoAPI_Revolution.h"
%include "GeomAlgoAPI_Rotation.h"
%include "GeomAlgoAPI_ShapeTools.h"
%include "GeomAlgoAPI_SketchBuilder.h"
%include "GeomAlgoAPI_BREPExport.h"
%include "GeomAlgoAPI_IGESExport.h"
%include "GeomAlgoAPI_STEPExport.h"
%include "GeomAlgoAPI_BREPImport.h"
%include "GeomAlgoAPI_IGESImport.h"
%include "GeomAlgoAPI_STEPImport.h"
%include "GeomAlgoAPI_Tools.h"
%include "GeomAlgoAPI_PaveFiller.h"
%include "GeomAlgoAPI_Intersection.h"
%include "GeomAlgoAPI_Pipe.h"
%include "GeomAlgoAPI_WireBuilder.h"
%include "GeomAlgoAPI_Sewing.h"
%include "GeomAlgoAPI_ShapeBuilder.h"
%include "GeomAlgoAPI_Exception.h"
%include "GeomAlgoAPI_ShapeAPI.h"
%include "GeomAlgoAPI_Copy.h"
%include "GeomAlgoAPI_Symmetry.h"
%include "GeomAlgoAPI_Box.h"
%include "GeomAlgoAPI_MapShapesAndAncestors.h"
%include "GeomAlgoAPI_ShapeInfo.h"
%include "GeomAlgoAPI_CanonicalRecognition.h"
