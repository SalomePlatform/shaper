/* Config.i */

%module ConfigAPI
%{
  #include "Config_swig.h"
%}

// to avoid error on this
#define CONFIG_EXPORT

%include "typemaps.i"
%include "std_string.i"

%include "Config_ModuleReader.h"
