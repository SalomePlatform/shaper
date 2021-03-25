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

#include "CollectionPlugin_GroupAddition.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultGroup.h>

#include <GeomAPI_ShapeIterator.h>
#include <GeomAlgoAPI_CompoundBuilder.h>

CollectionPlugin_GroupAddition::CollectionPlugin_GroupAddition()
{
}

void CollectionPlugin_GroupAddition::initAttributes()
{
  data()->addAttribute(CollectionPlugin_GroupAddition::LIST_ID(),
                       ModelAPI_AttributeSelectionList::typeId());
}

static void explodeCompound(const GeomShapePtr& theCompound, ListOfShape& theSubs)
{
  if (theCompound->isCompound()) {
    GeomAPI_ShapeIterator anIt(theCompound);
    for (; anIt.more(); anIt.next())
      explodeCompound(anIt.current(), theSubs);
  }
  else
    theSubs.push_back(theCompound);
}

static void keepUniqueShapes(ListOfShape& theShapes)
{
  ListOfShape::iterator anIt = theShapes.begin();
  while (anIt != theShapes.end()) {
    GeomShapePtr aCurrent = *anIt;
    ListOfShape::iterator anIt2 = theShapes.begin();
    for (; anIt2 != anIt; ++anIt2)
      if (aCurrent->isEqual(*anIt2))
        break;
    if (anIt2 != anIt) {
      // the same shape is found
      ListOfShape::iterator aRemoveIt = anIt++;
      theShapes.erase(aRemoveIt);
    }
    else
      ++anIt;
  }
}

void CollectionPlugin_GroupAddition::execute()
{
  ResultGroupPtr aGroup = document()->createGroup(data());
  // clean the result of the operation
  aGroup->store(GeomShapePtr());

  // collect all unique sub-shapes
  GeomShapePtr aCompound = aGroup->shape();
  ListOfShape aSubs;
  explodeCompound(aCompound, aSubs);
  keepUniqueShapes(aSubs);
  aCompound = aSubs.empty() ? GeomShapePtr() : GeomAlgoAPI_CompoundBuilder::compound(aSubs);
  aGroup->store(aCompound);

  // update the type of selection
  updateGroupType(LIST_ID());

  setResult(aGroup);
}
