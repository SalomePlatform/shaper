/* ConnectorAPI.i */

%module ConnectorAPI

%{
  #include "ConnectorAPI_swig.h"
%}

%include "doxyhelp.i"

// import other modules
%import "ModelHighAPI.i"

// to avoid error on this
#define CONNECTORAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_shared_ptr.i"

// all supported interfaces
%include "ConnectorAPI_Connector.h"
