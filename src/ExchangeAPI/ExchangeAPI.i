/* ExchangeAPI.i */

%module ExchangeAPI

%{
  #include "ExchangeAPI_swig.h"
%}

%include "doxyhelp.i"

// import other modules
%import "ModelHighAPI.i"

// to avoid error on this
#define EXCHANGEAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_list.i"
%include "std_shared_ptr.i"

// shared pointers
%shared_ptr(ExchangeAPI_Export)
%shared_ptr(ExchangeAPI_Import)

// all supported interfaces
%include "ExchangeAPI_Export.h"
%include "ExchangeAPI_Import.h"
