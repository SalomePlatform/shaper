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

#include "CollectionPlugin_GroupIntersection.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultGroup.h>

#include <GeomAPI_ShapeIterator.h>
#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_ShapeTools.h>

typedef std::set<GeomShapePtr, GeomAPI_Shape::ComparatorWithOri> SetOfShape;

CollectionPlugin_GroupIntersection::CollectionPlugin_GroupIntersection()
{
}

void CollectionPlugin_GroupIntersection::initAttributes()
{
  data()->addAttribute(CollectionPlugin_GroupIntersection::LIST_ID(),
                       ModelAPI_AttributeSelectionList::typeId());
}

static void explodeCompound(const GeomShapePtr& theCompound, SetOfShape& theSet)
{
  for (GeomAPI_ShapeIterator anIt(theCompound); anIt.more(); anIt.next())
    theSet.insert(anIt.current());
}

static void keepCommonShapes(const GeomShapePtr& theCompound, ListOfShape& theShapes)
{
  if (theShapes.empty()) {
    for (GeomAPI_ShapeIterator anIt(theCompound); anIt.more(); anIt.next())
      theShapes.push_back(anIt.current());
  }
  else {
    SetOfShape aSubs;
    explodeCompound(theCompound, aSubs);

    ListOfShape::iterator anIt = theShapes.begin();
    while (anIt != theShapes.end()) {
      if (aSubs.find(*anIt) == aSubs.end()) {
        // the shape is not found
        ListOfShape::iterator aRemoveIt = anIt++;
        theShapes.erase(aRemoveIt);
      }
      else
        ++anIt;
    }
  }
}

void CollectionPlugin_GroupIntersection::execute()
{
  ResultGroupPtr aGroup = document()->createGroup(data());
  // clean the result of the operation
  aGroup->store(GeomShapePtr());

  // shapes containing in each group
  ListOfShape aCommon;

  // collect shapes containing in all groups
  AttributeSelectionListPtr aSelectedGroups = selectionList(LIST_ID());
  for (int anIndex = 0; anIndex < aSelectedGroups->size(); ++anIndex) {
    AttributeSelectionPtr aCurSelection = aSelectedGroups->value(anIndex);
    ResultGroupPtr aCurGroup =
        std::dynamic_pointer_cast<ModelAPI_ResultGroup>(aCurSelection->context());
    keepCommonShapes(aCurGroup->shape(), aCommon);
    if (aCommon.empty())
      break;
  }

  if (aCommon.empty()) {
    setError("Error: Empty result.");
    removeResults(0);
  }
  else {
    // update the type of selection
    updateGroupType(LIST_ID());

    GeomShapePtr aCompound = GeomAlgoAPI_CompoundBuilder::compound(aCommon);
    aGroup->store(aCompound);
    setResult(aGroup);
  }
}
