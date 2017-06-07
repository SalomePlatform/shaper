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

#include "GeomAlgoAPI_Symmetry.h"

#include <BRepBuilderAPI_Transform.hxx>

//=================================================================================================
GeomAlgoAPI_Symmetry::GeomAlgoAPI_Symmetry(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                           std::shared_ptr<GeomAPI_Pnt>   thePoint)
{
  myMethodType = BY_POINT;
  mySourceShape = theSourceShape;
  myPoint = thePoint;
}

//=================================================================================================
GeomAlgoAPI_Symmetry::GeomAlgoAPI_Symmetry(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                           std::shared_ptr<GeomAPI_Ax1>   theAxis)
{
  myMethodType = BY_AXIS;
  mySourceShape = theSourceShape;
  myAxis = theAxis;
}

//=================================================================================================
GeomAlgoAPI_Symmetry::GeomAlgoAPI_Symmetry(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                           std::shared_ptr<GeomAPI_Ax2>   thePlane)
{
  myMethodType = BY_PLANE;
  mySourceShape = theSourceShape;
  myPlane = thePlane;
}

//=================================================================================================
bool GeomAlgoAPI_Symmetry::check()
{
  switch (myMethodType) {
    case BY_POINT: {
      if (!myPoint) {
        myError = "Symmetry builder :: point is not valid.";
        return false;
      }
      if (!mySourceShape) {
        myError = "Symmetry builder :: source shape is not valid.";
        return false;
      }
      return true;
    }
    case BY_AXIS: {
      if (!myAxis) {
        myError = "Symmetry builder :: axis is not valid.";
        return false;
      }
      if (!mySourceShape) {
        myError = "Symmetry builder :: source shape is not valid.";
        return false;
      }
      return true;
    }
    case BY_PLANE: {
      if (!myPlane) {
        myError = "Symmetry builder :: plane is not valid.";
        return false;
      }
      if (!mySourceShape) {
        myError = "Symmetry builder :: source shape is not valid.";
        return false;
      }
      return true;
    }
    default: {
      myError = "Symmetry builder :: method not implemented.";
      return false;
    }
  }
}

//=================================================================================================
void GeomAlgoAPI_Symmetry::build()
{
  gp_Trsf* aTrsf = new gp_Trsf();

  switch (myMethodType) {
    case BY_POINT: {
      const gp_Pnt& aPoint = myPoint->impl<gp_Pnt>();
      aTrsf->SetMirror(aPoint);
      break;
    }
    case BY_AXIS: {
      const gp_Ax1& anAxis = myAxis->impl<gp_Ax1>();
      aTrsf->SetMirror(anAxis);
      break;
    }
    case BY_PLANE: {
      const gp_Ax2& aPlane = myPlane->impl<gp_Ax2>();
      aTrsf->SetMirror(aPlane);
      break;
    }
    default: {
      myError = "Symmetry builder :: method not supported";
      return;
    }
  }

  const TopoDS_Shape& aSourceShape = mySourceShape->impl<TopoDS_Shape>();

  if(aSourceShape.IsNull()) {
    myError = "Symmetry builder :: source shape does not contain any actual shape.";
    return;
  }

  // Transform the shape while copying it.
  BRepBuilderAPI_Transform* aBuilder = new BRepBuilderAPI_Transform(aSourceShape, *aTrsf, true);
  if(!aBuilder) {
    myError = "Symmetry builder :: transform initialization failed.";
    return;
  }

  setImpl(aBuilder);
  setBuilderType(OCCT_BRepBuilderAPI_MakeShape);

  if(!aBuilder->IsDone()) {
    myError = "Symmetry builder :: algorithm failed.";
    return;
  }

  TopoDS_Shape aResult = aBuilder->Shape();

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  setShape(aShape);
  setDone(true);
}
