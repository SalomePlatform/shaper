// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include "GeomAlgoAPI_ThroughAll.h"

#include <GeomAlgoAPI_Boolean.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <BRep_Builder.hxx>
#include <BOPAlgo_BOP.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopExp_Explorer.hxx>

//=================================================================================================
GeomAlgoAPI_ThroughAll::GeomAlgoAPI_ThroughAll(std::shared_ptr<GeomAlgoAPI_Prism> thePrismAlgo,
                                               const ListOfShape& theObjects)
: GeomAlgoAPI_Boolean(thePrismAlgo->shape(), theObjects, GeomAlgoAPI_Tools::BOOL_CUT)
{
  removeEnds(thePrismAlgo);
}

//=================================================================================================
void GeomAlgoAPI_ThroughAll::removeEnds(std::shared_ptr<GeomAlgoAPI_Prism> thePrismAlgo)
{
  GeomShapePtr aCuttedTool = shape(); // result of BOP Cut (thePrismAlgo->shape() by theObjects)

  // Simplify the result
  ListOfShape aPieces = GeomAlgoAPI_ShapeTools::getLowLevelSubShapes(aCuttedTool);

  // Get end shapes of Prism
  const ListOfShape& fromShapes = thePrismAlgo->fromShapes();
  const ListOfShape& toShapes = thePrismAlgo->toShapes();
  ListOfShape endShapes (fromShapes);
  endShapes.insert(endShapes.end(), toShapes.begin(), toShapes.end());

  // Throw away end pieces of cutted tools (containing endShapes)
  TopTools_ListOfShape listTools;
  for (ListOfShape::const_iterator
         anIt = aPieces.begin(); anIt != aPieces.end(); anIt++) {
    TopoDS_Shape aPiece = (*anIt)->impl<TopoDS_Shape>();
    bool endPiece = false;

    for (ListOfShape::const_iterator aBaseIt = endShapes.begin();
         aBaseIt != endShapes.end() && !endPiece; aBaseIt++) {
      // Check, if the piece contains aBase (one of endShapes)
      TopoDS_Shape aBase = (*aBaseIt)->impl<TopoDS_Shape>();
      TopExp_Explorer anExp (aPiece, aBase.ShapeType());
      for (; anExp.More() && !endPiece; anExp.Next()) {
        if (anExp.Current().IsSame(aBase))
          endPiece = true;
      }
    }

    if (!endPiece) {
      listTools.Append(aPiece);
    }
  }

  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  for (TopTools_ListOfShape::Iterator anIt(listTools); anIt.More(); anIt.Next()) {
    aBuilder.Add(aCompound, anIt.Value());
  }

  std::shared_ptr<GeomAPI_Shape> aShape (new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aCompound));
  this->setShape(aShape);
  this->setDone(true);
}
