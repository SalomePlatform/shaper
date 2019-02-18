// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

/* FeaturesAPI.i */

%module FeaturesAPI

%{
  #include "FeaturesAPI_swig.h"
%}

%include "doxyhelp.i"

// import other modules
%import "ModelHighAPI.i"

// to avoid error on this
#define FEATURESAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_shared_ptr.i"

// shared pointers
%shared_ptr(FeaturesAPI_BooleanCut)
%shared_ptr(FeaturesAPI_BooleanFuse)
%shared_ptr(FeaturesAPI_BooleanCommon)
%shared_ptr(FeaturesAPI_BooleanSmash)
%shared_ptr(FeaturesAPI_BooleanFill)
%shared_ptr(FeaturesAPI_Extrusion)
%shared_ptr(FeaturesAPI_ExtrusionBoolean)
%shared_ptr(FeaturesAPI_ExtrusionCut)
%shared_ptr(FeaturesAPI_ExtrusionFuse)
%shared_ptr(FeaturesAPI_Fillet)
%shared_ptr(FeaturesAPI_Intersection)
%shared_ptr(FeaturesAPI_MultiRotation)
%shared_ptr(FeaturesAPI_MultiTranslation)
%shared_ptr(FeaturesAPI_Partition)
%shared_ptr(FeaturesAPI_Pipe)
%shared_ptr(FeaturesAPI_Placement)
%shared_ptr(FeaturesAPI_Recover)
%shared_ptr(FeaturesAPI_RemoveSubShapes)
%shared_ptr(FeaturesAPI_Revolution)
%shared_ptr(FeaturesAPI_RevolutionBoolean)
%shared_ptr(FeaturesAPI_RevolutionCut)
%shared_ptr(FeaturesAPI_RevolutionFuse)
%shared_ptr(FeaturesAPI_Rotation)
%shared_ptr(FeaturesAPI_Scale)
%shared_ptr(FeaturesAPI_Symmetry)
%shared_ptr(FeaturesAPI_Translation)
%shared_ptr(FeaturesAPI_Union)
%shared_ptr(FeaturesAPI_FusionFaces)

// all supported interfaces
%include "FeaturesAPI_BooleanCut.h"
%include "FeaturesAPI_BooleanFuse.h"
%include "FeaturesAPI_BooleanCommon.h"
%include "FeaturesAPI_BooleanSmash.h"
%include "FeaturesAPI_BooleanFill.h"
%include "FeaturesAPI_Extrusion.h"
%include "FeaturesAPI_ExtrusionBoolean.h"
%include "FeaturesAPI_Fillet.h"
%include "FeaturesAPI_Intersection.h"
%include "FeaturesAPI_Measurement.h"
%include "FeaturesAPI_MultiRotation.h"
%include "FeaturesAPI_MultiTranslation.h"
%include "FeaturesAPI_Partition.h"
%include "FeaturesAPI_Pipe.h"
%include "FeaturesAPI_Placement.h"
%include "FeaturesAPI_Recover.h"
%include "FeaturesAPI_RemoveSubShapes.h"
%include "FeaturesAPI_Revolution.h"
%include "FeaturesAPI_RevolutionBoolean.h"
%include "FeaturesAPI_Rotation.h"
%include "FeaturesAPI_Scale.h"
%include "FeaturesAPI_Symmetry.h"
%include "FeaturesAPI_Translation.h"
%include "FeaturesAPI_Union.h"
%include "FeaturesAPI_FusionFaces.h"
