/* ModelAPI.i */
%module ModelAPI
%{
  #include "ModelAPI.h"
  #include "ModelAPI_Document.h"
  #include "ModelAPI_PluginManager.h"
  #include "ModelAPI_Feature.h"
  #include "ModelAPI_Data.h"
  #include "ModelAPI_Attribute.h"
  #include "ModelAPI_AttributeDocRef.h"
  #include "ModelAPI_AttributeDouble.h"
  #include "ModelAPI_Iterator.h"
%}

// to avoid error on this
#define MODELAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_string.i"
//%include <std_shared_ptr.i>

// boost pointers
%include <boost_shared_ptr.i>
%shared_ptr(ModelAPI_Document)
%shared_ptr(ModelAPI_PluginManager)
%shared_ptr(ModelAPI_Feature)
%shared_ptr(ModelAPI_Data)
%shared_ptr(ModelAPI_Attribute)
%shared_ptr(ModelAPI_AttributeDocRef)
%shared_ptr(ModelAPI_AttributeDouble)
%shared_ptr(ModelAPI_Iterator)

// all supported interfaces
%include "ModelAPI_Document.h"
%include "ModelAPI_PluginManager.h"
%include "ModelAPI_Feature.h"
%include "ModelAPI_Data.h"
%include "ModelAPI_Attribute.h"
%include "ModelAPI_AttributeDocRef.h"
%include "ModelAPI_AttributeDouble.h"
%include "ModelAPI_Iterator.h"
