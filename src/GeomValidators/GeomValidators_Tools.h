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

#ifndef GeomValidators_Tools_HeaderFile
#define GeomValidators_Tools_HeaderFile

#include "GeomValidators.h"
#include "ModelAPI_Object.h"
#include "ModelAPI_Attribute.h"

#include "TopAbs_ShapeEnum.hxx"
#include "TopoDS_Shape.hxx"

namespace GeomValidators_Tools
{
  // Returns the object from the attribute
  /// \param theObj an object
  GEOMVALIDATORS_EXPORT ObjectPtr getObject(const AttributePtr& theAttribute);

};

#endif
