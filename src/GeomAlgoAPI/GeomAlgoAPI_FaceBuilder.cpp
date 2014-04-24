// File:        GeomAlgoAPI_FaceBuilder.cpp
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#include <GeomAlgoAPI_FaceBuilder.h>
#include <gp_Pln.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <TopoDS_Face.hxx>

boost::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_FaceBuilder::square(
  boost::shared_ptr<GeomAPI_Pnt> theCenter, boost::shared_ptr<GeomAPI_Dir> theNormal,
  const double theSize)
{
  const gp_Pnt& aCenter = theCenter->impl<gp_Pnt>();
  const gp_Dir& aDir = theNormal->impl<gp_Dir>();
  gp_Pln aPlane(aCenter, aDir);
  // half of the size in each direction from the center
  BRepBuilderAPI_MakeFace aFaceBuilder(aPlane, 
    -theSize / 2., theSize / 2., -theSize / 2., theSize / 2.);
  boost::shared_ptr<GeomAPI_Shape> aRes(new GeomAPI_Shape);
  aRes->setImpl(new TopoDS_Shape(aFaceBuilder.Face()));
  return aRes;
}
