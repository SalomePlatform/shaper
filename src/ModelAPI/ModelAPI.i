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

/* ModelAPI.i */
%module(directors="1") ModelAPI
%feature("director:except") {
    if ($error != NULL) {
      PyErr_Print();
      std::cerr << std::endl;
      throw Swig::DirectorMethodException();
    }
}

%{
  #include "ModelAPI_swig.h"
%}

// import other modules
%import "GeomAPI.i"

// to avoid error on this
#define MODELAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_string.i"
%include "std_list.i"
%include "std_shared_ptr.i"
%include "std_set.i"

// directors
%feature("director") ModelAPI_Plugin;
%feature("director") ModelAPI_Object;
%feature("director") ModelAPI_Feature;
%feature("director") ModelAPI_CompositeFeature;
%feature("director") ModelAPI_Data;

// shared pointers
// For ModelAPI_ResultConstruction.shape()
%shared_ptr(ModelAPI_Entity)
%shared_ptr(ModelAPI_Document)
%shared_ptr(ModelAPI_Session)
%shared_ptr(ModelAPI_Plugin)
%shared_ptr(ModelAPI_Object)
%shared_ptr(ModelAPI_Feature)
%shared_ptr(ModelAPI_CompositeFeature)
%shared_ptr(ModelAPI_Data)
%shared_ptr(ModelAPI_Attribute)
%shared_ptr(ModelAPI_AttributeDocRef)
%shared_ptr(ModelAPI_AttributeDouble)
%shared_ptr(ModelAPI_AttributeDoubleArray)
%shared_ptr(ModelAPI_AttributeInteger)
%shared_ptr(ModelAPI_AttributeIntArray)
%shared_ptr(ModelAPI_AttributeString)
%shared_ptr(ModelAPI_AttributeStringArray)
%shared_ptr(ModelAPI_AttributeReference)
%shared_ptr(ModelAPI_AttributeRefAttr)
%shared_ptr(ModelAPI_AttributeRefList)
%shared_ptr(ModelAPI_AttributeRefAttrList)
%shared_ptr(ModelAPI_AttributeBoolean)
%shared_ptr(ModelAPI_AttributeSelection)
%shared_ptr(ModelAPI_AttributeSelectionList)
%shared_ptr(ModelAPI_AttributeTables)
%shared_ptr(ModelAPI_Validator)
%shared_ptr(ModelAPI_AttributeValidator)
%shared_ptr(ModelAPI_FeatureValidator)
%shared_ptr(ModelAPI_Result)
%shared_ptr(ModelAPI_ResultConstruction)
%shared_ptr(ModelAPI_ResultBody)
%shared_ptr(ModelAPI_ResultPart)
%shared_ptr(ModelAPI_ResultGroup)
%shared_ptr(ModelAPI_ResultField)
%shared_ptr(ModelAPI_ResultParameter)
%shared_ptr(ModelAPI_ResultCompSolid)

%typecheck(SWIG_TYPECHECK_POINTER) const ModelAPI_AttributeTables::Value {
  $1 = (PyFloat_Check($input) || PyInt_Check($input) || PyLong_Check($input) || PyString_Check($input) || PyBool_Check($input)) ? 1 : 0;
}

// Tables Value reading as int, double, boolean or string
%typemap(in) const ModelAPI_AttributeTables::Value {
  if (PyInt_Check($input)) {
    $1.myInt = int(PyInt_AsLong($input));
    $1.myDouble = double(PyInt_AsLong($input));
    $1.myBool = PyInt_AsLong($input) != 0;
  } else if (PyFloat_Check($input)) {
    $1.myInt = int(PyFloat_AsDouble($input));
    $1.myDouble = PyFloat_AsDouble($input);
  } else if (PyBool_Check($input)) {
    $1.myBool = $input == Py_True;
  } else if (PyString_Check($input)) {
    $1.myStr = PyString_AsString($input);
  } else if ((SWIG_ConvertPtr($input, (void **)&$1, $1_descriptor, SWIG_POINTER_EXCEPTION)) == 0) {
  } else {
    PyErr_SetString(PyExc_ValueError, "Tables value must be int, double, string or bool.");
    return NULL;
  }
}

// all supported interfaces
%include "ModelAPI_Entity.h"
%include "ModelAPI_Document.h"
%include "ModelAPI_Session.h"
%include "ModelAPI_Plugin.h"
%include "ModelAPI_Object.h"
%include "ModelAPI_Feature.h"
%include "ModelAPI_CompositeFeature.h"
%include "ModelAPI_Data.h"
%include "ModelAPI_Attribute.h"
%include "ModelAPI_AttributeDocRef.h"
%include "ModelAPI_AttributeDouble.h"
%include "ModelAPI_AttributeDoubleArray.h"
%include "ModelAPI_AttributeInteger.h"
%include "ModelAPI_AttributeIntArray.h"
%include "ModelAPI_AttributeString.h"
%include "ModelAPI_AttributeStringArray.h"
%include "ModelAPI_AttributeReference.h"
%include "ModelAPI_AttributeRefAttr.h"
%include "ModelAPI_AttributeBoolean.h"
%include "ModelAPI_AttributeSelection.h"
%include "ModelAPI_AttributeSelectionList.h"
%include "ModelAPI_AttributeRefList.h"
%include "ModelAPI_AttributeRefAttrList.h"
%include "ModelAPI_AttributeTables.h"
%include "ModelAPI_Validator.h"
%include "ModelAPI_AttributeValidator.h"
%include "ModelAPI_FeatureValidator.h"
%include "ModelAPI_Result.h"
%include "ModelAPI_ResultConstruction.h"
%include "ModelAPI_ResultBody.h"
%include "ModelAPI_ResultPart.h"
%include "ModelAPI_ResultGroup.h"
%include "ModelAPI_ResultField.h"
%include "ModelAPI_ResultParameter.h"
%include "ModelAPI_Tools.h"
%include "ModelAPI_ResultCompSolid.h"

