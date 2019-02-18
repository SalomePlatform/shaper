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

#include <GeomAlgoAPI_Ellipsoid.h>

#include <gp_Ax2.hxx>
#include <gp_Elips.hxx>
#include <gp_GTrsf.hxx>
#include <BRepBuilderAPI_GTransform.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepOffsetAPI_Sewing.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

//=================================================================================================

GeomAlgoAPI_Ellipsoid::GeomAlgoAPI_Ellipsoid(const double theAx,
                                             const double theBy,
                                             const double theCz,
                                             const double theZCut1,
                                             const double theZCut2)
{
  myAx = theAx;
  myBy = theBy;
  myCz = theCz;
  myZCut1 = theZCut1;
  myZCut2 = theZCut2;
}

//=================================================================================================
bool GeomAlgoAPI_Ellipsoid::check()
{
  if (myAx < Precision::Confusion()) {
    myError = "Ellipsoid builder :: ax is negative or null.";
    return false;
  } else if (myBy < Precision::Confusion()) {
    myError = "Ellipsoid builder :: by is negative or null.";
    return false;
  } else if (myCz < Precision::Confusion()) {
    myError = "Ellipsoid builder :: cz is negative or null.";
    return false;
  } else if (myZCut1 < 0.) {
    myError = "Ellipsoid builder :: zcut1 is negative.";
    return false;
  } else if (myZCut2 < 0.) {
    myError = "Ellipsoid builder :: zcut2 is negative.";
    return false;
  } else if (myZCut1 < Precision::Confusion() && myZCut2 < Precision::Confusion()) {
    myError = "Ellipsoid builder :: zcut1 and zcut2 are null.";
    return false;
  }

  return true;
}

