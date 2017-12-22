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

/* FeaturesAPI.i */

%module BuildAPI

%{
  #include "BuildAPI_swig.h"
%}

%include "doxyhelp.i"

// import other modules
%import "ModelHighAPI.i"

// to avoid error on this
#define BUILDAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_shared_ptr.i"

// shared pointers
%shared_ptr(BuildAPI_Compound)
%shared_ptr(BuildAPI_CompSolid)
%shared_ptr(BuildAPI_Edge)
%shared_ptr(BuildAPI_Face)
%shared_ptr(BuildAPI_Filling)
%shared_ptr(BuildAPI_Shell)
%shared_ptr(BuildAPI_Solid)
%shared_ptr(BuildAPI_SubShapes)
%shared_ptr(BuildAPI_Vertex)
%shared_ptr(BuildAPI_Wire)

// all supported interfaces
%include "BuildAPI_Compound.h"
%include "BuildAPI_CompSolid.h"
%include "BuildAPI_Edge.h"
%include "BuildAPI_Face.h"
%include "BuildAPI_Filling.h"
%include "BuildAPI_Shell.h"
%include "BuildAPI_Solid.h"
%include "BuildAPI_SubShapes.h"
%include "BuildAPI_Vertex.h"
%include "BuildAPI_Wire.h"
