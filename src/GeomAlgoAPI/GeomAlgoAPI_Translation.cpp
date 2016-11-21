// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Translation.cpp
// Created:     8 June 2015
// Author:      Dmitry Bobylev
//
// Modified by Clarisse Genrault (CEA) : 17 Nov 2016

#include "GeomAlgoAPI_Translation.h"

#include <BRepBuilderAPI_Transform.hxx>
#include <Precision.hxx>
#include <gp_Ax1.hxx>

//=================================================================================================
GeomAlgoAPI_Translation::GeomAlgoAPI_Translation(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                                 std::shared_ptr<GeomAPI_Ax1>   theAxis,
                                                 double                         theDistance)
{
  myMethodType = BY_DISTANCE;
  mySourceShape = theSourceShape;
  myAxis = theAxis;
  myDistance = theDistance;
}

//=================================================================================================
GeomAlgoAPI_Translation::GeomAlgoAPI_Translation(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                                 double                         theDx,
                                                 double                         theDy,
                                                 double                         theDz)
{
  myMethodType = BY_DIM;
  mySourceShape = theSourceShape;
  myDx = theDx;
  myDy = theDy;
  myDz = theDz;
}

//=================================================================================================
bool GeomAlgoAPI_Translation::check()
{
  switch (myMethodType) {
    case BY_DISTANCE: {
      if (!myAxis) {
        myError = "Translation builder :: axis is invalid.";
        return false;
      }
      // TODO : verification de la distance
      if (!mySourceShape) {
        myError = "Translation builder :: source shape is invalid.";
        return false;
      }
      return true;
    }
    case BY_DIM: {
      if ((fabs(myDx) < Precision::Confusion()) &&
          (fabs(myDy) < Precision::Confusion()) &&
          (fabs(myDz) < Precision::Confusion())) {
        myError = "Translation builder :: Dx, Dy and Dz are null.";
        return false;
      }
      if (!mySourceShape) {
        myError = "Translation builder :: source shape is invalid.";
        return false;
      }
      return true;
    }
    default: {
      myError = "Translation builder :: method not implemented.";
      return false;
    }
  }
}

//=================================================================================================
void GeomAlgoAPI_Translation::build()
{
  gp_Trsf* aTrsf = new gp_Trsf();

  switch (myMethodType) {
    case BY_DISTANCE: {
      const gp_Ax1& anAxis = myAxis->impl<gp_Ax1>();
      aTrsf->SetTranslation(gp_Vec(anAxis.Direction()) * myDistance);
      break;
    }
    case BY_DIM: {
      aTrsf->SetTranslation(gp_Vec(myDx, myDy, myDz));
      break;
    }
    default: {
      myError = "Translation builder :: method not supported";
      return;
    }
  }

  const TopoDS_Shape& aSourceShape = mySourceShape->impl<TopoDS_Shape>();

  if(aSourceShape.IsNull()) {
    myError = "Translation builder :: source shape does not contain any actual shape.";
    return;
  }

  // Transform the shape while copying it.
  BRepBuilderAPI_Transform* aBuilder = new BRepBuilderAPI_Transform(aSourceShape, *aTrsf, true);
  if(!aBuilder) {
    myError = "Translation builder :: source shape does not contain any actual shape.";
    return;
  }

  setImpl(aBuilder);
  setBuilderType(OCCT_BRepBuilderAPI_MakeShape);

  if(!aBuilder->IsDone()) {
    myError = "Translation builder :: source shape does not contain any actual shape.";
    return;
  }

  TopoDS_Shape aResult = aBuilder->Shape();

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  setShape(aShape);
  setDone(true);
}