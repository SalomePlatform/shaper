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
  #include "ModelAPI_Data.h"
  #include "ModelAPI_Attribute.h"
  #include "ModelAPI_AttributeDocRef.h"
  #include "ModelAPI_AttributeDouble.h"
  #include "ModelAPI_AttributeInteger.h"
  #include "ModelAPI_AttributeString.h"
  #include "ModelAPI_AttributeReference.h"
  #include "ModelAPI_AttributeRefAttr.h"
  #include "ModelAPI_AttributeSelection.h"
  #include "ModelAPI_Validator.h"
  #include "ModelAPI_AttributeRefList.h"
  #include "ModelAPI_AttributeBoolean.h"
  #include "ModelAPI_Result.h"
  #include "ModelAPI_ResultConstruction.h"
  #include "ModelAPI_ResultBody.h"
  #include "ModelAPI_ResultPart.h"
  
  template<class T> boost::shared_ptr<T> castTo(boost::shared_ptr<ModelAPI_Result> theObject) 
  { 
    return boost::dynamic_pointer_cast<T>(theObject); 
  }
  
   
%}

// to avoid error on this
#define MODELAPI_EXPORT
#define GEOMAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_string.i"
%include "std_list.i"

// boost pointers
%include <boost_shared_ptr.i>
// For ModelAPI_ResultConstruction.shape()
%shared_ptr(GeomAPI_Interface)
%shared_ptr(GeomAPI_Shape)
%shared_ptr(ModelAPI_Document)
%shared_ptr(ModelAPI_Session)
%shared_ptr(ModelAPI_Object)
%shared_ptr(ModelAPI_Feature)
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
%shared_ptr(ModelAPI_Result)
%shared_ptr(ModelAPI_ResultConstruction)
%shared_ptr(ModelAPI_ResultBody)
%shared_ptr(ModelAPI_ResultPart)

// all supported interfaces
%include "GeomAPI_Interface.h"
%include "GeomAPI_Shape.h"
%include "ModelAPI_Document.h"
%include "ModelAPI_Session.h"
%include "ModelAPI_Object.h"
%include "ModelAPI_Feature.h"
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
%include "ModelAPI_AttributeRefList.h"
%include "ModelAPI_Validator.h"
%include "ModelAPI_Result.h"
%include "ModelAPI_ResultConstruction.h"
%include "ModelAPI_ResultBody.h"
%include "ModelAPI_ResultPart.h"

%template(ObjectList) std::list<boost::shared_ptr<ModelAPI_Object> >;
%template(ResultList) std::list<boost::shared_ptr<ModelAPI_Result> >;

template<class T> boost::shared_ptr<T> castTo(boost::shared_ptr<ModelAPI_Result> theObject);
%template(modelAPI_ResultConstruction) castTo<ModelAPI_ResultConstruction>;
%template(modelAPI_ResultBody) castTo<ModelAPI_ResultBody>;
%template(modelAPI_ResultPart) castTo<ModelAPI_ResultPart>;
