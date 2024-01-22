// Copyright (C) 2014-2024  CEA, EDF
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

/* Events.i */
%module(directors="1") EventsAPI
%feature("director:except") {
    if ($error != NULL) {
      PyErr_Print();
      std::cerr << std::endl;
      throw Swig::DirectorMethodException();
    }
}

%{
  #include "Events.h"
  #include "Events_InfoMessage.h"
  #include "Events_Listener.h"
  #include "Events_Loop.h"
  #include "Events_Message.h"
  #include "Events_MessageGroup.h"

  #ifdef _MSC_VER
  # pragma warning(disable: 4456) // declaration of variable hides previous local declaration
  # pragma warning(disable: 4459) // declaration of variable hides global declaration
  #endif
%}


// to avoid error on this
#define EVENTS_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_string.i"
%include "std_wstring.i"

// directors
%feature("director") Events_Listener;

// all supported interfaces
%include "Events_Message.h"
%include "Events_InfoMessage.h"
%include "Events_Listener.h"
%include "Events_Loop.h"
%include "Events_MessageGroup.h"
