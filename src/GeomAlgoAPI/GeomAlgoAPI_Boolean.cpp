// File:        GeomAlgoAPI_Boolean.cpp
// Created:     02 Sept 2014
// Author:      Vitaly Smetannikov

#include "GeomAlgoAPI_Boolean.h"

#include <BRepAlgoAPI_Cut.hxx>


boost::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_Boolean::makeCut(
  boost::shared_ptr<GeomAPI_Shape> theShape,
  boost::shared_ptr<GeomAPI_Shape> theTool)
{
  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  const TopoDS_Shape& aTool = theTool->impl<TopoDS_Shape>();

  BRepAlgoAPI_Cut aCut(aShape, aTool);
  if (aCut.IsDone()) {
    boost::shared_ptr<GeomAPI_Shape> aResult(new GeomAPI_Shape());
    aResult->setImpl(new TopoDS_Shape(aCut.Shape()));
    return aResult;
  }
  return boost::shared_ptr<GeomAPI_Shape>();
}
