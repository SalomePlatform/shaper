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

#include "GeomAlgoAPI_MakeSweep.h"

//=================================================================================================
const ListOfShape& GeomAlgoAPI_MakeSweep::fromShapes() const
{
  return myFromShapes;
}

//=================================================================================================
const ListOfShape& GeomAlgoAPI_MakeSweep::toShapes() const
{
  return myToShapes;
}

//=================================================================================================
void GeomAlgoAPI_MakeSweep::addFromShape(const std::shared_ptr<GeomAPI_Shape> theFace)
{
  myFromShapes.push_back(theFace);
}

//=================================================================================================
void GeomAlgoAPI_MakeSweep::setFromShapes(const ListOfShape& theListOfFaces)
{
  myFromShapes = theListOfFaces;
}

//=================================================================================================
void GeomAlgoAPI_MakeSweep::addToShape(const std::shared_ptr<GeomAPI_Shape> theFace)
{
  myToShapes.push_back(theFace);
}

//=================================================================================================
void GeomAlgoAPI_MakeSweep::setToShapes(const ListOfShape& theListOfFaces)
{
  myToShapes = theListOfFaces;
}
