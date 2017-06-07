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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include <GeomAlgoAPI_MakeShapeCustom.h>

//=================================================================================================
GeomAlgoAPI_MakeShapeCustom::GeomAlgoAPI_MakeShapeCustom()
{}

//=================================================================================================
void GeomAlgoAPI_MakeShapeCustom::setResult(const std::shared_ptr<GeomAPI_Shape> theShape)
{
  setShape(theShape);
}

//=================================================================================================
bool GeomAlgoAPI_MakeShapeCustom::addModified(const std::shared_ptr<GeomAPI_Shape> theBase,
                                              const std::shared_ptr<GeomAPI_Shape> theResult)
{
  return myModified.add(theBase, theResult);
}

//=================================================================================================
bool GeomAlgoAPI_MakeShapeCustom::addGenerated(const std::shared_ptr<GeomAPI_Shape> theBase,
                                               const std::shared_ptr<GeomAPI_Shape> theResult)
{
  return myGenerated.add(theBase, theResult);
}

//=================================================================================================
bool GeomAlgoAPI_MakeShapeCustom::addDeleted(const std::shared_ptr<GeomAPI_Shape> theShape)
{
  return myDeleted.bind(theShape, theShape);
}

//=================================================================================================
void GeomAlgoAPI_MakeShapeCustom::generated(const std::shared_ptr<GeomAPI_Shape> theShape,
                                            ListOfShape& theHistory)
{
  ListOfShape aGenerated;
  if(myGenerated.find(theShape, aGenerated)) {
    theHistory.insert(theHistory.end(), aGenerated.begin(), aGenerated.end());
  }
}

//=================================================================================================
void GeomAlgoAPI_MakeShapeCustom::modified(const std::shared_ptr<GeomAPI_Shape> theShape,
                                           ListOfShape& theHistory)
{
  ListOfShape aModified;
  if(myModified.find(theShape, aModified)) {
    theHistory.insert(theHistory.end(), aModified.begin(), aModified.end());
  }
}

//=================================================================================================
bool GeomAlgoAPI_MakeShapeCustom::isDeleted(const std::shared_ptr<GeomAPI_Shape> theShape)
{
  return myDeleted.isBound(theShape);
}