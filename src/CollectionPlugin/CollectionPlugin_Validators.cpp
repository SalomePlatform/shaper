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

#include "CollectionPlugin_Validators.h"
#include "CollectionPlugin_Group.h"
#include "CollectionPlugin_Field.h"
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultGroup.h>
#include <Events_InfoMessage.h>


bool CollectionPlugin_FieldValidator::isValid(const FeaturePtr& theFeature,
  const std::list<std::string>& theArguments,
  Events_InfoMessage& theError) const
{
  AttributeSelectionListPtr aSelList =
    theFeature->selectionList(CollectionPlugin_Field::SELECTED_ID());
  std::string aTypeStr = aSelList->selectionType();
  if (aTypeStr == "part")
    return true;
  if (aSelList->isInitialized()) {
    int aSize = aSelList->size();
    bool aIsDefined = aSize > 0;
    if (!aIsDefined)
      theError = "Selection list is not initialized";
    return aIsDefined;
  }
  theError = "Selection list is not initialized";
  return false;
}

bool CollectionPlugin_GroupOperationAttributeValidator::isValid(
    const AttributePtr& theAttribute,
    const std::list<std::string>& theArguments,
    Events_InfoMessage& theError) const
{
  AttributeSelectionListPtr aSelList =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  if (!aSelList) {
    theError = "Error: This validator can only work with selection list of attributes";
    return false;
  }

  FeaturePtr anOwner = ModelAPI_Feature::feature(theAttribute->owner());
  // check selected items
  std::string aType;
  for (int anIndex = 0; anIndex < aSelList->size(); ++anIndex) {
    AttributeSelectionPtr aCurSelection = aSelList->value(anIndex);
    // applicable the groups only
    ResultPtr aGroupResult = aCurSelection->context();
    if (aGroupResult->groupName() != ModelAPI_ResultGroup::group()) {
      theError = "Error: Groups can be selected only.";
      return false;
    }
    // groups of same type can be selected only
    FeaturePtr aGroupFeature = ModelAPI_Feature::feature(aGroupResult->data()->owner());
    std::string aGroupType =
        aGroupFeature->selectionList(CollectionPlugin_Group::LIST_ID())->selectionType();
    if (aType.empty())
      aType = aGroupType;
    else if (aType != aGroupType) {
      theError = "Error: Groups should have same type";
      return false;
    }
  }
  return true;
}
