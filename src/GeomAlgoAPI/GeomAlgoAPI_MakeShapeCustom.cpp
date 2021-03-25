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

#include <GeomAlgoAPI_MakeShapeCustom.h>

//==================================================================================================
GeomAlgoAPI_MakeShapeCustom::GeomAlgoAPI_MakeShapeCustom()
{
}

//==================================================================================================
void GeomAlgoAPI_MakeShapeCustom::setResult(const GeomShapePtr theShape)
{
  setShape(theShape);
}

//==================================================================================================
bool GeomAlgoAPI_MakeShapeCustom::addModified(const GeomShapePtr theOldShape,
                                              const GeomShapePtr theNewShape)
{
  if (!isValidForHistory(theNewShape)) return false;
  GeomShapePtr aNewShape = theNewShape;
  return myModified.add(theOldShape, aNewShape);
}

//==================================================================================================
bool GeomAlgoAPI_MakeShapeCustom::addGenerated(const GeomShapePtr theOldShape,
                                               const GeomShapePtr theNewShape)
{
  if (!isValidForHistory(theNewShape)) return false;
  GeomShapePtr aNewShape = theNewShape;
  return myGenerated.add(theOldShape, aNewShape);
}

//==================================================================================================
bool GeomAlgoAPI_MakeShapeCustom::addDeleted(const GeomShapePtr theOldShape)
{
  return myDeleted.bind(theOldShape, theOldShape);
}

//==================================================================================================
void GeomAlgoAPI_MakeShapeCustom::generated(const GeomShapePtr theOldShape,
                                            ListOfShape& theNewShapes)
{
  ListOfShape aGenerated;
  if(myGenerated.find(theOldShape, aGenerated)) {
    theNewShapes.insert(theNewShapes.end(), aGenerated.begin(), aGenerated.end());
  }
}

//==================================================================================================
void GeomAlgoAPI_MakeShapeCustom::modified(const GeomShapePtr theOldShape,
                                           ListOfShape& theNewShapes)
{
  ListOfShape aModified;
  if(myModified.find(theOldShape, aModified)) {
    theNewShapes.insert(theNewShapes.end(), aModified.begin(), aModified.end());
  }
}

//==================================================================================================
bool GeomAlgoAPI_MakeShapeCustom::isDeleted(const GeomShapePtr theOldShape)
{
  return myDeleted.isBound(theOldShape);
}