// std::list -> []
%template(StringList) std::list<std::string>;
%template(ObjectList) std::list<std::shared_ptr<ModelAPI_Object> >;
%template(FeatureList) std::list<std::shared_ptr<ModelAPI_Feature> >;
%template(ResultList) std::list<std::shared_ptr<ModelAPI_Result> >;
%template(DocumentList) std::list<std::shared_ptr<ModelAPI_Document> >;
// std::set -> []
%template(AttributeSet) std::set<std::shared_ptr<ModelAPI_Attribute> >;
%template(FeatureSet) std::set<std::shared_ptr<ModelAPI_Feature> >;

// std::dynamic_pointer_cast
template<class T1, class T2> std::shared_ptr<T1> shared_ptr_cast(std::shared_ptr<T2> theObject);
%template(featureToCompositeFeature) shared_ptr_cast<ModelAPI_CompositeFeature, ModelAPI_Feature>;
%template(objectToFeature) shared_ptr_cast<ModelAPI_Feature, ModelAPI_Object>;
%template(objectToResult) shared_ptr_cast<ModelAPI_Result, ModelAPI_Object>;
%template(compositeFeatureToFeature) shared_ptr_cast<ModelAPI_Feature, ModelAPI_CompositeFeature>;

%template(modelAPI_Result) shared_ptr_cast<ModelAPI_Result, ModelAPI_Object>;
%template(modelAPI_ResultConstruction) shared_ptr_cast<ModelAPI_ResultConstruction, ModelAPI_Result>;
%template(modelAPI_ResultBody) shared_ptr_cast<ModelAPI_ResultBody, ModelAPI_Result>;
%template(modelAPI_ResultPart) shared_ptr_cast<ModelAPI_ResultPart, ModelAPI_Result>;
%template(modelAPI_ResultParameter) shared_ptr_cast<ModelAPI_ResultParameter, ModelAPI_Result>;
%template(modelAPI_ResultGroup) shared_ptr_cast<ModelAPI_ResultPart, ModelAPI_ResultGroup>;
%template(modelAPI_ResultField) shared_ptr_cast<ModelAPI_ResultPart, ModelAPI_ResultField>;
%template(modelAPI_ResultCompSolid) shared_ptr_cast<ModelAPI_ResultCompSolid, ModelAPI_ResultBody>;

// Attribute casts
%template(modelAPI_AttributeDocRef)        shared_ptr_cast<ModelAPI_AttributeDocRef, ModelAPI_Attribute>;
%template(modelAPI_AttributeDouble)        shared_ptr_cast<ModelAPI_AttributeDouble, ModelAPI_Attribute>;
%template(modelAPI_AttributeDoubleArray)   shared_ptr_cast<ModelAPI_AttributeDoubleArray, ModelAPI_Attribute>;
%template(modelAPI_AttributeInteger)       shared_ptr_cast<ModelAPI_AttributeInteger, ModelAPI_Attribute>;
%template(modelAPI_AttributeIntArray)      shared_ptr_cast<ModelAPI_AttributeIntArray, ModelAPI_Attribute>;
%template(modelAPI_AttributeString)        shared_ptr_cast<ModelAPI_AttributeString, ModelAPI_Attribute>;
%template(modelAPI_AttributeStringArray)   shared_ptr_cast<ModelAPI_AttributeStringArray, ModelAPI_Attribute>;
%template(modelAPI_AttributeReference)     shared_ptr_cast<ModelAPI_AttributeReference, ModelAPI_Attribute>;
%template(modelAPI_AttributeRefAttr)       shared_ptr_cast<ModelAPI_AttributeRefAttr, ModelAPI_Attribute>;
%template(modelAPI_AttributeBoolean)       shared_ptr_cast<ModelAPI_AttributeBoolean, ModelAPI_Attribute>;
%template(modelAPI_AttributeSelection)     shared_ptr_cast<ModelAPI_AttributeSelection, ModelAPI_Attribute>;
%template(modelAPI_AttributeSelectionList) shared_ptr_cast<ModelAPI_AttributeSelectionList, ModelAPI_Attribute>;
%template(modelAPI_AttributeRefList)       shared_ptr_cast<ModelAPI_AttributeRefList, ModelAPI_Attribute>;
%template(modelAPI_AttributeRefAttrList)   shared_ptr_cast<ModelAPI_AttributeRefAttrList, ModelAPI_Attribute>;
%template(modelAPI_AttributeTables)        shared_ptr_cast<ModelAPI_AttributeTables, ModelAPI_Attribute>;

%template(PointList) std::list<std::shared_ptr<GeomAPI_Pnt> >;
%template(PointSet) std::set<std::shared_ptr<GeomAPI_Pnt> >;

// Geometry casts
%template(shapeToEdge) shared_ptr_cast<GeomAPI_Edge, GeomAPI_Shape>;
