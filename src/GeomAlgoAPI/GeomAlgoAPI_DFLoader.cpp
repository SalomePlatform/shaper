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

#include <GeomAlgoAPI_DFLoader.h>
#include <TopoDS_Iterator.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
//=======================================================================
//function : refineResult
//purpose  :
//=======================================================================
const TopoDS_Shape GeomAlgoAPI_DFLoader::refineResult(const TopoDS_Shape& theResult)
{
  TopoDS_Shape aResult = theResult;
  const TopAbs_ShapeEnum aShType = theResult.ShapeType();
  if(aShType == TopAbs_COMPOUND || aShType == TopAbs_SHELL || aShType == TopAbs_WIRE) {
    Standard_Integer aSubResultsNb = 0;
    TopoDS_Iterator anIt(theResult);
    for(; anIt.More(); anIt.Next()) {
      ++aSubResultsNb;
    }
    if(aSubResultsNb == 1) {
      anIt.Initialize(theResult);
      if(anIt.More()) {
        aResult = anIt.Value();
      }
    }
  }

  return aResult;
}
