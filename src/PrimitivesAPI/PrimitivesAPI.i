// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

/* PrimitivesAPI.i */

%module PrimitivesAPI

%{
  #include "PrimitivesAPI_swig.h"
%}

%include "doxyhelp.i"

// import other modules
%import "ModelHighAPI.i"

// to avoid error on this
#define PRIMITIVESAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_shared_ptr.i"

// shared pointers
%shared_ptr(PrimitivesAPI_Box)
%shared_ptr(PrimitivesAPI_Cone)
%shared_ptr(PrimitivesAPI_Cylinder)
%shared_ptr(PrimitivesAPI_Sphere)
%shared_ptr(PrimitivesAPI_Torus)

// all supported interfaces
%include "PrimitivesAPI_Box.h"
%include "PrimitivesAPI_Cone.h"
%include "PrimitivesAPI_Cylinder.h"
%include "PrimitivesAPI_Sphere.h"
%include "PrimitivesAPI_Torus.h"
