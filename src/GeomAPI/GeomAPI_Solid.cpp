// Copyright (C) 2018-2020  CEA/DEN, EDF R&D
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

#include "GeomAPI_Solid.h"

#include "GeomAPI_Box.h"
#include "GeomAPI_Cone.h"
#include "GeomAPI_Cylinder.h"
#include "GeomAPI_Dir.h"
#include "GeomAPI_Face.h"
#include "GeomAPI_Pln.h"
#include "GeomAPI_Pnt.h"
#include "GeomAPI_Shell.h"
#include "GeomAPI_Sphere.h"
#include "GeomAPI_Torus.h"
#include "GeomAPI_XYZ.h"

#include <BRep_Builder.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Wire.hxx>

//==================================================================================================
GeomAPI_Solid::GeomAPI_Solid() : GeomAPI_Shape()
{
}

//==================================================================================================
GeomAPI_Solid::GeomAPI_Solid(const std::shared_ptr<GeomAPI_Shape>& theShape)
{
  if (!theShape->isNull() && theShape->isSolid()) {
    setImpl(new TopoDS_Shape(theShape->impl<TopoDS_Shape>()));
  }
}

//==================================================================================================
std::shared_ptr<GeomAPI_Sphere> GeomAPI_Solid::getSphere() const
{
  GeomSpherePtr aSphere;
  ListOfShape aShells = subShapes(SHELL);
  if (aShells.size() == 1)
    aSphere = aShells.front()->shell()->getSphere();
  return aSphere;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Cylinder> GeomAPI_Solid::getCylinder() const
{
  bool isCylinder = false;

  GeomPointPtr aLocation;
  GeomDirPtr anAxis;
  double aRadius = 0.0;
  double aHeight = 0.0;

  GeomPlanePtr aCaps[2];

  for (TopExp_Explorer anExp(impl<TopoDS_Shape>(), TopAbs_FACE); anExp.More(); anExp.Next()) {
    GeomFacePtr aFace(new GeomAPI_Face);
    aFace->setImpl(new TopoDS_Shape(anExp.Current()));

    GeomCylinderPtr aCurCyl = aFace->getCylinder();
    if (aCurCyl) {
      if (isCylinder) { // at least one cylindrical face is found
        // compare radii
        if (Abs(aRadius - aCurCyl->radius()) >= Precision::Confusion() ||
          // check directions are collinear
          !anAxis->isParallel(aCurCyl->axis()) ||
          // check current center is on the main axis
          anAxis->xyz()->cross(aLocation->xyz()->decreased(aCurCyl->location()->xyz())
          )->squareModulus() >= Precision::SquareConfusion()) {
          isCylinder = false;
          break;
        }
      }
      else { // first cylinder is found
        aLocation = aCurCyl->location();
        if (anAxis) {
          // the plane is found => compare directions
          if (!anAxis->isParallel(aCurCyl->axis()))
            break;
        }
        else
          anAxis = aCurCyl->axis();
        aRadius = aCurCyl->radius();
        aHeight = aCurCyl->height();
        isCylinder = true;
      }
    }
    else {
      // check the face is planar
      bool isPlaneApplicable = false;
      GeomPlanePtr aCurPln = aFace->getPlane();
      if (aCurPln) {
        // verify the plane is already exists
        int aLastPlanIndex = 0;
        while (aLastPlanIndex < 2) {
          if (!aCaps[aLastPlanIndex]) {
            // add new plane
            aCaps[aLastPlanIndex] = aCurPln;
            break;
          }
          if (aCaps[aLastPlanIndex]->isCoincident(aCurPln))
            break;
          ++aLastPlanIndex;
        }

        isPlaneApplicable = aLastPlanIndex < 2;
      }

      if (isPlaneApplicable) {
        if (!anAxis) // no cylinder is found, store the normal as further cylinder's axis
          anAxis = aCurPln->direction();
      }
      else {
        isCylinder = false;
        break;
      }
    }
  }

  isCylinder = isCylinder && aCaps[0] && aCaps[1] &&
               aCaps[0]->direction()->isParallel(anAxis) &&
               aCaps[1]->direction()->isParallel(anAxis);

  GeomCylinderPtr aCylinder;
  if (isCylinder) {
    // intersect planes with cylinder's axis
    std::shared_ptr<GeomAPI_XYZ> anAxisXYZ = anAxis->xyz();
    std::shared_ptr<GeomAPI_XYZ> aLocationXYZ = aLocation->xyz();
    double aParam0 = anAxisXYZ->dot( aCaps[0]->location()->xyz()->decreased(aLocationXYZ) );
    double aParam1 = anAxisXYZ->dot( aCaps[1]->location()->xyz()->decreased(aLocationXYZ) );
    if (aParam0 > aParam1 + Precision::Confusion()) {
      double tmp = aParam0;
      aParam0 = aParam1;
      aParam1 = tmp;
    }

    // update location of cylinder to be coincident with one of planes
    aLocation->setX(aLocation->x() + aParam0 * anAxis->x());
    aLocation->setY(aLocation->y() + aParam0 * anAxis->y());
    aLocation->setZ(aLocation->z() + aParam0 * anAxis->z());

    aHeight = aParam1 - aParam0;

    aCylinder = GeomCylinderPtr(new GeomAPI_Cylinder(aLocation, anAxis, aRadius, aHeight));
  }
  return aCylinder;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Cone> GeomAPI_Solid::getCone() const
{
  bool isCone = false;

  GeomPointPtr anApex;
  GeomDirPtr anAxis;
  double aSemiAngle = 0.0;

  GeomPlanePtr aCaps[2];

  for (TopExp_Explorer anExp(impl<TopoDS_Shape>(), TopAbs_FACE); anExp.More(); anExp.Next()) {
    GeomFacePtr aFace(new GeomAPI_Face);
    aFace->setImpl(new TopoDS_Shape(anExp.Current()));

    GeomConePtr aCurCone = aFace->getCone();
    if (aCurCone) {
      if (isCone) { // at least one conical face is found
        // check equal apexes
        if (anApex->distance(aCurCone->apex()) >= Precision::Confusion() ||
        // check semi-angle
            Abs(aSemiAngle - aCurCone->semiAngle() >= Precision::Confusion()) ||
        // check axes are collinear
            !anAxis->isParallel(aCurCone->axis())) {
          isCone = false;
          break;
        }
      }
      else { // first cone is found
        anApex = aCurCone->apex();
        if (anAxis) {
          // the plane is found => compare directions
          if (!anAxis->isParallel(aCurCone->axis()))
            break;
        }
        else
          anAxis = aCurCone->axis();
        aSemiAngle = aCurCone->semiAngle();
        isCone = true;
      }
    }
    else {
      // check the face is planar
      bool isPlaneApplicable = false;
      GeomPlanePtr aCurPln = aFace->getPlane();
      if (aCurPln) {
        // verify the plane is already exists
        int aLastPlanIndex = 0;
        while (aLastPlanIndex < 2) {
          if (!aCaps[aLastPlanIndex]) {
            // add new plane
            aCaps[aLastPlanIndex] = aCurPln;
            break;
          }
          if (aCaps[aLastPlanIndex]->isCoincident(aCurPln))
            break;
          ++aLastPlanIndex;
        }

        isPlaneApplicable = aLastPlanIndex < 2;
      }

      if (isPlaneApplicable) {
        if (!anAxis) // no cone is found, store the normal as further cone's axis
          anAxis = aCurPln->direction();
      }
      else {
        isCone = false;
        break;
      }
    }
  }

  isCone = isCone && aCaps[0] && aCaps[0]->direction()->isParallel(anAxis);
  if (isCone && aCaps[1]) // cone map have only one cap, if it is bounded by the apex
    isCone = aCaps[1]->direction()->isParallel(anAxis);

  GeomConePtr aCone;
  if (isCone) {
    // intersect planes with cone's axis
    std::shared_ptr<GeomAPI_XYZ> anAxisXYZ = anAxis->xyz();
    std::shared_ptr<GeomAPI_XYZ> anApexXYZ = anApex->xyz();
    double aParam0 = anAxisXYZ->dot(aCaps[0]->location()->xyz()->decreased(anApexXYZ));
    double aParam1 =
        aCaps[1] ? anAxisXYZ->dot(aCaps[1]->location()->xyz()->decreased(anApexXYZ)) : 0.0;
    if (aParam0 <= 0.0 && aParam1 <= 0.0) {
      // reverse axis to make smaller cap be the first
      anAxis->reverse();
      aParam0 = -aParam0;
      aParam1 = -aParam1;
    }
    if (aParam0 > aParam1 + Precision::Confusion()) {
      double tmp = aParam0;
      aParam0 = aParam1;
      aParam1 = tmp;
    }

    // calculate location of cone to be coincident with one of planes
    GeomPointPtr aLocation(new GeomAPI_Pnt(
        anApex->x() + aParam0 * anAxis->x(),
        anApex->y() + aParam0 * anAxis->y(),
        anApex->z() + aParam0 * anAxis->z()));

    // calculate radii of caps
    aParam0 *= Tan(aSemiAngle);
    aParam1 *= Tan(aSemiAngle);

    aCone = GeomConePtr(new GeomAPI_Cone(aLocation, anAxis, aSemiAngle, aParam0, aParam1));
  }
  return aCone;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Torus> GeomAPI_Solid::getTorus() const
{
  GeomTorusPtr aTorus;
  ListOfShape aShells = subShapes(SHELL);
  if (aShells.size() == 1)
    aTorus = aShells.front()->shell()->getTorus();
  return aTorus;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Box> GeomAPI_Solid::getParallelepiped() const
{
  GeomBoxPtr aBox;
  ListOfShape aShells = subShapes(SHELL);
  if (aShells.size() == 1)
    aBox = aShells.front()->shell()->getParallelepiped();
  return aBox;
}

//==================================================================================================
GeomPointPtr GeomAPI_Solid::middlePoint() const
{
  GeomPointPtr anInnerPoint;

  const TopoDS_Solid& aSolid = impl<TopoDS_Solid>();
  if (aSolid.IsNull())
    return anInnerPoint;

  GProp_GProps aProps;
  BRepGProp::VolumeProperties(aSolid, aProps, 1.e-4);

  gp_Pnt aPnt = aProps.CentreOfMass();
  anInnerPoint = GeomPointPtr(new GeomAPI_Pnt(aPnt.X(), aPnt.Y(), aPnt.Z()));
  return anInnerPoint;
}
