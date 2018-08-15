// Copyright (C) 2018-20xx  CEA/DEN, EDF R&D
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

#include "GeomAPI_Shell.h"

#include "GeomAPI_Ax3.h"
#include "GeomAPI_Box.h"
#include "GeomAPI_Cone.h"
#include "GeomAPI_Cylinder.h"
#include "GeomAPI_Face.h"
#include "GeomAPI_Pnt.h"
#include "GeomAPI_Sphere.h"
#include "GeomAPI_Torus.h"
#include "GeomAPI_Wire.h"
#include "GeomAPI_XYZ.h"

#include <BRep_Builder.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#include <map>

//=================================================================================================
GeomAPI_Shell::GeomAPI_Shell()
{
  TopoDS_Shell* aShell = new TopoDS_Shell();

  BRep_Builder aBuilder;
  aBuilder.MakeShell(*aShell);

  this->setImpl(aShell);
}

//=================================================================================================
GeomAPI_Shell::GeomAPI_Shell(const std::shared_ptr<GeomAPI_Shape>& theShape)
{
  if (!theShape->isNull() && theShape->isShell()) {
    setImpl(new TopoDS_Shape(theShape->impl<TopoDS_Shape>()));
  }
}

//=================================================================================================
std::shared_ptr<GeomAPI_Sphere> GeomAPI_Shell::getSphere() const
{
  bool isSphere = true;
  bool isFirstFace = true;
  GeomSpherePtr aSphere;

  for (TopExp_Explorer anExp(impl<TopoDS_Shape>(), TopAbs_FACE); anExp.More(); anExp.Next()) {
    GeomFacePtr aFace(new GeomAPI_Face);
    aFace->setImpl(new TopoDS_Shape(anExp.Current()));

    GeomSpherePtr aCurSphere = aFace->getSphere();
    if (!aCurSphere) {
      isSphere = false;
      break;
    }

    if (isFirstFace) {
      aSphere = aCurSphere;
      isFirstFace = false;
    }
    else if (aSphere->center()->distance(aCurSphere->center()) >= Precision::Confusion() ||
             Abs(aSphere->radius() - aCurSphere->radius()) >= Precision::Confusion()) {
      isSphere = false;
      break;
    }
  }

  return isSphere ? aSphere : GeomSpherePtr();
}

//=================================================================================================
std::shared_ptr<GeomAPI_Cylinder> GeomAPI_Shell::getCylinder() const
{
  bool isCylinder = true;
  bool isFirstFace = true;

  GeomPointPtr aLocation;
  GeomDirPtr anAxis;
  double aRadius;
  double aHeight;

  for (TopExp_Explorer anExp(impl<TopoDS_Shape>(), TopAbs_FACE); anExp.More(); anExp.Next()) {
    GeomFacePtr aFace(new GeomAPI_Face);
    aFace->setImpl(new TopoDS_Shape(anExp.Current()));

    GeomCylinderPtr aCurCyl = aFace->getCylinder();
    if (!aCurCyl) {
      isCylinder = false;
      break;
    }

    if (isFirstFace) {
      aLocation = aCurCyl->location();
      anAxis = aCurCyl->axis();
      aRadius = aCurCyl->radius();
      aHeight = aCurCyl->height();
      isFirstFace = false;
    }
    else {
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

      double aMinHeight = 0.0;
      double aMaxHeight = aHeight;

      std::shared_ptr<GeomAPI_XYZ> aCurCylLoc = aCurCyl->location()->xyz();
      std::shared_ptr<GeomAPI_XYZ> aCurCylLocHeight =
        aCurCylLoc->added(aCurCyl->axis()->xyz()->multiplied(aCurCyl->height()));

      std::shared_ptr<GeomAPI_XYZ> anAxisXYZ = anAxis->xyz();

      double aDist = anAxisXYZ->dot(aCurCylLoc->decreased(aLocation->xyz()));
      if (aDist < aMinHeight)
        aMinHeight = aDist;
      else if (aDist > aMaxHeight)
        aMaxHeight = aDist;

      aDist = anAxisXYZ->dot(aCurCylLocHeight->decreased(aLocation->xyz()));
      if (aDist < aMinHeight)
        aMinHeight = aDist;
      else if (aDist > aMaxHeight)
        aMaxHeight = aDist;

      if (aMinHeight < 0.0) {
        // move location of full cylinder
        aLocation->setX(aLocation->x() + aMinHeight * anAxis->x());
        aLocation->setY(aLocation->y() + aMinHeight * anAxis->y());
        aLocation->setZ(aLocation->z() + aMinHeight * anAxis->z());
      }

      aHeight = aMaxHeight - aMinHeight;
    }
  }

  GeomCylinderPtr aCylinder;
  if (isCylinder)
    aCylinder = GeomCylinderPtr(new GeomAPI_Cylinder(aLocation, anAxis, aRadius, aHeight));
  return aCylinder;
}

