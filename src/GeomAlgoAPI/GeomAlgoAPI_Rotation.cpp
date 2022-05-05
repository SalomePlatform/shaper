// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include <GeomAlgoAPI_Rotation.h>

#include <GeomAPI_Ax1.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_XYZ.h>

#include <Precision.hxx>

// Verify points are applicable to build the rotation transformation
static bool checkPoints(GeomPointPtr theCenterPoint,
                        GeomPointPtr theStartPoint,
                        GeomPointPtr theEndPoint,
                        std::string& theError);

//=================================================================================================
GeomAlgoAPI_Rotation::GeomAlgoAPI_Rotation(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                           std::shared_ptr<GeomAPI_Ax1>   theAxis,
                                           double                         theAngle)
{
  if (!theAxis) {
    myError = "Rotation builder :: axis is not valid.";
    return;
  }

  GeomTrsfPtr aTrsf(new GeomAPI_Trsf);
  aTrsf->setRotation(theAxis, theAngle);

  build(theSourceShape, aTrsf);
}


//=================================================================================================
GeomAlgoAPI_Rotation::GeomAlgoAPI_Rotation(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                           std::shared_ptr<GeomAPI_Pnt>   theCenterPoint,
                                           std::shared_ptr<GeomAPI_Pnt>   theStartPoint,
                                           std::shared_ptr<GeomAPI_Pnt>   theEndPoint)
{
  if (!checkPoints(theCenterPoint, theStartPoint, theEndPoint, myError))
    return;

  GeomTrsfPtr aTrsf(new GeomAPI_Trsf);
  aTrsf->setRotation(theCenterPoint, theStartPoint, theEndPoint);

  build(theSourceShape, aTrsf);
}

//=================================================================================================
bool checkPoints(GeomPointPtr theCenterPoint,
                 GeomPointPtr theStartPoint,
                 GeomPointPtr theEndPoint,
                 std::string& theError)
{
  if (!theCenterPoint) {
    theError = "Rotation builder :: center point is not valid.";
    return false;
  }
  if (!theStartPoint) {
    theError = "Rotation builder :: start point is not valid.";
    return false;
  }
  if (!theEndPoint) {
    theError = "Rotation builder :: end point is not valid.";
    return false;
  }
  if (theCenterPoint->distance(theStartPoint) < Precision::Confusion()) {
    theError = "Rotation builder :: center point and start point coincide.";
    return false;
  }
  if (theCenterPoint->distance(theEndPoint) < Precision::Confusion()) {
    theError = "Rotation builder :: center point and end point coincide.";
    return false;
  }
  if (theStartPoint->distance(theEndPoint) < Precision::Confusion()) {
    theError = "Rotation builder :: start point and end point coincide.";
    return false;
  }
  std::shared_ptr<GeomAPI_XYZ> aCenterPointXYZ = theCenterPoint->xyz();
  std::shared_ptr<GeomAPI_XYZ> aStartPointXYZ = theStartPoint->xyz();
  std::shared_ptr<GeomAPI_XYZ> aEndPointXYZ = theEndPoint->xyz();
  std::shared_ptr<GeomAPI_XYZ> vectCenterPointStartPoint =
      aStartPointXYZ->decreased(aCenterPointXYZ);
  std::shared_ptr<GeomAPI_XYZ> vectCenterPointEndPoint =
      aEndPointXYZ->decreased(aCenterPointXYZ);
  std::shared_ptr<GeomAPI_XYZ> crossProduct =
      vectCenterPointStartPoint->cross(vectCenterPointEndPoint);

  if (crossProduct->squareModulus() < Precision::SquareConfusion()) {
    theError = "Rotation builder :: center point, start point and end point are on a line.";
    return false;
  }
  return true;
}
