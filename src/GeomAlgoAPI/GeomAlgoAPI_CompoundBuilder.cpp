// File:        GeomAlgoAPI_CompoundBuilder.cpp
// Created:     24 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <GeomAlgoAPI_CompoundBuilder.h>
//#include <gp_Pln.hxx>
//#include <BRepBuilderAPI_MakeFace.hxx>
//#include <TopoDS_Face.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Compound.hxx>

boost::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_CompoundBuilder::compound(
    std::list<boost::shared_ptr<GeomAPI_Shape> > theShapes)
{
  BRep_Builder aBuilder;
  TopoDS_Compound aComp;
  aBuilder.MakeCompound(aComp);

  std::list<boost::shared_ptr<GeomAPI_Shape> >::const_iterator anIt = theShapes.begin(), aLast =
      theShapes.end();
  for (; anIt != aLast; anIt++) {
    aBuilder.Add(aComp, (*anIt)->impl<TopoDS_Shape>());
  }

  boost::shared_ptr<GeomAPI_Shape> aRes(new GeomAPI_Shape);
  aRes->setImpl(new TopoDS_Shape(aComp));
  return aRes;
}
