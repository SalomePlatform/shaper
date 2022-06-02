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

#include "GeomAlgoAPI_BoundingBox.h"

#include <BRepBuilderAPI_Copy.hxx>
#include <Bnd_Box.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRep_Builder.hxx>
#include <Geom_Circle.hxx>
#include <ShapeAnalysis.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <gp_Pln.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <ShapeFix_Shape.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <Standard_ErrorHandler.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>

/**
* This function constructs and returns modified shape from the original one
* for singular cases. It is used for the method GetMinDistanceSingular.
*
* \param theShape the original shape
* \param theModifiedShape output parameter. The modified shape.
* \param theAddDist output parameter. The added distance for modified shape.
* \retval true if the shape is modified; false otherwise.
*
* \internal
*/
Standard_Boolean ModifyShape(const TopoDS_Shape  &theShape,
                             TopoDS_Shape  &theModifiedShape,
                             Standard_Real &theAddDist)
{
  TopExp_Explorer anExp;
  int nbf = 0;

  theAddDist = 0.;
  theModifiedShape.Nullify();

  for ( anExp.Init( theShape, TopAbs_FACE ); anExp.More(); anExp.Next() ) {
    nbf++;
    theModifiedShape = anExp.Current();
  }
  if(nbf==1) {
    TopoDS_Shape sh = theShape;
    while(sh.ShapeType()==TopAbs_COMPOUND) {
      TopoDS_Iterator it(sh);
      sh = it.Value();
    }
    Handle(Geom_Surface) S = BRep_Tool::Surface(TopoDS::Face(theModifiedShape));
    if(S->IsKind(STANDARD_TYPE(Geom_SphericalSurface)) ||
       S->IsKind(STANDARD_TYPE(Geom_ToroidalSurface)) ||
       S->IsUPeriodic()) {
      const Standard_Boolean isShell =
        (sh.ShapeType()==TopAbs_SHELL || sh.ShapeType()==TopAbs_FACE);

      if (!isShell && S->IsKind(STANDARD_TYPE(Geom_SphericalSurface))) {
        Handle(Geom_SphericalSurface) SS = Handle(Geom_SphericalSurface)::DownCast(S);
        gp_Pnt PC = SS->Location();
          BRep_Builder B;
          TopoDS_Vertex V;
          B.MakeVertex(V,PC,1.e-7);
          theModifiedShape = V;
          theAddDist = SS->Radius();
          return Standard_True;
        }
        if (!isShell && S->IsKind(STANDARD_TYPE(Geom_ToroidalSurface))) {
          Handle(Geom_ToroidalSurface) TS = Handle(Geom_ToroidalSurface)::DownCast(S);
          gp_Ax3 ax3 = TS->Position();
          Handle(Geom_Circle) C = new Geom_Circle(ax3.Ax2(),TS->MajorRadius());
          BRep_Builder B;
          TopoDS_Edge E;
          B.MakeEdge(E,C,1.e-7);
          theModifiedShape = E;
          theAddDist = TS->MinorRadius();
          return Standard_True;
        }

        // non solid case or any periodic surface (Mantis 22454).
        double U1,U2,V1,V2;
        // changes for 0020677: EDF 1219 GEOM: MinDistance gives 0 instead of 20.88
        //S->Bounds(U1,U2,V1,V2); changed by
        ShapeAnalysis::GetFaceUVBounds(TopoDS::Face(theModifiedShape),U1,U2,V1,V2);
        // end of changes for 020677 (dmv)
        Handle(Geom_RectangularTrimmedSurface) TrS1 =
          new Geom_RectangularTrimmedSurface(S,U1,(U1+U2)/2.,V1,V2);
        Handle(Geom_RectangularTrimmedSurface) TrS2 =
          new Geom_RectangularTrimmedSurface(S,(U1+U2)/2.,U2,V1,V2);
        TopoDS_Shape aMShape;

        TopoDS_Face F1 = BRepBuilderAPI_MakeFace(TrS1, Precision::Confusion());
        TopoDS_Face F2 = BRepBuilderAPI_MakeFace(TrS2, Precision::Confusion());

        if (isShell) {
          BRep_Builder B;
          B.MakeCompound(TopoDS::Compound(aMShape));
          B.Add(aMShape, F1);
          B.Add(aMShape, F2);
        } else {
          // The original shape is a solid.
          BRepBuilderAPI_Sewing aSewing (Precision::Confusion()*10.0);
          aSewing.Add(F1);
          aSewing.Add(F2);
          aSewing.Perform();
          aMShape = aSewing.SewedShape();
          BRep_Builder B;
          TopoDS_Solid aSolid;
          B.MakeSolid(aSolid);
          B.Add(aSolid, aMShape);
          aMShape = aSolid;
        }

        Handle(ShapeFix_Shape) sfs = new ShapeFix_Shape;
        sfs->Init(aMShape);
        sfs->SetPrecision(1.e-6);
        sfs->SetMaxTolerance(1.0);
        sfs->Perform();
        theModifiedShape = sfs->Shape();
        return Standard_True;
      }
    }

    theModifiedShape = theShape;
    return Standard_False;
  }

