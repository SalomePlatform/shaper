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

#include <GeomAlgoAPI_Translation.h>

#include <GeomAPI_Ax1.h>
#include <GeomAPI_Shape.h>

#include <Precision.hxx>

//=================================================================================================
GeomAlgoAPI_Translation::GeomAlgoAPI_Translation(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                                 std::shared_ptr<GeomAPI_Ax1>   theAxis,
                                                 double                         theDistance)
{
  if (!theAxis) {
    myError = "Translation builder :: axis is not valid.";
    return;
  }

  GeomTrsfPtr aTrsf(new GeomAPI_Trsf);
  aTrsf->setTranslation(theAxis, theDistance);

  build(theSourceShape, aTrsf);
}

//=================================================================================================
GeomAlgoAPI_Translation::GeomAlgoAPI_Translation(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                                 double                         theDx,
                                                 double                         theDy,
                                                 double                         theDz)
{
  GeomTrsfPtr aTrsf(new GeomAPI_Trsf);
  aTrsf->setTranslation(theDx, theDy, theDz);

  build(theSourceShape, aTrsf);
}

//=================================================================================================
GeomAlgoAPI_Translation::GeomAlgoAPI_Translation(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                                 std::shared_ptr<GeomAPI_Pnt>   theStartPoint,
                                                 std::shared_ptr<GeomAPI_Pnt>   theEndPoint)
{
  if (!theStartPoint) {
    myError = "Translation builder :: start point is not valid.";
    return;
  }
  if (!theEndPoint) {
    myError = "Translation builder :: end point is not valid.";
    return;
  }
  if (theStartPoint->distance(theEndPoint) < Precision::Confusion()) {
    myError = "Translation builder :: start point and end point coincide.";
    return;
  }

  GeomTrsfPtr aTrsf(new GeomAPI_Trsf);
  aTrsf->setTranslation(theStartPoint, theEndPoint);

  build(theSourceShape, aTrsf);
}
