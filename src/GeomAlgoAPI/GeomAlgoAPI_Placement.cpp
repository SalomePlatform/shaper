// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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

#include "GeomAlgoAPI_Placement.h"

#include <GeomAlgoAPI_DFLoader.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <GeomAPI_Dir.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Face.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_ShapeIterator.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_XYZ.h>

#include <BRepBuilderAPI_Transform.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepGProp.hxx>
#include <gp_Trsf.hxx>
#include <gp_Quaternion.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>

GeomAlgoAPI_Placement::GeomAlgoAPI_Placement(const std::shared_ptr<GeomAPI_Shape> theSourceSolid,
                                             const std::shared_ptr<GeomAPI_Shape> theDestSolid,
                                             const std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                             const std::shared_ptr<GeomAPI_Shape> theDestShape,
                                             const bool theIsReverse,
                                             const bool theIsCentering,
                                             const bool theSimpleTransform)
{
  build(theSourceSolid, theDestSolid, theSourceShape, theDestShape,
    theIsReverse, theIsCentering, theSimpleTransform);
}

void GeomAlgoAPI_Placement::build(const std::shared_ptr<GeomAPI_Shape>& theSourceSolid,
                                  const std::shared_ptr<GeomAPI_Shape>& theDestSolid,
                                  const std::shared_ptr<GeomAPI_Shape>& theSourceShape,
                                  const std::shared_ptr<GeomAPI_Shape>& theDestShape,
                                  const bool theIsReverse,
                                  const bool theIsCentering,
                                  const bool theSimpleTransform)
{
  // Filling the parameters of the objects
  static const int aNbObjects = 2;
  gp_Pnt aSrcDstPoints[aNbObjects]; // points on the selected objects (0 - source, 1 - destination)
  gp_Vec aSrcDstNormals[aNbObjects]; // normal vectors, if planar faces are selected
  gp_Vec aSrcDstDirections[aNbObjects]; // directions of linear edges
  bool hasNormal[aNbObjects];
  bool hasDirection[aNbObjects];
  std::shared_ptr<GeomAPI_Shape> aShapes[aNbObjects] = {theSourceShape, theDestShape};

  GProp_GProps aProps;
  static const double aPropEps = 1.e-4;
  GeomShapePtr aShape;
  bool isCompound = false;
  for (int i = 0; i < aNbObjects; i++) {
    aShape = aShapes[i];
    isCompound = false;
    if (aShapes[i]->isCompound()) {
      isCompound = true;
      GeomAPI_ShapeIterator anIt(aShapes[i]);
      aShape = anIt.current();

      GeomPointPtr aPnt = GeomAlgoAPI_ShapeTools::centreOfMass(aShapes[i]);
      aSrcDstPoints[i].SetCoord(aPnt->x(), aPnt->y(), aPnt->z());
    }

    if (aShape->isFace()) {
      std::shared_ptr<GeomAPI_Face> aFace(new GeomAPI_Face(aShape));
      std::shared_ptr<GeomAPI_Pln> aPlane = aFace->getPlane();
      std::shared_ptr<GeomAPI_Dir> aDir = aPlane->direction();
      aSrcDstNormals[i].SetCoord(aDir->x(), aDir->y(), aDir->z());

      if (!isCompound) {
        BRepGProp::SurfaceProperties(aFace->impl<TopoDS_Face>(), aProps, aPropEps);
        gp_Pnt aLoc = aProps.CentreOfMass();
        aSrcDstPoints[i].SetCoord(aLoc.X(), aLoc.Y(), aLoc.Z());
      }
    }
    else if (aShape->isEdge()) {
      std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge(aShape));
      std::shared_ptr<GeomAPI_Lin> aLine = anEdge->line();
      std::shared_ptr<GeomAPI_Dir> aDir = aLine->direction();
      aSrcDstDirections[i].SetCoord(aDir->x(), aDir->y(), aDir->z());

      if (!isCompound) {
        std::shared_ptr<GeomAPI_Pnt> aFirstPnt = anEdge->firstPoint();
        std::shared_ptr<GeomAPI_Pnt> aLastPnt = anEdge->lastPoint();
        std::shared_ptr<GeomAPI_XYZ> aLoc = aFirstPnt->xyz()->added(aLastPnt->xyz())
                                                            ->multiplied(0.5);
        aSrcDstPoints[i].SetCoord(aLoc->x(), aLoc->y(), aLoc->z());
      }
    }
    else if (aShape->isVertex()) {
      std::shared_ptr<GeomAPI_Vertex> aVertex(new GeomAPI_Vertex(aShape));
      std::shared_ptr<GeomAPI_Pnt> aPnt = aVertex->point();
      aSrcDstPoints[i].SetCoord(aPnt->x(), aPnt->y(), aPnt->z());
    } else // something goes wrong
      return;
    hasNormal[i] = aSrcDstNormals[i].SquareMagnitude() >= Precision::SquareConfusion();
    hasDirection[i] = aSrcDstDirections[i].SquareMagnitude() >= Precision::SquareConfusion();
  }

  // Initial shapes
  const TopoDS_Shape& aSourceShape = theSourceSolid->impl<TopoDS_Shape>();
  const TopoDS_Shape& aDestShape = theDestSolid->impl<TopoDS_Shape>();
  // Check the material of the solids to be on the correct side
  BRepClass3d_SolidClassifier aClassifier;
  static const double aTransStep = 10. * Precision::Confusion();
  if (hasNormal[0]) {
    aClassifier.Load(aSourceShape);
    gp_Pnt aPoint = aSrcDstPoints[0];
    aPoint.Translate(aSrcDstNormals[0] * aTransStep);
    aClassifier.Perform(aPoint, Precision::Confusion());
    if ((aClassifier.State() == TopAbs_OUT && !theIsReverse) ||
      (aClassifier.State() == TopAbs_IN && theIsReverse))
      aSrcDstNormals[0].Reverse();
  }
  if (hasNormal[1]) {
    aClassifier.Load(aDestShape);
    gp_Pnt aPoint = aSrcDstPoints[1];
    aPoint.Translate(aSrcDstNormals[1] * aTransStep);
    aClassifier.Perform(aPoint, Precision::Confusion());
    if (aClassifier.State() == TopAbs_IN)
      aSrcDstNormals[1].Reverse();
  }

  // Calculate directions, which comply the normal, for vertices and edges
  if (!hasNormal[0] || !hasNormal[1]) {
    if (hasNormal[0] || hasNormal[1]) { // plane with line or vertex
      if (hasDirection[0] || hasDirection[1]) { // plane - line
        int anInd = hasDirection[0] ? 0 : 1;
        gp_Vec aVec = aSrcDstNormals[1 - anInd].Crossed(aSrcDstDirections[anInd]);
        if (aVec.SquareMagnitude() < Precision::SquareConfusion()) {
          // normal and direction are collinear
          aVec = aSrcDstNormals[1 - anInd].Crossed(
            gp_Vec(aSrcDstPoints[1 - anInd], aSrcDstPoints[anInd]));
          if (aVec.SquareMagnitude() < Precision::SquareConfusion()) {
            // normal and points direction are collinear
            if (Abs(aSrcDstNormals[1 - anInd].Y()) >= Precision::Confusion() ||
              Abs(aSrcDstNormals[1 - anInd].Z()) >= Precision::Confusion())
              aVec = gp::DX();
            else
              aVec = gp::DY();
          }
        }
        aSrcDstNormals[anInd] = aSrcDstDirections[anInd].Crossed(aVec).Normalized();
      } else { // plane - point
        int anInd = hasNormal[0] ? 1 : 0;
        aSrcDstNormals[anInd] = aSrcDstNormals[1 - anInd];
      }
    } else {
      if (hasDirection[0] && hasDirection[1]) { // line - line
        gp_Vec aVec = aSrcDstDirections[0].Crossed(aSrcDstDirections[1]);
        if (aVec.SquareMagnitude() < Precision::SquareConfusion()) { // lines are parallel
          aVec = aSrcDstDirections[0].Crossed(gp_Vec(aSrcDstPoints[0], aSrcDstPoints[1]));
          if (aVec.SquareMagnitude() < Precision::SquareConfusion()) { // lines are equal
            if (Abs(aSrcDstDirections[0].Y()) >= Precision::Confusion() ||
              Abs(aSrcDstDirections[0].Z()) >= Precision::Confusion())
              aVec = gp::DX();
            else
              aVec = gp::DY();
          }
        }
        aSrcDstNormals[0] = aSrcDstDirections[0].Crossed(aVec);
        aSrcDstNormals[0].Normalize();
        aSrcDstNormals[1] = aSrcDstDirections[1].Crossed(aVec);
        aSrcDstNormals[1].Normalize();
        if (aSrcDstDirections[0].Dot(aSrcDstDirections[1]) < -Precision::Confusion())
          aSrcDstNormals[1].Reverse();
      } else if (!hasDirection[0] && !hasDirection[1]) { // point - point
        aSrcDstNormals[0] = gp_Vec(aSrcDstPoints[0], aSrcDstPoints[1]);
        aSrcDstNormals[0].Normalize();
        aSrcDstNormals[1] = -aSrcDstNormals[0];
      } else { // line - point
        int anInd = hasDirection[0] ? 0 : 1;
        gp_Vec aVec(aSrcDstPoints[anInd], aSrcDstPoints[1 - anInd]);
        aVec.Cross(aSrcDstDirections[anInd]);
        if (aVec.SquareMagnitude() < Precision::SquareConfusion()) { // point is on line
          if (Abs(aSrcDstDirections[1 - anInd].Y()) >= Precision::Confusion() ||
            Abs(aSrcDstDirections[1 - anInd].Z()) >= Precision::Confusion())
            aVec = gp::DX();
          else
            aVec = gp::DY();
        }
        aSrcDstNormals[anInd] = aSrcDstDirections[anInd].Crossed(aVec).Normalized();
        aSrcDstNormals[1 - anInd] = aSrcDstNormals[anInd];
      }
    }
  }

  // Reverse the normal if it was not done before
  if (!hasNormal[0] && theIsReverse)
    aSrcDstNormals[0].Reverse();

  // Calculate transformation
  gp_Trsf aTrsf;
  gp_Vec aSrcDir = aSrcDstNormals[0];
  gp_Vec aDstDir = aSrcDstNormals[1];
  // Calculate rotation
  gp_Quaternion aRot(aSrcDir, aDstDir);
  aTrsf.SetRotation(aRot);
  // Calculate translation
  gp_Vec aSrcLoc(aSrcDstPoints[0].XYZ());
  gp_Vec aDstLoc(aSrcDstPoints[1].XYZ());
  if (!theIsCentering)
    aDstLoc = aSrcLoc + gp_Vec(aDstDir) * (aDstLoc-aSrcLoc).Dot(aDstDir);
  aSrcLoc.Transform(aTrsf);
  gp_Vec aTrans = aDstLoc - aSrcLoc;
  aTrsf.SetTransformation(aRot, aTrans);

  if (theSimpleTransform) { // just add transformation
    TopLoc_Location aDelta(aTrsf);
    // store the accumulated information about the result and this delta
    myTrsf.reset(new GeomAPI_Trsf(new gp_Trsf(aTrsf)));
    TopoDS_Shape aResult = aSourceShape.Moved(aDelta);
    std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
    aShape->setImpl(new TopoDS_Shape(aResult));
    this->setShape(aShape);
    this->setDone(true); // it is allways true for simple transformation generation
  } else { // internal rebuild of the shape
    // Transform the shape with copying it
    BRepBuilderAPI_Transform* aBuilder = new BRepBuilderAPI_Transform(aSourceShape, aTrsf, true);
    if(!aBuilder) {
      return;
    }
    this->setImpl(aBuilder);
    this->setBuilderType(OCCT_BRepBuilderAPI_MakeShape);
    if(aBuilder->IsDone() != Standard_True) {
      return;
    }
    TopoDS_Shape aResult = aBuilder->Shape();

    std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
    aShape->setImpl(new TopoDS_Shape(aResult));
    this->setShape(aShape);
    this->setDone(true);
  }
}

//=================================================================================================
std::shared_ptr<GeomAPI_Trsf> GeomAlgoAPI_Placement::transformation() const
{
  return myTrsf;
}
