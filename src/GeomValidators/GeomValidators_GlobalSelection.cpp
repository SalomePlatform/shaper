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

#include "GeomValidators_GlobalSelection.h"

#include <Events_InfoMessage.h>

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_ResultBody.h>

bool GeomValidators_GlobalSelection::isValid(const AttributePtr& theAttribute,
                                             const std::list<std::string>& theArguments,
                                             Events_InfoMessage& theError) const
{
  std::string anAttributeType = theAttribute->attributeType();
  if(anAttributeType == ModelAPI_AttributeSelection::typeId()) {
    AttributeSelectionPtr anAttrSelection =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
    ResultPtr aContext = anAttrSelection->context();
    if (!aContext.get()) {
      theError = "Error: Context is empty.";
      return false;
    }

    GeomShapePtr aShape = anAttrSelection->value();
    if (aShape && !aShape->isEqual(aContext->shape())) {
      theError = "Error: Local selection not allowed.";
      return false;
    }
  } else if(anAttributeType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr anAttrSelectionList =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);

    // All objects should not be result constructions.
    for(int anIndex = 0, aSize = anAttrSelectionList->size(); anIndex < aSize; ++anIndex) {
      AttributeSelectionPtr anAttrSelection = anAttrSelectionList->value(anIndex);
      if(!isValid(anAttrSelection, theArguments, theError)) {
        return false;
      }
    }
  } else {
// LCOV_EXCL_START
    theError = "Error: Attribute \"%1\" does not supported by this validator.";
    theError.arg(anAttributeType);
    return false;
// LCOV_EXCL_STOP
  }

  return true;
}
