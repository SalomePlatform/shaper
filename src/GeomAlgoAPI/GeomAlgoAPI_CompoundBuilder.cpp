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
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <BRep_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>

std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_CompoundBuilder::compound(
    std::list<std::shared_ptr<GeomAPI_Shape> > theShapes)
{
  BRep_Builder aBuilder;
  TopoDS_Compound aComp;
  aBuilder.MakeCompound(aComp);

  std::list<std::shared_ptr<GeomAPI_Shape> >::const_iterator anIt = theShapes.begin(), aLast =
      theShapes.end();
  for (; anIt != aLast; anIt++) {
    aBuilder.Add(aComp, (*anIt)->impl<TopoDS_Shape>());
  }

  std::shared_ptr<GeomAPI_Shape> aRes(new GeomAPI_Shape);
  aRes->setImpl(new TopoDS_Shape(aComp));
  return aRes;
}

int GeomAlgoAPI_CompoundBuilder::id(
      std::shared_ptr<GeomAPI_Shape> theContext, std::shared_ptr<GeomAPI_Shape> theSub)
{
  int anID = 0;
  TopoDS_Shape aMainShape = theContext->impl<TopoDS_Shape>();
  const TopoDS_Shape& aSubShape = theSub->impl<TopoDS_Shape>();
  if (!aMainShape.IsNull() && !aSubShape.IsNull()) {
    TopTools_IndexedMapOfShape aSubShapesMap;
    TopExp::MapShapes(aMainShape, aSubShapesMap);
    anID = aSubShapesMap.FindIndex(aSubShape);
  }

  return anID;
}
