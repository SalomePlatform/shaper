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
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "GeomValidators_NotSelfIntersected.h"

#include <GeomAPI_Shape.h>

#include <Events_InfoMessage.h>

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Feature.h>

bool GeomValidators_NotSelfIntersected::isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                const std::list<std::string>& theArguments,
                                                Events_InfoMessage& theError) const
{
  if (theArguments.empty()) {
    theError = "Error: empty selection.";
    return false;
  }

  for (std::list<std::string>::const_iterator anIt = theArguments.cbegin();
       anIt != theArguments.cend();
       ++anIt)
  {
    std::string anArgument = *anIt;
    AttributePtr anAttribute = theFeature->attribute(anArgument);
    if (!anAttribute.get()) {
      theError = std::string("Error: Feature does not contain attribute: ") + anArgument;
      return false;
    }
    if (anAttribute->attributeType() == ModelAPI_AttributeSelectionList::typeId()) {
      AttributeSelectionListPtr anAttrSelectionList =
        std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(anAttribute);
      for (int anIndex = 0; anIndex < anAttrSelectionList->size(); ++anIndex) {
        AttributeSelectionPtr anAttrSelection = anAttrSelectionList->value(anIndex);
        if (!anAttrSelection.get()) {
          theError = "Error: Empty attribute selection.";
          return false;
        }
        ResultPtr aContext = anAttrSelection->context();
        if (!aContext.get()) {
          FeaturePtr aContFeat = anAttrSelection->contextFeature();
          if (!aContFeat.get() || !aContFeat->results().size()) {
            theError = "Error: Empty selection context.";
            return false;
          }
        }
        std::shared_ptr<GeomAPI_Shape> aShape = anAttrSelection->value();
        if (!aShape.get() && aContext.get()) {
          GeomShapePtr aContextShape = aContext->shape();
          aShape = aContextShape;
        }
        if (!aShape.get()) {
          theError = "Error: Empty shape.";
          return false;
        }

        if (aShape->isSelfIntersected(4)) {
          theError = "Error: One of selected shapes are self-intersected.";
          return false;
        }
      }
    } else {
      theError = std::string("Error: validator does not support attribute with type: ")
        + anAttribute->attributeType();
      return false;
    }
  }

  return true;
}

bool GeomValidators_NotSelfIntersected::isNotObligatory(std::string /*theFeature*/,
                                                        std::string /*theAttribute*/)
{
  return false;
}
