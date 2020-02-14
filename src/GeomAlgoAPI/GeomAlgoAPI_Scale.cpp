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

#include <GeomAlgoAPI_Scale.h>

#include <GeomAPI_Pnt.h>

#include <BRepBuilderAPI_GTransform.hxx>

//=================================================================================================
GeomAlgoAPI_Scale::GeomAlgoAPI_Scale(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                     std::shared_ptr<GeomAPI_Pnt>   theCenterPoint,
                                     double theScaleFactor)
{
  if (fabs(theScaleFactor) < Precision::Confusion()) {
    myError = "Scale builder :: the scale factor is null.";
    return;
  }

  GeomTrsfPtr aTrsf(new GeomAPI_Trsf);
  aTrsf->setScale(theCenterPoint, theScaleFactor);

  build(theSourceShape, aTrsf);
}

//=================================================================================================
GeomAlgoAPI_Scale::GeomAlgoAPI_Scale(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                     std::shared_ptr<GeomAPI_Pnt>   theCenterPoint,
                                     double theScaleFactorX,
                                     double theScaleFactorY,
                                     double theScaleFactorZ)
{
  if (fabs(theScaleFactorX) < Precision::Confusion()) {
    myError = "Scale builder :: the scale factor in X is null.";
    return;
  }
  if (fabs(theScaleFactorY) < Precision::Confusion()) {
    myError = "Scale builder :: the scale factor in Y is null.";
    return;
  }
  if (fabs(theScaleFactorZ) < Precision::Confusion()) {
    myError = "Scale builder :: the scale factor in Z is null.";
    return;
  }

  buildByDimensions(theSourceShape, theCenterPoint,
                    theScaleFactorX, theScaleFactorY, theScaleFactorZ);
}

//=================================================================================================
void GeomAlgoAPI_Scale::buildByDimensions(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                          std::shared_ptr<GeomAPI_Pnt>   theCenterPoint,
                                          double                         theScaleFactorX,
                                          double                         theScaleFactorY,
                                          double                         theScaleFactorZ)
{
  const gp_Pnt& aCenterPoint = theCenterPoint->impl<gp_Pnt>();

  // Perform the rotation matrix
  gp_Mat aMatRot(theScaleFactorX, 0., 0.,
                 0., theScaleFactorY, 0.,
                 0., 0., theScaleFactorZ);

  // Perform the tranformation
  gp_GTrsf aGTrsf;
  gp_GTrsf aGTrsfP0;
  gp_GTrsf aGTrsf0P;
  aGTrsfP0.SetTranslationPart(gp::Origin().XYZ() - aCenterPoint.XYZ());
  aGTrsf0P.SetTranslationPart(aCenterPoint.XYZ());
  aGTrsf.SetVectorialPart(aMatRot);
  aGTrsf = aGTrsf0P.Multiplied(aGTrsf);
  aGTrsf = aGTrsf.Multiplied(aGTrsfP0);

  const TopoDS_Shape& aSourceShape = theSourceShape->impl<TopoDS_Shape>();

  if(aSourceShape.IsNull()) {
    myError = "Scale builder :: source shape does not contain any actual shape.";
    return;
  }

  // Transform the shape while copying it.
  BRepBuilderAPI_GTransform* aBuilder = new BRepBuilderAPI_GTransform(aSourceShape, aGTrsf, true);
  if(!aBuilder) {
    myError = "Scale builder :: transform initialization failed.";
    return;
  }

  setImpl(aBuilder);
  setBuilderType(OCCT_BRepBuilderAPI_MakeShape);

  if(!aBuilder->IsDone()) {
    myError = "Scale builder :: algorithm failed.";
    return;
  }

  TopoDS_Shape aResult = aBuilder->Shape();

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  setShape(aShape);
  setDone(true);
}
