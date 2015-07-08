// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Placement.cpp
// Created:     2 Dec 2014
// Author:      Artem ZHIDKOV

#include <GeomAlgoAPI_Placement.h>
#include <GeomAlgoAPI_DFLoader.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_XYZ.h>

#include <BRepBuilderAPI_Transform.hxx>
#include <gp_Trsf.hxx>
#include <gp_Quaternion.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <Precision.hxx>

#define DEB_PLACEMENT 1
GeomAlgoAPI_Placement::GeomAlgoAPI_Placement(
  std::shared_ptr<GeomAPI_Shape> theSourceSolid,
  std::shared_ptr<GeomAPI_Shape> theDestSolid,
  std::shared_ptr<GeomAPI_Shape> theSourceShape,
  std::shared_ptr<GeomAPI_Shape> theDestShape,
  bool theIsReverse,
  bool theIsCentering, 
  bool theSimpleTransform)
  : myDone(false),
  myShape(new GeomAPI_Shape())
{
  build(theSourceSolid, theDestSolid, theSourceShape, theDestShape, 
    theIsReverse, theIsCentering, theSimpleTransform);
}

void GeomAlgoAPI_Placement::build(
  const std::shared_ptr<GeomAPI_Shape>& theSourceSolid,
  const std::shared_ptr<GeomAPI_Shape>& theDestSolid,
  const std::shared_ptr<GeomAPI_Shape>& theSourceShape,
  const std::shared_ptr<GeomAPI_Shape>& theDestShape,
  bool theIsReverse,
  bool theIsCentering,
  bool theSimpleTransform)
{
  // Filling the parameters of the objects
  static const int aNbObjects = 2;
  gp_Pnt aSrcDstPoints[aNbObjects]; // points on the selected objects (0 - source, 1 - destination)
  gp_Vec aSrcDstNormals[aNbObjects]; // normal vectors, if planar faces are selected
  gp_Vec aSrcDstDirections[aNbObjects]; // directions of linear edges
  bool hasNormal[aNbObjects];
  bool hasDirection[aNbObjects];
  std::shared_ptr<GeomAPI_Shape> aShapes[aNbObjects] = {theSourceShape, theDestShape};

  for (int i = 0; i < aNbObjects; i++) {
    if (aShapes[i]->isFace()) {
      std::shared_ptr<GeomAPI_Face> aFace(new GeomAPI_Face(aShapes[i]));
      std::shared_ptr<GeomAPI_Pln> aPlane = aFace->getPlane();
      std::shared_ptr<GeomAPI_Dir> aDir = aPlane->direction();
      std::shared_ptr<GeomAPI_Pnt> aLoc = aPlane->location();
      aSrcDstPoints[i].SetCoord(aLoc->x(), aLoc->y(), aLoc->z());
      aSrcDstNormals[i].SetCoord(aDir->x(), aDir->y(), aDir->z());
    } else if (aShapes[i]->isEdge()) {
      std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge(aShapes[i]));
      std::shared_ptr<GeomAPI_Lin> aLine = anEdge->line();
      std::shared_ptr<GeomAPI_Dir> aDir = aLine->direction();
      std::shared_ptr<GeomAPI_Pnt> aFirstPnt = anEdge->firstPoint();
      std::shared_ptr<GeomAPI_Pnt> aLastPnt = anEdge->lastPoint();
      std::shared_ptr<GeomAPI_XYZ> aLoc = aFirstPnt->xyz()->added(aLastPnt->xyz())->multiplied(0.5);
      aSrcDstPoints[i].SetCoord(aLoc->x(), aLoc->y(), aLoc->z());
      aSrcDstDirections[i].SetCoord(aDir->x(), aDir->y(), aDir->z());
    } else if (aShapes[i]->isVertex()) {
      std::shared_ptr<GeomAPI_Vertex> aVertex(new GeomAPI_Vertex(aShapes[i]));
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
        if (aVec.SquareMagnitude() < Precision::SquareConfusion()) { // normal and direction are collinear
          aVec = aSrcDstNormals[1 - anInd].Crossed(
            gp_Vec(aSrcDstPoints[1 - anInd], aSrcDstPoints[anInd]));
          if (aVec.SquareMagnitude() < Precision::SquareConfusion()) { // normal and points direction are collinear
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
    TopoDS_Shape aResult = aSourceShape.Moved(aDelta);
    myShape->setImpl(new TopoDS_Shape(aResult));
  } else { // internal rebuild of the shape
    // Transform the shape with copying it
    BRepBuilderAPI_Transform* aBuilder = new BRepBuilderAPI_Transform(aSourceShape, aTrsf, true);
    if(aBuilder) {
      setImpl(aBuilder);
      myDone = aBuilder->IsDone() == Standard_True;
      if (myDone) {
        TopoDS_Shape aResult = aBuilder->Shape();
        // fill data map to keep correct orientation of sub-shapes 
        for (TopExp_Explorer Exp(aResult,TopAbs_FACE); Exp.More(); Exp.Next()) {
          std::shared_ptr<GeomAPI_Shape> aCurrentShape(new GeomAPI_Shape());
          aCurrentShape->setImpl(new TopoDS_Shape(Exp.Current()));
          myMap.bind(aCurrentShape, aCurrentShape);
        }
  #ifdef DEB_PLACEMENT
        int aNum = myMap.size();
        cout << "MAP of Oriented shapes =" << aNum <<endl;

  #endif

        myShape->setImpl(new TopoDS_Shape(aResult));
        myMkShape = new GeomAlgoAPI_MakeShape (aBuilder);
      }
    }
  }
}

//============================================================================
const bool GeomAlgoAPI_Placement::isValid() const
{
  if (myShape.get()) { // only for not-simple transform
    BRepCheck_Analyzer aChecker(myShape->impl<TopoDS_Shape>());
    return (aChecker.IsValid() == Standard_True);
  }
  return true;
}

//============================================================================
const bool GeomAlgoAPI_Placement::hasVolume() const
{
  bool hasVolume(false);
  if(isValid()) {
    const TopoDS_Shape& aRShape = myShape->impl<TopoDS_Shape>();
    GProp_GProps aGProp;
    BRepGProp::VolumeProperties(aRShape, aGProp);
    if(aGProp.Mass() > Precision::Confusion()) 
      hasVolume = true;	
  }
  return hasVolume;
}

//============================================================================
const std::shared_ptr<GeomAPI_Shape>& GeomAlgoAPI_Placement::shape () const 
{
  return myShape;
}

//============================================================================
void GeomAlgoAPI_Placement::mapOfShapes (GeomAPI_DataMapOfShapeShape& theMap) const
{
  theMap = myMap;
}

//============================================================================
GeomAlgoAPI_MakeShape * GeomAlgoAPI_Placement::makeShape() const
{
  return myMkShape;
}

//============================================================================
GeomAlgoAPI_Placement::~GeomAlgoAPI_Placement()
{
  if (!empty())
    myMap.clear();
}
