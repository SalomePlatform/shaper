// File:        GeomAlgoAPI_Boolean.cpp
// Created:     02 Sept 2014
// Author:      Vitaly Smetannikov

#include "GeomAlgoAPI_Boolean.h"

#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Fuse.hxx>


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


boost::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_Boolean::makeFuse(
  boost::shared_ptr<GeomAPI_Shape> theShape,
  boost::shared_ptr<GeomAPI_Shape> theTool)
{
  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  const TopoDS_Shape& aTool = theTool->impl<TopoDS_Shape>();

  BRepAlgoAPI_Fuse aFuse(aShape, aTool);
  if (aFuse.IsDone()) {
    boost::shared_ptr<GeomAPI_Shape> aResult(new GeomAPI_Shape());
    aResult->setImpl(new TopoDS_Shape(aFuse.Shape()));
    return aResult;
  }
  return boost::shared_ptr<GeomAPI_Shape>();
}


boost::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_Boolean::makeCommon(
  boost::shared_ptr<GeomAPI_Shape> theShape,
  boost::shared_ptr<GeomAPI_Shape> theTool)
{
  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  const TopoDS_Shape& aTool = theTool->impl<TopoDS_Shape>();

  BRepAlgoAPI_Common aCommon(aShape, aTool);
  if (aCommon.IsDone()) {
    boost::shared_ptr<GeomAPI_Shape> aResult(new GeomAPI_Shape());
    aResult->setImpl(new TopoDS_Shape(aCommon.Shape()));
    return aResult;
  }
  return boost::shared_ptr<GeomAPI_Shape>();
}