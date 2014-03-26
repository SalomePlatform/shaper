/* ModelAPI.i */
%module ModelAPI
%{
  #include "ModelAPI.hxx"
  #include "ModelAPI_Document.hxx"
  #include "ModelAPI_PluginManager.hxx"
  #include "ModelAPI_Feature.hxx"
%}

// to avoid error on this
#define MODELAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_string.i"

// boost pointers
%include <boost_shared_ptr.i>
%shared_ptr(ModelAPI_PluginManager)
%shared_ptr(ModelAPI_Feature)

// all supported interfaces
%include "ModelAPI_Document.hxx"
%include "ModelAPI_PluginManager.hxx"
%include "ModelAPI_Feature.hxx"
