/* ModelAPI.i */
%module ModelAPI
%{
  #include "GeomAPI_Interface.h"
  #include "GeomAPI_Shape.h"
  #include "ModelAPI.h"
  #include "ModelAPI_Document.h"
  #include "ModelAPI_Session.h"
  #include "ModelAPI_Object.h"
  #include "ModelAPI_Feature.h"
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
  #include "ModelAPI_ResultParameters.h"
  #include "ModelAPI_ResultGroup.h"
  #include "ModelAPI_Tools.h"
  
  template<class T1, class T2> 
  std::shared_ptr<T1> shared_ptr_cast(std::shared_ptr<T2> theObject)
  { 
    return std::dynamic_pointer_cast<T1>(theObject); 
  }
  
%}

// to avoid error on this
#define MODELAPI_EXPORT
#define GEOMAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_string.i"
%include "std_list.i"
%include "std_shared_ptr.i"

// shared pointers
// For ModelAPI_ResultConstruction.shape()
%shared_ptr(GeomAPI_Interface)
%shared_ptr(GeomAPI_Shape)
%shared_ptr(ModelAPI_Document)
%shared_ptr(ModelAPI_Session)
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
%shared_ptr(ModelAPI_ResultParameters)

// all supported interfaces
%include "GeomAPI_Interface.h"
%include "GeomAPI_Shape.h"
%include "ModelAPI_Document.h"
%include "ModelAPI_Session.h"
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
%include "ModelAPI_ResultParameters.h"
%include "ModelAPI_Tools.h"

%template(ObjectList) std::list<std::shared_ptr<ModelAPI_Object> >;
%template(ResultList) std::list<std::shared_ptr<ModelAPI_Result> >;
%template(DocumentList) std::list<std::shared_ptr<ModelAPI_Document> >;

template<class T1, class T2> std::shared_ptr<T1> shared_ptr_cast(std::shared_ptr<T2> theObject);
%template(modelAPI_CompositeFeature) shared_ptr_cast<ModelAPI_CompositeFeature, ModelAPI_Feature>;
%template(modelAPI_Feature) shared_ptr_cast<ModelAPI_Feature, ModelAPI_Object>;
%template(modelAPI_ResultConstruction) shared_ptr_cast<ModelAPI_ResultConstruction, ModelAPI_Result>;
%template(modelAPI_ResultBody) shared_ptr_cast<ModelAPI_ResultBody, ModelAPI_Result>;
%template(modelAPI_ResultPart) shared_ptr_cast<ModelAPI_ResultPart, ModelAPI_Result>;
%template(modelAPI_ResultGroup) shared_ptr_cast<ModelAPI_ResultPart, ModelAPI_ResultGroup>;
