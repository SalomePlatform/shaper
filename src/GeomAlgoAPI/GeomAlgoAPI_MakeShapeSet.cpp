// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include "GeomAlgoAPI_MakeShapeSet.h"


#include <NCollection_List.hxx>
#include <NCollection_Map.hxx>
#include <TopoDS_Shape.hxx>

//==================================================================================================
GeomAlgoAPI_MakeShapeSet::GeomAlgoAPI_MakeShapeSet()
: GeomAlgoAPI_MakeShapeList()
{}

//==================================================================================================
GeomAlgoAPI_MakeShapeSet::GeomAlgoAPI_MakeShapeSet(const ListOfMakeShape& theMakeShapeSet)
: GeomAlgoAPI_MakeShapeList(theMakeShapeSet)
{
}

//==================================================================================================
void GeomAlgoAPI_MakeShapeSet::generated(const GeomShapePtr theOldShape,
                                         ListOfShape& theNewShapes)
{
  if (myListOfMakeShape.empty()) {
    return;
  }

  for (ListOfMakeShape::iterator aBuilderIt = myListOfMakeShape.begin();
      aBuilderIt != myListOfMakeShape.end();
      ++aBuilderIt)
  {
    GeomMakeShapePtr aMakeShape = *aBuilderIt;
    aMakeShape->generated(theOldShape, theNewShapes);
  }
}

//==================================================================================================
void GeomAlgoAPI_MakeShapeSet::modified(const GeomShapePtr theOldShape,
                                        ListOfShape& theNewShapes)
{
  if (myListOfMakeShape.empty()) {
    return;
  }

  for (ListOfMakeShape::iterator aBuilderIt = myListOfMakeShape.begin();
      aBuilderIt != myListOfMakeShape.end();
      ++aBuilderIt)
  {
    GeomMakeShapePtr aMakeShape = *aBuilderIt;
    ListOfShape aModifiedShapes;
    aMakeShape->modified(theOldShape, theNewShapes);
  }
}
