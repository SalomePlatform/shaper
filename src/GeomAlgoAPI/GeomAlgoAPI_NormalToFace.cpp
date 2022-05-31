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

#include "GeomAlgoAPI_NormalToFace.h"

#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <gp_Ax3.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>
#include <TopoDS_Iterator.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBndLib.hxx>
#include <GeomLProp_SLProps.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <GeomAPI_Edge.h>

 /*!
   * \brief Return type of shape for explode. In case of compound it will be a type of its first sub shape.
   * \param theShape The shape to get type of.
   * \retval TopAbs_ShapeEnum Return type of shape for explode.
   */
TopAbs_ShapeEnum GetTypeOfSimplePart(const TopoDS_Shape& theShape)
{
  TopAbs_ShapeEnum aType = theShape.ShapeType();
  if      (aType == TopAbs_VERTEX)                             return TopAbs_VERTEX;
  else if (aType == TopAbs_EDGE  || aType == TopAbs_WIRE)      return TopAbs_EDGE;
  else if (aType == TopAbs_FACE  || aType == TopAbs_SHELL)     return TopAbs_FACE;
  else if (aType == TopAbs_SOLID || aType == TopAbs_COMPSOLID) return TopAbs_SOLID;
  else if (aType == TopAbs_COMPOUND) {
    // Only the iType of the first shape in the compound is taken into account
    TopoDS_Iterator It (theShape, Standard_False, Standard_False);
    if (It.More()) {
      return GetTypeOfSimplePart(It.Value());
    }
  }
  return TopAbs_SHAPE;
}

 /*!
   * \brief Get Local Coordinate System, corresponding to the given shape.
   * \param theShape The shape to get type of.
   * \retval gp_Ax3 Return axis.
   *
   * Origin of the LCS is situated at the shape's center of mass.
   * Axes of the LCS are obtained from shape's location or,
   * if the shape is a planar face, from position of its plane.
   */
gp_Ax3 GetPosition(const TopoDS_Shape& theShape)
{
  gp_Ax3 aResult;

  if (theShape.IsNull())
    return aResult;

  // Axes
  aResult.Transform(theShape.Location().Transformation());
  if (theShape.ShapeType() == TopAbs_FACE) {
    Handle(Geom_Surface) aGS = BRep_Tool::Surface(TopoDS::Face(theShape));
    if (!aGS.IsNull() && aGS->IsKind(STANDARD_TYPE(Geom_Plane))) {
      Handle(Geom_Plane) aGPlane = Handle(Geom_Plane)::DownCast(aGS);
      gp_Pln aPln = aGPlane->Pln();
      aResult = aPln.Position();
      // In case of reverse orinetation of the face invert the plane normal
      // (the face's normal does not mathc the plane's normal in this case)
      if (theShape.Orientation() == TopAbs_REVERSED) {
        gp_Dir Vx =  aResult.XDirection();
        gp_Dir N  =  aResult.Direction().Mirrored(Vx);
        gp_Pnt P  =  aResult.Location();
        aResult = gp_Ax3(P, N, Vx);
      }
    }
  }

  // Origin
  gp_Pnt aPnt;

  TopAbs_ShapeEnum aShType = theShape.ShapeType();

  if (aShType == TopAbs_VERTEX) {
    aPnt = BRep_Tool::Pnt(TopoDS::Vertex(theShape));
  } else {
    if (aShType == TopAbs_COMPOUND) {
      aShType = GetTypeOfSimplePart(theShape);
    }

    GProp_GProps aSystem;
    if (aShType == TopAbs_EDGE || aShType == TopAbs_WIRE)
      BRepGProp::LinearProperties(theShape, aSystem);
    else if (aShType == TopAbs_FACE || aShType == TopAbs_SHELL)
      BRepGProp::SurfaceProperties(theShape, aSystem);
    else
      BRepGProp::VolumeProperties(theShape, aSystem);

    aPnt = aSystem.CentreOfMass();
  }

  aResult.SetLocation(aPnt);

  return aResult;
}