//=================================================================================================
void GeomAlgoAPI_Ellipsoid::build()
{
  myCreatedFaces.clear();

  BRepOffsetAPI_Sewing aSewer;
  gp_Ax2 aRefAx2;
  gp_Elips anElips;

  gp_Pnt anOrigin(0., 0., 0.);
  gp_Dir aDirX(1., 0., 0.);
  gp_Dir aDirY(0., 1., 0.);
  gp_Dir aDirZ(0., 0., 1.);
  gp_Ax1 aZAxis(anOrigin, aDirZ);

  // Calculate the parameters needed to make the edges and the faces
  // gp_Elips needs the second parameter to be greater than the third (major axis)
  if (myCz < myAx) {
    aRefAx2 = gp_Ax2(anOrigin, aDirY, aDirX);
    anElips = gp_Elips(aRefAx2, myAx / 2., myCz / 2.);
  } else {
    aRefAx2 = gp_Ax2(anOrigin, aDirY, aDirZ);
    anElips = gp_Elips(aRefAx2, myCz / 2., myAx / 2.);
  }

  double aLowPositionFactor = sqrt(1. - (myZCut1 * myZCut1 * 4. / (myCz * myCz))) / 2.;
  double aHighPositionFactor = sqrt(1. - (myZCut2 * myZCut2 * 4. / (myCz * myCz))) / 2.;

  double aXEndTop = myAx * aHighPositionFactor;
  double aXEndBottom = myAx *  aLowPositionFactor;

  // Build the XZ ellipse
  gp_Pnt anEndPoint1(aXEndTop, 0., myZCut2);
  gp_Pnt anEndPoint2(aXEndBottom, 0., -myZCut1);
  BRepBuilderAPI_MakeEdge anElipsBuilder(anElips, anEndPoint1, anEndPoint2);
  anElipsBuilder.Build();
  TopoDS_Edge anOuterEdge = anElipsBuilder.Edge();

  // Perform a revolution based on the section to build a simple version of the outer face
  // (isotropic in XY)
  BRepPrimAPI_MakeRevol aRevolBuilder(anOuterEdge, aZAxis, 2. * M_PI, Standard_True);
  if (!aRevolBuilder.IsDone()) {
    myError = "Ellipsoid builder :: section revolution did not succeed";
    return;
  }

  gp_GTrsf aGTrsf;
  gp_Mat rot (1.,          0., 0.,
              0., myBy / myAx, 0.,
              0.,          0., 1.);

  aGTrsf.SetVectorialPart(rot);

  BRepBuilderAPI_GTransform aScaleBuilder(aRevolBuilder.Shape(), aGTrsf, true);
  if (!aScaleBuilder.IsDone()) {
    myError = "Ellipsoid builder :: scale did not succeed";
    return;
  }

  TopoDS_Face anOuterFace = TopoDS::Face(aScaleBuilder.Shape());
  aSewer.Add(TopoDS::Face(anOuterFace.Reversed()));

  // Build the high and low ellipse if needed
  gp_Ax2 aLowAx2;
  gp_Ax2 aHighAx2;
  gp_Elips aLowElips;
  gp_Elips aHighElips;
  if (myBy < myAx) {
    if ((myCz / 2. - myZCut1) > Precision::Confusion()) {
      aLowAx2 = gp_Ax2(gp_Pnt(0., 0., -myZCut1), aDirZ, aDirX);
      aLowElips = gp_Elips(aLowAx2, aLowPositionFactor * myAx, aLowPositionFactor * myBy);
    }
    if ((myCz / 2. - myZCut2) > Precision::Confusion()) {
      aHighAx2 = gp_Ax2(gp_Pnt(0., 0., myZCut2), aDirZ, aDirX);
      aHighElips = gp_Elips(aHighAx2, aHighPositionFactor * myAx, aHighPositionFactor * myBy);
    }
  } else {
    if ((myCz / 2. - myZCut1) > Precision::Confusion()) {
      aLowAx2 = gp_Ax2(gp_Pnt(0., 0., -myZCut1), aDirZ, aDirY);
      aLowElips = gp_Elips(aLowAx2, aLowPositionFactor * myBy, aLowPositionFactor * myAx);
    }
    if ((myCz / 2. - myZCut2) > Precision::Confusion()) {
      aHighAx2 = gp_Ax2(gp_Pnt(0., 0., myZCut2), aDirZ, aDirY);
      aHighElips = gp_Elips(aHighAx2, aHighPositionFactor * myBy, aHighPositionFactor * myAx);
    }
  }

  // Make higher and lower elliptical faces if needed
  if ((myCz / 2. - myZCut1) > Precision::Confusion()) {
    TopoDS_Face aBottomFace;
    BRepBuilderAPI_MakeEdge aBottomEdgeMk(aLowElips);
    aBottomEdgeMk.Build();
    BRepBuilderAPI_MakeWire aBottomWireMk;
    aBottomWireMk.Add(aBottomEdgeMk.Edge());
    BRepBuilderAPI_MakeFace aBottomFaceMk(aBottomWireMk.Wire());
    aBottomFace = aBottomFaceMk.Face();
    aSewer.Add(TopoDS::Face(aBottomFace.Reversed()));
  }
  if ((myCz / 2. - myZCut2) > Precision::Confusion()) {
    TopoDS_Face aTopFace;
    BRepBuilderAPI_MakeEdge aTopEdgeMk(aHighElips);
    aTopEdgeMk.Build();
    BRepBuilderAPI_MakeWire aTopWireMk;
    aTopWireMk.Add(aTopEdgeMk.Edge());
    BRepBuilderAPI_MakeFace aTopFaceMk(aTopWireMk.Wire());
    aTopFace = aTopFaceMk.Face();
    aSewer.Add(TopoDS::Face(aTopFace.Reversed()));
  }

  TopoDS_Shell aShell;
  aSewer.Perform();
  if ((myCz / 2. - myZCut2) > Precision::Confusion() ||
      (myCz / 2. - myZCut1) > Precision::Confusion()) {
    aShell = TopoDS::Shell(aSewer.SewedShape());
  } else {
    TopoDS_Builder aBuilder;
    aBuilder.MakeShell(aShell);
    aBuilder.Add(aShell, aSewer.SewedShape());
  }

  BRepBuilderAPI_MakeSolid *anEllipsoidMk = new BRepBuilderAPI_MakeSolid(aShell);
  anEllipsoidMk->Build();

  // Store and publish the results
  std::shared_ptr<GeomAPI_Shape> aResultShape =
    std::shared_ptr<GeomAPI_Shape>(new GeomAPI_Shape());
  aResultShape->setImpl(new TopoDS_Shape(anEllipsoidMk->Solid()));
  setShape(aResultShape);

  // Test on the shapes
  if (!(aResultShape).get() || aResultShape->isNull()) {
    myError = "Ellipsoid builder  :: resulting shape is null.";
    return;
  }

  setImpl(anEllipsoidMk);
  setBuilderType(OCCT_BRepBuilderAPI_MakeShape);

  setDone(true);
}
