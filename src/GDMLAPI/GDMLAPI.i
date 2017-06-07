// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

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

