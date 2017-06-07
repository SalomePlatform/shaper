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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

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
GeomAlgoAPI_Translation::GeomAlgoAPI_Translation(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                                 std::shared_ptr<GeomAPI_Pnt>   theStartPoint,
                                                 std::shared_ptr<GeomAPI_Pnt>   theEndPoint)
{
  myMethodType = BY_POINTS;
  mySourceShape = theSourceShape;
  myStartPoint = theStartPoint;
  myEndPoint = theEndPoint;
}

//=================================================================================================
bool GeomAlgoAPI_Translation::check()
{
  switch (myMethodType) {
    case BY_DISTANCE: {
      if (!myAxis) {
        myError = "Translation builder :: axis is not valid.";
        return false;
      }
      if (!mySourceShape) {
        myError = "Translation builder :: source shape is not valid.";
        return false;
      }
      return true;
    }
    case BY_DIM: {
      if (!mySourceShape) {
        myError = "Translation builder :: source shape is not valid.";
        return false;
      }
      return true;
    }
    case BY_POINTS: {
      if (!myStartPoint) {
        myError = "Translation builder :: start point is not valid.";
        return false;
      }
      if (!myEndPoint) {
        myError = "Translation builder :: end point is not valid.";
        return false;
      }
      if (!mySourceShape) {
        myError = "Translation builder :: source shape is invalid.";
        return false;
      }
      if(myStartPoint->distance(myEndPoint) < Precision::Confusion()) {
        myError = "Translation builder :: start point and end point coincide.";
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
    case BY_POINTS: {
      const gp_Pnt& aStartPoint = myStartPoint->impl<gp_Pnt>();
      const gp_Pnt& aEndPoint = myEndPoint->impl<gp_Pnt>();
      aTrsf->SetTranslation(aStartPoint, aEndPoint);
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
    myError = "Translation builder :: transform initialization failed.";
    return;
  }

  setImpl(aBuilder);
  setBuilderType(OCCT_BRepBuilderAPI_MakeShape);

  if(!aBuilder->IsDone()) {
    myError = "Translation builder :: algorithm failed.";
    return;
  }

  TopoDS_Shape aResult = aBuilder->Shape();

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  setShape(aShape);
  setDone(true);
}
