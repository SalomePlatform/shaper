// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

/* FeaturesAPI.i */

%module FeaturesAPI

%{
  #include "FeaturesAPI_swig.h"
%}

%include "doxyhelp.i"

// import other modules
%import "ModelHighAPI.i"

// to avoid error on this
#define FEATURESAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_shared_ptr.i"

// functions with named parameters
%feature("kwargs") addCommon;
%feature("kwargs") addCut;
%feature("kwargs") addFuse;
%feature("kwargs") addPartition;
%feature("kwargs") addPlacement;
%feature("kwargs") addSplit;
%feature("kwargs") addSmash;
%feature("kwargs") addUnion;

// shared pointers
%shared_ptr(FeaturesAPI_BooleanCut)
%shared_ptr(FeaturesAPI_BooleanFuse)
%shared_ptr(FeaturesAPI_BooleanCommon)
%shared_ptr(FeaturesAPI_BooleanSmash)
%shared_ptr(FeaturesAPI_BooleanFill)
%shared_ptr(FeaturesAPI_Chamfer)
%shared_ptr(FeaturesAPI_Extrusion)
%shared_ptr(FeaturesAPI_ExtrusionBoolean)
%shared_ptr(FeaturesAPI_ExtrusionCut)
%shared_ptr(FeaturesAPI_ExtrusionFuse)
%shared_ptr(FeaturesAPI_Fillet)
%shared_ptr(FeaturesAPI_Intersection)
%shared_ptr(FeaturesAPI_MultiRotation)
%shared_ptr(FeaturesAPI_MultiTranslation)
%shared_ptr(FeaturesAPI_Partition)
%shared_ptr(FeaturesAPI_Pipe)
%shared_ptr(FeaturesAPI_Placement)
%shared_ptr(FeaturesAPI_Recover)
%shared_ptr(FeaturesAPI_RemoveSubShapes)
%shared_ptr(FeaturesAPI_Revolution)
%shared_ptr(FeaturesAPI_RevolutionBoolean)
%shared_ptr(FeaturesAPI_RevolutionCut)
%shared_ptr(FeaturesAPI_RevolutionFuse)
%shared_ptr(FeaturesAPI_Rotation)
%shared_ptr(FeaturesAPI_Scale)
%shared_ptr(FeaturesAPI_Symmetry)
%shared_ptr(FeaturesAPI_Translation)
%shared_ptr(FeaturesAPI_Union)
%shared_ptr(FeaturesAPI_FusionFaces)
%shared_ptr(FeaturesAPI_RemoveResults)
%shared_ptr(FeaturesAPI_Copy)
%shared_ptr(FeaturesAPI_ImportResult)
%shared_ptr(FeaturesAPI_Defeaturing)


%typecheck(SWIG_TYPECHECK_POINTER) std::pair<std::list<ModelHighAPI_Selection>, bool>, const std::pair<std::list<ModelHighAPI_Selection>, bool> & {
  ModelHighAPI_Selection* temp_selection;
  bool* temp_bool;
  int newmem = 0;
  $1 = 1;
  std::list<PyObject*> temp_inputlist;
  if (PySequence_Check($input)) {
    for (Py_ssize_t i = 0; i < PySequence_Size($input) && $1; ++i) {
      PyObject * temp = PySequence_GetItem($input, i);

      if ((SWIG_ConvertPtrAndOwn(temp, (void **)&temp_selection, $descriptor(ModelHighAPI_Selection*), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
        if (temp_selection) {
          $1 = 1;
        } else {
          $1 = 0;
        }
      } else {
        $1 = 0;
      }
    }
  } else if (PyBool_Check($input)) {
    $1 = 1;
  } else {
    $1 = 0;
  }
}

%typemap(in) const std::pair<std::list<ModelHighAPI_Selection>, bool> & (std::pair<std::list<ModelHighAPI_Selection>, bool> temp) {
  ModelHighAPI_Selection* temp_selection;
  std::list<ModelHighAPI_Selection> temp_selectionlist;
  int newmem = 0;
  std::list<PyObject*> temp_inputlist;
  if (PySequence_Check($input)) {
    for (Py_ssize_t i = 0; i < PySequence_Size($input); ++i) {
      PyObject * temp = PySequence_GetItem($input, i);

      if ((SWIG_ConvertPtrAndOwn(temp, (void **)&temp_selection, $descriptor(ModelHighAPI_Selection*), SWIG_POINTER_EXCEPTION, &newmem)) == 0) {
        if (temp_selection) {
          temp_selectionlist.push_back(*temp_selection);
          if (newmem & SWIG_CAST_NEW_MEMORY) {
            delete temp_selection;
          }
        }
      } else {
        PyErr_SetString(PyExc_TypeError, "argument must be ModelHighAPI_Selection.");
        return NULL;
      }
    }
    temp = std::pair<std::list<ModelHighAPI_Selection>, bool>(temp_selectionlist, false);
  } else if (PyBool_Check($input)) {
    temp = std::pair<std::list<ModelHighAPI_Selection>, bool>(std::list<ModelHighAPI_Selection>(), $input == Py_True);
  } else {
    PyErr_SetString(PyExc_TypeError, "argument must be std::list<ModelHighAPI_Selection> or bool.");
    return NULL;
  }
  $1 = &temp;
}

// fix compilation error: 'res*' was not declared in this scope
%typemap(freearg) const std::pair<std::list<ModelHighAPI_Selection>, bool> & {}

// all supported interfaces
%include "FeaturesAPI_BooleanCut.h"
%include "FeaturesAPI_BooleanFuse.h"
%include "FeaturesAPI_BooleanCommon.h"
%include "FeaturesAPI_BooleanSmash.h"
%include "FeaturesAPI_BooleanFill.h"
%include "FeaturesAPI_Chamfer.h"
%include "FeaturesAPI_Defeaturing.h"
%include "FeaturesAPI_Extrusion.h"
%include "FeaturesAPI_ExtrusionBoolean.h"
%include "FeaturesAPI_Fillet.h"
%include "FeaturesAPI_Intersection.h"
%include "FeaturesAPI_Measurement.h"
%include "FeaturesAPI_MultiRotation.h"
%include "FeaturesAPI_MultiTranslation.h"
%include "FeaturesAPI_Partition.h"
%include "FeaturesAPI_Pipe.h"
%include "FeaturesAPI_Placement.h"
%include "FeaturesAPI_Recover.h"
%include "FeaturesAPI_RemoveSubShapes.h"
%include "FeaturesAPI_Revolution.h"
%include "FeaturesAPI_RevolutionBoolean.h"
%include "FeaturesAPI_Rotation.h"
%include "FeaturesAPI_Scale.h"
%include "FeaturesAPI_Symmetry.h"
%include "FeaturesAPI_Translation.h"
%include "FeaturesAPI_Union.h"
%include "FeaturesAPI_FusionFaces.h"
%include "FeaturesAPI_RemoveResults.h"
%include "FeaturesAPI_Copy.h"
%include "FeaturesAPI_ImportResult.h"
