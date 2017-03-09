// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Rotation.cpp
// Created:     12 May 2015
// Author:      Dmitry Bobylev

#include "GeomAlgoAPI_Rotation.h"

#include <GeomAPI_XYZ.h>

#include <BRepBuilderAPI_Transform.hxx>

#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>

#include <Precision.hxx>

//=================================================================================================
GeomAlgoAPI_Rotation::GeomAlgoAPI_Rotation(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                           std::shared_ptr<GeomAPI_Ax1>   theAxis,
                                           double                         theAngle)
{
  myMethodType = BY_ANGLE;
  mySourceShape = theSourceShape;
  myAxis = theAxis;
  myAngle = theAngle;
}


//=================================================================================================
GeomAlgoAPI_Rotation::GeomAlgoAPI_Rotation(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                           std::shared_ptr<GeomAPI_Pnt>   theCenterPoint,
                                           std::shared_ptr<GeomAPI_Pnt>   theStartPoint,
                                           std::shared_ptr<GeomAPI_Pnt>   theEndPoint)
{
  myMethodType = BY_POINTS;
  mySourceShape = theSourceShape;
  myCenterPoint = theCenterPoint;
  myStartPoint = theStartPoint;
  myEndPoint = theEndPoint;
}

//=================================================================================================
bool GeomAlgoAPI_Rotation::check()
{
  switch (myMethodType) {
    case BY_ANGLE: {
      if (!myAxis) {
        myError = "Rotation builder :: axis is not valid.";
        return false;
      }
      if (!mySourceShape) {
        myError = "Rotation builder :: source shape is not valid.";
        return false;
      }
      if (myAngle < -360.) {
        myError = "Rotation builder :: angle smaller than -360 degrees.";
        return false;
      }
      if (myAngle > 360.) {
        myError = "Rotation builder :: angle greater than 360 degrees.";
        return false;
      }
      return true;
    }
    case BY_POINTS: {
      if (!myCenterPoint) {
        myError = "Rotation builder :: start point is not valid.";
        return false;
      }
      if (!myStartPoint) {
        myError = "Rotation builder :: start point is not valid.";
        return false;
      }
      if (!myEndPoint) {
        myError = "Rotation builder :: start point is not valid.";
        return false;
      }
      if (!mySourceShape) {
        myError = "Rotation builder :: source shape is not valid.";
        return false;
      }
      if(myCenterPoint->distance(myStartPoint) < Precision::Confusion()) {
        myError = "Rotation builder :: center point and start point coincide.";
        return false;
      }
      if(myCenterPoint->distance(myEndPoint) < Precision::Confusion()) {
        myError = "Rotation builder :: center point and end point coincide.";
        return false;
      }
      if(myStartPoint->distance(myEndPoint) < Precision::Confusion()) {
        myError = "Rotation builder :: start point and end point coincide.";
        return false;
      }
      std::shared_ptr<GeomAPI_XYZ> aCenterPointXYZ = myCenterPoint->xyz();
      std::shared_ptr<GeomAPI_XYZ> aStartPointXYZ = myStartPoint->xyz();
      std::shared_ptr<GeomAPI_XYZ> aEndPointXYZ = myEndPoint->xyz();
      std::shared_ptr<GeomAPI_XYZ> vectCenterPointStartPoint =
        aStartPointXYZ->decreased(aCenterPointXYZ);
      std::shared_ptr<GeomAPI_XYZ> vectCenterPointEndPoint =
        aEndPointXYZ->decreased(aCenterPointXYZ);
      std::shared_ptr<GeomAPI_XYZ> crossProduct =
        vectCenterPointStartPoint->cross(vectCenterPointEndPoint);
      std::shared_ptr<GeomAPI_Pnt> aOriginPnt =
        std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(0.,0.,0.));
      std::shared_ptr<GeomAPI_XYZ> aOriginXYZ = aOriginPnt->xyz();

      if (crossProduct->distance(aOriginXYZ) < Precision::Confusion()) {
        myError = "Rotation builder :: center point, start point and end point are on a line.";
        return false;
      }
      return true;
    }
    default: {
      myError = "Rotation builder :: method not implemented.";
      return false;
    }
  }
}

//=================================================================================================
void GeomAlgoAPI_Rotation::build()
{
  gp_Trsf* aTrsf = new gp_Trsf();

  switch (myMethodType) {
    case BY_ANGLE: {
      const gp_Ax1& anAxis = myAxis->impl<gp_Ax1>();
      aTrsf->SetRotation(anAxis, myAngle/180.0*M_PI);
      break;
    }
    case BY_POINTS: {
      const gp_Pnt& aCenterPoint = myCenterPoint->impl<gp_Pnt>();
      const gp_Pnt& aStartPoint = myStartPoint->impl<gp_Pnt>();
      const gp_Pnt& aEndPoint = myEndPoint->impl<gp_Pnt>();
      gp_Vec aVec1(aCenterPoint, aStartPoint);
      gp_Vec aVec2(aCenterPoint, aEndPoint);
      gp_Dir aDir(aVec1^aVec2);
      gp_Ax1 anAxis(aCenterPoint, aDir);
      double anAngle = aVec1.Angle(aVec2);
      if (fabs(anAngle) < Precision::Angular()) anAngle += 2.*M_PI;
      aTrsf->SetRotation(anAxis, anAngle);
      break;
    }
    default: {
      myError = "Rotation builder :: method not supported";
      return;
    }
  }

  const TopoDS_Shape& aSourceShape = mySourceShape->impl<TopoDS_Shape>();

  if(aSourceShape.IsNull()) {
    myError = "Rotation builder :: source shape does not contain any actual shape.";
    return;
  }

  // Transform the shape while copying it.
  BRepBuilderAPI_Transform* aBuilder = new BRepBuilderAPI_Transform(aSourceShape, *aTrsf, true);
  if(!aBuilder) {
    myError = "Rotation builder :: transform initialization failed.";
    return;
  }

  setImpl(aBuilder);
  setBuilderType(OCCT_BRepBuilderAPI_MakeShape);

  if(!aBuilder->IsDone()) {
    myError = "Rotation builder :: algorithm failed.";
    return;
  }

  TopoDS_Shape aResult = aBuilder->Shape();

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  setShape(aShape);
  setDone(true);
}