//=================================================================================================
bool GeomAlgoAPI_NormalToFace::normal(GeomShapePtr theFace,
                                      GeomShapePtr thePoint,
                                      GeomEdgePtr  theNormal,
                                      std::string& theError)
{
  #ifdef _DEBUG
  std::cout << "GeomAlgoAPI_NormalToFace::normal" << std::endl;
  #endif

  if (!theFace.get()) {
    theError = "Face for normale calculation is null";
    return false;
  }

  TopoDS_Shape aShape = theFace->impl<TopoDS_Shape>();

  if (aShape.ShapeType() != TopAbs_FACE) {
    theError = "Shape for normale calculation is not a face";
    return false;
  }

  TopoDS_Face aFace = TopoDS::Face(aShape);

  // Point
  gp_Pnt p1 (0,0,0);
  // Point parameters on surface
  double u1, u2, v1, v2;
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
  Handle(ShapeAnalysis_Surface) aSurfAna = new ShapeAnalysis_Surface (aSurf);
  gp_Ax3 aPos = GetPosition(aFace);
  p1 = aPos.Location();
  // Set default starting point using UV bounds
  ShapeAnalysis::GetFaceUVBounds(aFace, u1, u2, v1, v2);
  gp_Pnt2d pUV ((u2 + u1) * 0.5, (v2 + v1) * 0.5);

  // Change to Vertex coord if selected
  if (thePoint.get()) {
    TopoDS_Shape anOptPnt = thePoint->impl<TopoDS_Shape>();
    if (anOptPnt.IsNull()) {
      theError = "Invalid shape given for point argument";
      return false;
    }
    p1 = BRep_Tool::Pnt(TopoDS::Vertex(anOptPnt));
    pUV = aSurfAna->ValueOfUV(p1, Precision::Confusion());
  }

  // Normal direction
  gp_Vec Vec1,Vec2;
  BRepAdaptor_Surface SF (aFace);
  SF.D1(pUV.X(), pUV.Y(), p1, Vec1, Vec2);
  if (Vec1.Magnitude() < Precision::Confusion()) {
    gp_Vec tmpV;
    gp_Pnt tmpP;
    SF.D1(pUV.X(), pUV.Y()-0.1, tmpP, Vec1, tmpV);
  }
  else if (Vec2.Magnitude() < Precision::Confusion()) {
    gp_Vec tmpV;
    gp_Pnt tmpP;
    SF.D1(pUV.X()-0.1, pUV.Y(), tmpP, tmpV, Vec2);
  }

  gp_Vec V = Vec1.Crossed(Vec2);
  Standard_Real mod = V.Magnitude();
  if (mod < Precision::Confusion())
    Standard_NullObject::Raise("Normal vector of a face has null magnitude");

  // Set length of normal vector to average radius of curvature
  Standard_Real radius = 0.0;
  GeomLProp_SLProps aProperties (aSurf, pUV.X(), pUV.Y(), 2, Precision::Confusion());
  if (aProperties.IsCurvatureDefined()) {
    Standard_Real radius1 = Abs(aProperties.MinCurvature());
    Standard_Real radius2 = Abs(aProperties.MaxCurvature());
    if (Abs(radius1) > Precision::Confusion()) {
      radius = 1.0 / radius1;
      if (Abs(radius2) > Precision::Confusion()) {
        radius = (radius + 1.0 / radius2) / 2.0;
      }
    } else {
      if (Abs(radius2) > Precision::Confusion()) {
        radius = 1.0 / radius2;
      }
    }
  }

  // Set length of normal vector to average dimension of the face
  // (only if average radius of curvature is not appropriate)
  if (radius < Precision::Confusion()) {
    Bnd_Box B;
    Standard_Real Xmin, Xmax, Ymin, Ymax, Zmin, Zmax;
    BRepBndLib::Add(aFace, B);
    B.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
    radius = ((Xmax - Xmin) + (Ymax - Ymin) + (Zmax - Zmin)) / 3.0;
  }

  if (radius < Precision::Confusion())
    radius = 1.0;

  V *= radius / mod;

  // consider the face orientation
  if (aFace.Orientation() == TopAbs_REVERSED ||
      aFace.Orientation() == TopAbs_INTERNAL) {
    V = - V;
  }

  // Edge
  gp_Pnt p2 = p1.Translated(V);
  BRepBuilderAPI_MakeEdge aBuilder (p1, p2);
  if (!aBuilder.IsDone())
    Standard_NullObject::Raise("Vector construction failed");
  aShape = aBuilder.Shape();

  theNormal->setImpl(new TopoDS_Shape(aShape));

  return true;
}

