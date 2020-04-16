// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <BRep_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

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

// Returns true if transformations are equal with the given precision
static bool isEqual(const gp_Trsf& theT1, const gp_Trsf& theT2, const double thePrecision)
{
  for(int aRow = 1; aRow < 4; aRow++) {
    for(int aCol = 1; aCol < 5; aCol++) {
      double aDiff = theT1.Value(aRow, aCol) - theT2.Value(aRow, aCol);
      if (aDiff < 0) aDiff = -aDiff;
      if (aDiff > thePrecision)
        return false;
    }
  }
  return true;
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
    if (anID == 0) { // try to search shape with the same location if TopLoc_Location is different
      TopExp_Explorer anExp(aMainShape, aSubShape.ShapeType());
      for(; anExp.More(); anExp.Next()) {
        if (anExp.Current().TShape() == aSubShape.TShape()) {
          const TopLoc_Location aLoc1 = anExp.Current().Location();
          if (isEqual(aLoc1.Transformation(), aSubShape.Location().Transformation(), 1.e-7)) {
            anID = aSubShapesMap.FindIndex(anExp.Current());
            break;
          }
        }
      }
    }
  }

  return anID;
}
