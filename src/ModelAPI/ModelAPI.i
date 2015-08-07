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
  #include "ModelAPI.h"
  #include "ModelAPI_Entity.h"
  #include "ModelAPI_Document.h"
  #include "ModelAPI_Session.h"
  #include "ModelAPI_Object.h"
  #include "ModelAPI_Feature.h"
  #include "ModelAPI_Plugin.h"
  #include "ModelAPI_CompositeFeature.h"
  #include "ModelAPI_Data.h"
  #include "ModelAPI_Attribute.h"
  #include "ModelAPI_AttributeDocRef.h"
  #include "ModelAPI_AttributeDouble.h"
  #include "ModelAPI_AttributeInteger.h"
  #include "ModelAPI_AttributeString.h"
  #include "ModelAPI_AttributeReference.h"
  #include "ModelAPI_AttributeRefAttr.h"
  #include "ModelAPI_AttributeSelection.h"
  #include "ModelAPI_AttributeSelectionList.h"
  #include "ModelAPI_AttributeValidator.h"
  #include "ModelAPI_Validator.h"
  #include "ModelAPI_FeatureValidator.h"
  #include "ModelAPI_AttributeRefList.h"
  #include "ModelAPI_AttributeBoolean.h"
  #include "ModelAPI_Result.h"
  #include "ModelAPI_ResultConstruction.h"
  #include "ModelAPI_ResultBody.h"
  #include "ModelAPI_ResultPart.h"
  #include "ModelAPI_ResultParameter.h"
  #include "ModelAPI_ResultGroup.h"
  #include "ModelAPI_Tools.h"
  
  #include <memory>
  #include <string>
  
  template<class T1, class T2> 
  std::shared_ptr<T1> shared_ptr_cast(std::shared_ptr<T2> theObject)
  { 
    return std::dynamic_pointer_cast<T1>(theObject); 
  }

%}

// to avoid error on this
#define MODELAPI_EXPORT

// standard definitions
%include "GeomAPI.i"
%include "typemaps.i"
%include "std_string.i"
%include "std_list.i"
%include "std_shared_ptr.i"

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
%shared_ptr(ModelAPI_AttributeInteger)
%shared_ptr(ModelAPI_AttributeString)
%shared_ptr(ModelAPI_AttributeReference)
%shared_ptr(ModelAPI_AttributeRefAttr)
%shared_ptr(ModelAPI_AttributeRefList)
%shared_ptr(ModelAPI_AttributeBoolean)
%shared_ptr(ModelAPI_AttributeSelection)
%shared_ptr(ModelAPI_AttributeSelectionList)
%shared_ptr(ModelAPI_Validator)
%shared_ptr(ModelAPI_AttributeValidator)
%shared_ptr(ModelAPI_FeatureValidator)
%shared_ptr(ModelAPI_Result)
%shared_ptr(ModelAPI_ResultConstruction)
%shared_ptr(ModelAPI_ResultBody)
%shared_ptr(ModelAPI_ResultPart)
%shared_ptr(ModelAPI_ResultGroup)
%shared_ptr(ModelAPI_ResultParameter)

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
%include "ModelAPI_AttributeInteger.h"
%include "ModelAPI_AttributeString.h"
%include "ModelAPI_AttributeReference.h"
%include "ModelAPI_AttributeRefAttr.h"
%include "ModelAPI_AttributeBoolean.h"
%include "ModelAPI_AttributeSelection.h"
%include "ModelAPI_AttributeSelectionList.h"
%include "ModelAPI_AttributeRefList.h"
%include "ModelAPI_Validator.h"
%include "ModelAPI_AttributeValidator.h"
%include "ModelAPI_FeatureValidator.h"
%include "ModelAPI_Result.h"
%include "ModelAPI_ResultConstruction.h"
%include "ModelAPI_ResultBody.h"
%include "ModelAPI_ResultPart.h"
%include "ModelAPI_ResultGroup.h"
%include "ModelAPI_ResultParameter.h"
%include "ModelAPI_Tools.h"

// std::list -> [] 
%template(ObjectList) std::list<std::shared_ptr<ModelAPI_Object> >;
%template(ResultList) std::list<std::shared_ptr<ModelAPI_Result> >;
%template(DocumentList) std::list<std::shared_ptr<ModelAPI_Document> >;

// std::dynamic_pointer_cast
template<class T1, class T2> std::shared_ptr<T1> shared_ptr_cast(std::shared_ptr<T2> theObject);
%template(featureToCompositeFeature) shared_ptr_cast<ModelAPI_CompositeFeature, ModelAPI_Feature>;
%template(objectToFeature) shared_ptr_cast<ModelAPI_Feature, ModelAPI_Object>;
%template(compositeFeatureToFeature) shared_ptr_cast<ModelAPI_Feature, ModelAPI_CompositeFeature>;

%template(modelAPI_Result) shared_ptr_cast<ModelAPI_Result, ModelAPI_Object>;
%template(modelAPI_ResultConstruction) shared_ptr_cast<ModelAPI_ResultConstruction, ModelAPI_Result>;
%template(modelAPI_ResultBody) shared_ptr_cast<ModelAPI_ResultBody, ModelAPI_Result>;
%template(modelAPI_ResultPart) shared_ptr_cast<ModelAPI_ResultPart, ModelAPI_Result>;
%template(modelAPI_ResultParameter) shared_ptr_cast<ModelAPI_ResultParameter, ModelAPI_Result>;
%template(modelAPI_ResultGroup) shared_ptr_cast<ModelAPI_ResultPart, ModelAPI_ResultGroup>;

// Attribute casts
%template(modelAPI_AttributeDocRef)        shared_ptr_cast<ModelAPI_AttributeDocRef, ModelAPI_Attribute>;
%template(modelAPI_AttributeDouble)        shared_ptr_cast<ModelAPI_AttributeDouble, ModelAPI_Attribute>;
%template(modelAPI_AttributeInteger)       shared_ptr_cast<ModelAPI_AttributeInteger, ModelAPI_Attribute>;
%template(modelAPI_AttributeString)        shared_ptr_cast<ModelAPI_AttributeString, ModelAPI_Attribute>;
%template(modelAPI_AttributeReference)     shared_ptr_cast<ModelAPI_AttributeReference, ModelAPI_Attribute>;
%template(modelAPI_AttributeRefAttr)       shared_ptr_cast<ModelAPI_AttributeRefAttr, ModelAPI_Attribute>;
%template(modelAPI_AttributeBoolean)       shared_ptr_cast<ModelAPI_AttributeBoolean, ModelAPI_Attribute>;
%template(modelAPI_AttributeSelection)     shared_ptr_cast<ModelAPI_AttributeSelection, ModelAPI_Attribute>;
%template(modelAPI_AttributeSelectionList) shared_ptr_cast<ModelAPI_AttributeSelectionList, ModelAPI_Attribute>;
%template(modelAPI_AttributeRefList)       shared_ptr_cast<ModelAPI_AttributeRefList, ModelAPI_Attribute>;
