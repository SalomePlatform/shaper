/* Events.i */
%module EventsAPI
%{
  #include "Events.h"
  #include "Events_Error.h"
  #include "Events_Message.h"
%}


// to avoid error on this
#define EVENTS_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_string.i"

// all supported interfaces
%include "Events_Message.h"
%include "Events_Error.h"
