// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include <GeomAlgoAPI_Symmetry.h>

#include <GeomAPI_Pnt.h>
#include <GeomAPI_Ax1.h>
#include <GeomAPI_Ax2.h>

//=================================================================================================
GeomAlgoAPI_Symmetry::GeomAlgoAPI_Symmetry(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                           std::shared_ptr<GeomAPI_Pnt>   thePoint)
{
  if (!thePoint) {
    myError = "Symmetry builder :: point is not valid.";
    return;
  }

  GeomTrsfPtr aTrsf(new GeomAPI_Trsf);
  aTrsf->setSymmetry(thePoint);

  build(theSourceShape, aTrsf);
}

//=================================================================================================
GeomAlgoAPI_Symmetry::GeomAlgoAPI_Symmetry(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                           std::shared_ptr<GeomAPI_Ax1>   theAxis)
{
  if (!theAxis) {
    myError = "Symmetry builder :: axis is not valid.";
    return;
  }

  GeomTrsfPtr aTrsf(new GeomAPI_Trsf);
  aTrsf->setSymmetry(theAxis);

  build(theSourceShape, aTrsf);
}

//=================================================================================================
GeomAlgoAPI_Symmetry::GeomAlgoAPI_Symmetry(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                           std::shared_ptr<GeomAPI_Ax2>   thePlane)
{
  if (!thePlane) {
    myError = "Symmetry builder :: plane is not valid.";
    return;
  }

  GeomTrsfPtr aTrsf(new GeomAPI_Trsf);
  aTrsf->setSymmetry(thePlane);

  build(theSourceShape, aTrsf);
}
