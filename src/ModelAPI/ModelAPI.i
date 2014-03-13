/* ModelAPI.i */
%module ModelAPI
%{
  #include "ModelAPI.hxx"
  #include "ModelAPI_Application.hxx"
  #include "ModelAPI_Document.hxx"
%}
%include "ModelAPI_Application.hxx"
%include "ModelAPI_Document.hxx"
