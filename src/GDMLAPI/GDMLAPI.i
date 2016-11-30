/* GDMLAPI.i */
// File:        GDMLAPI.i
// Created:     29 Nov 2016
// Author:      Clarisse Genrault (CEA)

%module GDMLAPI

%{
  #include "GDMLAPI_swig.h"
%}

%include "doxyhelp.i"

// import other modules
%import "ModelHighAPI.i"

// to avoid error on this
#define GDMLAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_string.i"
%include "std_shared_ptr.i"
%include "std_list.i"

// shared pointers
%shared_ptr(GDMLAPI_ConeSegment)

// all supported interfaces
%include "GDMLAPI_ConeSegment.h"

