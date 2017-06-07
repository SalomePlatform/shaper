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

#include <GeomValidators_Finite.h>

#include <Events_InfoMessage.h>

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultConstruction.h>

bool GeomValidators_Finite::isValid(const AttributePtr& theAttribute,
                                   const std::list<std::string>& theArguments,
                                   Events_InfoMessage& theError) const
{
  bool aValid = true;

  const std::string anAttributeType = theAttribute->attributeType();

  if(anAttributeType == ModelAPI_AttributeSelection::typeId()) {
    AttributeSelectionPtr aSelectionAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
    ResultPtr aResult = aSelectionAttr->context();
    ResultConstructionPtr aConstruction =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aResult);
    if (aConstruction.get() && aConstruction->isInfinite()) {
      aValid = false;
      theError = "Infinite result is selected.";
    }
  } else if(anAttributeType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aSelectionListAttr =
                      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
    for (int i = 0, aSize = aSelectionListAttr->size(); i < aSize; i++) {
      AttributeSelectionPtr aSelectAttr = aSelectionListAttr->value(i);
      ResultPtr aResult = aSelectAttr->context();
      if (aResult.get() && aResult->groupName() == ModelAPI_ResultConstruction::group()) {
        ResultConstructionPtr aConstruction =
          std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aResult);
        if (aConstruction.get() && aConstruction->isInfinite()) {
          aValid = false;
          theError = "Infinite result is selected.";
        }
      }
    }
  }
  return aValid;
}