//=======================================================================
// function : GetMinDistanceSingular
//=======================================================================
double GetMinDistanceSingular(const TopoDS_Shape& aSh1,
                              const TopoDS_Shape& aSh2,
                              gp_Pnt& Ptmp1, gp_Pnt& Ptmp2)
{
  TopoDS_Shape     tmpSh1;
  TopoDS_Shape     tmpSh2;
  Standard_Real    AddDist1 = 0.;
  Standard_Real    AddDist2 = 0.;
  Standard_Boolean IsChange1 = ModifyShape(aSh1, tmpSh1, AddDist1);
  Standard_Boolean IsChange2 = ModifyShape(aSh2, tmpSh2, AddDist2);

  if( !IsChange1 && !IsChange2 )
    return -2.0;

  BRepExtrema_DistShapeShape dst(tmpSh1,tmpSh2);
  if (dst.IsDone()) {
    double MinDist = 1.e9;
    gp_Pnt PMin1, PMin2, P1, P2;
    for (int i = 1; i <= dst.NbSolution(); i++) {
      P1 = dst.PointOnShape1(i);
      P2 = dst.PointOnShape2(i);
      Standard_Real Dist = P1.Distance(P2);
      if (MinDist > Dist) {
        MinDist = Dist;
        PMin1 = P1;
        PMin2 = P2;
      }
    }
    if(MinDist<1.e-7) {
      Ptmp1 = PMin1;
      Ptmp2 = PMin2;
    }
    else {
      gp_Dir aDir(gp_Vec(PMin1,PMin2));
      if( MinDist > (AddDist1+AddDist2) ) {
        Ptmp1 = gp_Pnt(PMin1.X() + aDir.X()*AddDist1,
                       PMin1.Y() + aDir.Y()*AddDist1,
                       PMin1.Z() + aDir.Z()*AddDist1);
        Ptmp2 = gp_Pnt(PMin2.X() - aDir.X()*AddDist2,
                       PMin2.Y() - aDir.Y()*AddDist2,
                       PMin2.Z() - aDir.Z()*AddDist2);
        return (MinDist - AddDist1 - AddDist2);
      }
      else {
        if( AddDist1 > 0 ) {
          Ptmp1 = gp_Pnt(PMin1.X() + aDir.X()*AddDist1,
                         PMin1.Y() + aDir.Y()*AddDist1,
                         PMin1.Z() + aDir.Z()*AddDist1);
          Ptmp2 = Ptmp1;
        }
        else {
          Ptmp2 = gp_Pnt(PMin2.X() - aDir.X()*AddDist2,
                         PMin2.Y() - aDir.Y()*AddDist2,
                         PMin2.Z() - aDir.Z()*AddDist2);
          Ptmp1 = Ptmp2;
        }
      }
    }
    double res = MinDist - AddDist1 - AddDist2;
    if(res<0.) res = 0.0;
    return res;
  }
  return -2.0;
}

//=======================================================================
// function : GetMinDistance
//=======================================================================
Standard_Real GetMinDistance(const TopoDS_Shape& theShape1,
                             const TopoDS_Shape& theShape2,
                             gp_Pnt& thePnt1, gp_Pnt& thePnt2)
{
  Standard_Real aResult = 1.e9;

  // Issue 0020231: A min distance bug with torus and vertex.
  // Make GetMinDistance() return zero if a sole VERTEX is inside any of SOLIDs

  // which of shapes consists of only one vertex?
  TopExp_Explorer exp1(theShape1,TopAbs_VERTEX), exp2(theShape2,TopAbs_VERTEX);
  TopoDS_Shape V1 = exp1.More() ? exp1.Current() : TopoDS_Shape();
  TopoDS_Shape V2 = exp2.More() ? exp2.Current() : TopoDS_Shape();
  exp1.Next(); exp2.Next();
  if ( exp1.More() ) V1.Nullify();
  if ( exp2.More() ) V2.Nullify();
  // vertex and container of solids
  TopoDS_Shape V = V1.IsNull() ? V2 : V1;
  TopoDS_Shape S = V1.IsNull() ? theShape1 : theShape2;
  if ( !V.IsNull() ) {
    // classify vertex against solids
    gp_Pnt p = BRep_Tool::Pnt( TopoDS::Vertex( V ) );
    for ( exp1.Init( S, TopAbs_SOLID ); exp1.More(); exp1.Next() ) {
      BRepClass3d_SolidClassifier classifier( exp1.Current(), p, 1e-6);
      if ( classifier.State() == TopAbs_IN ) {
        thePnt1 = p;
        thePnt2 = p;
        return 0.0;
      }
    }
  }

  aResult = GetMinDistanceSingular(theShape1, theShape2, thePnt1, thePnt2);


  BRepExtrema_DistShapeShape dst (theShape1, theShape2);
  if (dst.IsDone()) {
    gp_Pnt P1, P2;

    for (int i = 1; i <= dst.NbSolution(); i++) {
      P1 = dst.PointOnShape1(i);
      P2 = dst.PointOnShape2(i);

      Standard_Real Dist = P1.Distance(P2);
      if (aResult < 0 || aResult > Dist) {
        aResult = Dist;
        thePnt1 = P1;
        thePnt2 = P2;
      }
    }
  }

  return aResult;
}