//=================================================================================================
std::shared_ptr<GeomAPI_Cone> GeomAPI_Shell::getCone() const
{
  bool isCone = true;
  bool isFirstFace = true;

  GeomPointPtr anApex;
  GeomDirPtr anAxis;
  double aSemiAngle, aCosSemiAngle;
  double aHeight1, aHeight2;

  for (TopExp_Explorer anExp(impl<TopoDS_Shape>(), TopAbs_FACE); anExp.More(); anExp.Next()) {
    GeomFacePtr aFace(new GeomAPI_Face);
    aFace->setImpl(new TopoDS_Shape(anExp.Current()));

    GeomConePtr aCurCone = aFace->getCone();
    if (!aCurCone) {
      isCone = false;
      break;
    }

    if (isFirstFace) {
      anApex = aCurCone->apex();
      anAxis = aCurCone->axis();
      aSemiAngle = aCurCone->semiAngle();
      aCosSemiAngle = Cos(aSemiAngle);
      aHeight1 = aCurCone->radius1() * aCosSemiAngle;
      aHeight2 = aCurCone->radius2() * aCosSemiAngle;
      isFirstFace = false;
    }
    else {
      // check equal locations
      if (anApex->distance(aCurCone->apex()) >= Precision::Confusion() ||
      // check equal angles
          Abs(aSemiAngle - aCurCone->semiAngle()) >= Precision::Confusion() ||
      // check directions are collinear
          !anAxis->isParallel(aCurCone->axis())) {
        isCone = false;
        break;
      }

      double aSign = anAxis->dot(aCurCone->axis());
      double aCurSemiAngle = aCurCone->semiAngle();
      double aCosCurSemiAngle = Cos(aSemiAngle);

      double aH = aCurCone->radius1() * aCosCurSemiAngle * aSign;
      if (aH < aHeight1)
        aHeight1 = aH;
      else if (aH > aHeight2)
        aHeight2 = aH;

      aH = aCurCone->radius2() * aCosCurSemiAngle * aSign;
      if (aH < aHeight1)
        aHeight1 = aH;
      else if (aH > aHeight2)
        aHeight2 = aH;
    }
  }

  GeomConePtr aCone;
  if (isCone) {
    GeomPointPtr aLocation(new GeomAPI_Pnt(
      anApex->xyz()->added(anAxis->xyz()->multiplied(aHeight1))));
    double aRadius1 = aHeight1 * Tan(aSemiAngle);
    double aRadius2 = aHeight2 * Tan(aSemiAngle);

    aCone = GeomConePtr(new GeomAPI_Cone(aLocation, anAxis, aSemiAngle, aRadius1, aRadius2));
  }
  return aCone;
}

//=================================================================================================
std::shared_ptr<GeomAPI_Torus> GeomAPI_Shell::getTorus() const
{
  bool isTorus = true;
  bool isFirstFace = true;
  GeomTorusPtr aTorus;

  for (TopExp_Explorer anExp(impl<TopoDS_Shape>(), TopAbs_FACE); anExp.More(); anExp.Next()) {
    GeomFacePtr aFace(new GeomAPI_Face);
    aFace->setImpl(new TopoDS_Shape(anExp.Current()));

    GeomTorusPtr aCurTorus = aFace->getTorus();
    if (!aCurTorus) {
      isTorus = false;
      break;
    }

    if (isFirstFace) {
      aTorus = aCurTorus;
      isFirstFace = false;
    }
    else {
      // compare radii
      if (Abs(aTorus->majorRadius() - aCurTorus->majorRadius()) >= Precision::Confusion() ||
          Abs(aTorus->minorRadius() - aCurTorus->minorRadius()) >= Precision::Confusion() ||
      // check equal centers
          aTorus->center()->distance(aCurTorus->center()) >= Precision::SquareConfusion() ||
      // check directions are collinear
          !aTorus->direction()->isParallel(aCurTorus->direction())) {
        isTorus = false;
        break;
      }
    }
  }

  return isTorus ? aTorus : GeomTorusPtr();
}

