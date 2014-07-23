/* ModelAPI.i */
%module ModelAPI
%{
  #include "ModelAPI.h"
  #include "ModelAPI_Document.h"
  #include "ModelAPI_PluginManager.h"
  #include "ModelAPI_Object.h"
  #include "ModelAPI_Feature.h"
  #include "ModelAPI_Data.h"
  #include "ModelAPI_Attribute.h"
  #include "ModelAPI_AttributeDocRef.h"
  #include "ModelAPI_AttributeDouble.h"
  #include "ModelAPI_AttributeReference.h"
  #include "ModelAPI_AttributeRefAttr.h"
  #include "ModelAPI_Validator.h"
  #include "ModelAPI_AttributeRefList.h"
  #include "ModelAPI_Result.h"
%}

// to avoid error on this
#define MODELAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_string.i"
%include "std_list.i"

// boost pointers
%include <boost_shared_ptr.i>
%shared_ptr(ModelAPI_Document)
%shared_ptr(ModelAPI_PluginManager)
%shared_ptr(ModelAPI_Object)
%shared_ptr(ModelAPI_Feature)
%shared_ptr(ModelAPI_Data)
%shared_ptr(ModelAPI_Attribute)
%shared_ptr(ModelAPI_AttributeDocRef)
%shared_ptr(ModelAPI_AttributeDouble)
%shared_ptr(ModelAPI_AttributeReference)
%shared_ptr(ModelAPI_AttributeRefAttr)
%shared_ptr(ModelAPI_AttributeRefList)
%shared_ptr(ModelAPI_Result)

// all supported interfaces
%include "ModelAPI_Document.h"
%include "ModelAPI_PluginManager.h"
%include "ModelAPI_Object.h"
%include "ModelAPI_Feature.h"
%include "ModelAPI_Data.h"
%include "ModelAPI_Attribute.h"
%include "ModelAPI_AttributeDocRef.h"
%include "ModelAPI_AttributeDouble.h"
%include "ModelAPI_AttributeReference.h"
%include "ModelAPI_AttributeRefAttr.h"
%include "ModelAPI_Validator.h"
%include "ModelAPI_AttributeRefList.h"
%include "ModelAPI_Result.h"

%template(ObjectList) std::list<boost::shared_ptr<ModelAPI_Object> >;