//=======================================================================
// function : PreciseBoundingBox
//=======================================================================
Standard_Boolean PreciseBoundingBox(const TopoDS_Shape &theShape, Bnd_Box &theBox)
{
  if (theBox.IsVoid()) BRepBndLib::Add( theShape, theBox );
  if (theBox.IsVoid()) return Standard_False;

  Standard_Real aBound[6];
  theBox.Get(aBound[0], aBound[2], aBound[4], aBound[1], aBound[3], aBound[5]);

  Standard_Integer i;
  const gp_Pnt aMid(0.5*(aBound[1] + aBound[0]),  // XMid
                    0.5*(aBound[3] + aBound[2]),  // YMid
                    0.5*(aBound[5] + aBound[4])); // ZMid
  const gp_XYZ aSize(aBound[1] - aBound[0],       // DX
                     aBound[3] - aBound[2],       // DY
                     aBound[5] - aBound[4]);      // DZ
  const gp_Pnt aPnt[6] =
    {
      gp_Pnt(aBound[0] - (aBound[1] - aBound[0]), aMid.Y(), aMid.Z()), // XMin
      gp_Pnt(aBound[1] + (aBound[1] - aBound[0]), aMid.Y(), aMid.Z()), // XMax
      gp_Pnt(aMid.X(), aBound[2] - (aBound[3] - aBound[2]), aMid.Z()), // YMin
      gp_Pnt(aMid.X(), aBound[3] + (aBound[3] - aBound[2]), aMid.Z()), // YMax
      gp_Pnt(aMid.X(), aMid.Y(), aBound[4] - (aBound[5] - aBound[4])), // ZMin
      gp_Pnt(aMid.X(), aMid.Y(), aBound[5] + (aBound[5] - aBound[4]))  // ZMax
    };
  const gp_Dir aDir[3] = { gp::DX(), gp::DY(), gp::DZ() };
  const Standard_Real aPlnSize[3] =
    {
      0.5*Max(aSize.Y(), aSize.Z()), // XMin, XMax planes
      0.5*Max(aSize.X(), aSize.Z()), // YMin, YMax planes
      0.5*Max(aSize.X(), aSize.Y())  // ZMin, ZMax planes
    };
  gp_Pnt aPMin[2];

  for (i = 0; i < 6; i++) {
    const Standard_Integer iHalf = i/2;
    const gp_Pln aPln(aPnt[i], aDir[iHalf]);
    BRepBuilderAPI_MakeFace aMkFace(aPln, -aPlnSize[iHalf], aPlnSize[iHalf],
                                    -aPlnSize[iHalf], aPlnSize[iHalf]);

    if (!aMkFace.IsDone()) {
      return Standard_False;
    }

    TopoDS_Shape aFace = aMkFace.Shape();

    // Get minimal distance between planar face and shape.
    Standard_Real aMinDist = GetMinDistance(aFace, theShape, aPMin[0], aPMin[1]);

    if (aMinDist < 0.) {
      return Standard_False;
    }

    aBound[i] = aPMin[1].Coord(iHalf + 1);
  }

  // Update Bounding box with the new values.
  theBox.SetVoid();
  theBox.Update(aBound[0], aBound[2], aBound[4], aBound[1], aBound[3], aBound[5]);

  return Standard_True;
}

//=================================================================================================
bool GetBoundingBox(const std::shared_ptr<GeomAPI_Shape>& theShape,
                    double& theXmin, double& theXmax,
                    double& theYmin, double& theYmax,
                    double& theZmin, double& theZmax,
                    std::string& theError)
{
  #ifdef _DEBUG
  std::cout << "GetBoundingBox " << std::endl;
  #endif

  if (!theShape.get()) {
    theError = "GetBoundingBox : An invalid argument";
    return false;
  }

  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();

  //Compute the parameters
  Bnd_Box B;
  try {
    OCC_CATCH_SIGNALS;
    BRepBuilderAPI_Copy aCopyTool (aShape);
    if (!aCopyTool.IsDone()) {
      theError = "GetBoundingBox Error: Bad shape detected";
      return false;
    }

    aShape = aCopyTool.Shape();

    // remove triangulation to obtain more exact boundaries
    BRepTools::Clean(aShape);

    BRepBndLib::Add(aShape, B);

    if (!PreciseBoundingBox(aShape, B)) {
      theError = "GetBoundingBox Error: Bounding box cannot be precised";
      return false;
    }

    B.Get(theXmin, theYmin, theZmin, theXmax, theYmax, theZmax);
  }
  catch (Standard_Failure& aFail) {
    theError = aFail.GetMessageString();
    return false;
  }
  return true;
}