//=================================================================================================
std::shared_ptr<GeomAPI_Box> GeomAPI_Shell::getParallelepiped() const
{
  struct Plane
  {
    std::shared_ptr<GeomAPI_Ax3> myAxes;
    double myWidth;
    double myDepth;
    double myHeight;
  } aPlanes[6];
  std::map<int, int> aParallelPlanes;

  int aNbPlanes = 0;
  for (TopExp_Explorer anExp(impl<TopoDS_Shape>(), TopAbs_WIRE);
       anExp.More() && aNbPlanes < 6; anExp.Next()) {
    GeomWirePtr aWire(new GeomAPI_Wire);
    aWire->setImpl(new TopoDS_Shape(anExp.Current()));

    std::list<GeomPointPtr> aCorners;
    if (aWire->isRectangle(aCorners)) {
      // convert rectangle to plane with dimensions
      GeomPointPtr anOrigin = aCorners.front();
      aCorners.pop_front();

      GeomPointPtr aFront = aCorners.front();
      GeomPointPtr aBack = aCorners.back();

      aPlanes[aNbPlanes].myWidth = aBack->distance(anOrigin);
      aPlanes[aNbPlanes].myDepth = aFront->distance(anOrigin);
      aPlanes[aNbPlanes].myHeight = Precision::Infinite();

      GeomDirPtr aDX(new GeomAPI_Dir(aBack->x() - anOrigin->x(),
                                     aBack->y() - anOrigin->y(),
                                     aBack->z() - anOrigin->z()));
      GeomDirPtr aDY(new GeomAPI_Dir(aFront->x() - anOrigin->x(),
                                     aFront->y() - anOrigin->y(),
                                     aFront->z() - anOrigin->z()));
      GeomDirPtr aDZ(new GeomAPI_Dir(aDX->cross(aDY)));
      aPlanes[aNbPlanes].myAxes =
          std::shared_ptr<GeomAPI_Ax3>(new GeomAPI_Ax3(anOrigin, aDX, aDZ));

      // find parallel plane
      for (int i = 0; i < aNbPlanes; ++i) {
        double aDot = aPlanes[i].myAxes->normal()->dot(aDZ);
        if (Abs(aDot + 1.0) < Precision::Angular()) {
          if (aParallelPlanes.find(i) == aParallelPlanes.end())
            aParallelPlanes[i] = aNbPlanes;
          else
            break; // parallel planes already exist
        }
      }

      ++aNbPlanes;

    } else
      break;
  }

  if (aNbPlanes != 6 || aParallelPlanes.size() != 3) // not a parallelepiped
    return GeomBoxPtr();

  // calculate heights for planes computed by rectangles
  for (std::map<int, int>::iterator it = aParallelPlanes.begin();
       it != aParallelPlanes.end(); ++it) {
    GeomDirPtr aNormal = aPlanes[it->first].myAxes->normal();
    GeomPointPtr anOrigin = aPlanes[it->first].myAxes->origin();
    GeomPointPtr aNeighbor = aPlanes[it->second].myAxes->origin();

    aPlanes[it->first].myHeight =
    aPlanes[it->second].myHeight =
        aNormal->xyz()->dot( aNeighbor->xyz()->decreased(anOrigin->xyz()) );
  }

  // check if the box is oriented in the main axes
  int anIndex = 0;
  for (int i = 0; i < 6; ++i) {
    if (Abs(aPlanes[i].myAxes->dirX()->x() - 1.) < Precision::Angular() &&
        Abs(aPlanes[i].myAxes->normal()->z() - 1.) < Precision::Angular()) {
      anIndex = i;
      break;
    }
  }

  // construct a box
  GeomBoxPtr aBox(new GeomAPI_Box(aPlanes[anIndex].myAxes,
                                  aPlanes[anIndex].myWidth,
                                  aPlanes[anIndex].myDepth,
                                  aPlanes[anIndex].myHeight));
  return aBox;
}

//=================================================================================================
GeomPointPtr GeomAPI_Shell::middlePoint() const
{
  GeomPointPtr anInnerPoint;

  const TopoDS_Shell& aShell = impl<TopoDS_Shell>();
  if (aShell.IsNull())
    return anInnerPoint;

  GProp_GProps aProps;
  BRepGProp::SurfaceProperties(aShell, aProps, 1.e-4);

  gp_Pnt aPnt = aProps.CentreOfMass();
  anInnerPoint = GeomPointPtr(new GeomAPI_Pnt(aPnt.X(), aPnt.Y(), aPnt.Z()));
  return anInnerPoint;
}
