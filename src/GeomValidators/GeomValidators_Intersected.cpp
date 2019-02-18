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

#include "GeomValidators_Intersected.h"

#include <GeomAPI_Shape.h>

#include <Events_InfoMessage.h>

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_Feature.h>

bool GeomValidators_Intersected::isValid(const AttributePtr& theAttribute,
                                         const std::list<std::string>& theArguments,
                                         Events_InfoMessage& theError) const
{
// LCOV_EXCL_START
  if (!theAttribute) {
    theError = "Error: empty selection.";
    return false;
  }

  if (theArguments.empty()) {
    theError = "Error: compare with nothing";
    return false;
  }
// LCOV_EXCL_STOP

  FeaturePtr aFeature = ModelAPI_Feature::feature(theAttribute->owner());

  // get shape selected by theAttribute
  AttributeSelectionPtr aSelection =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
  GeomShapePtr aBaseShape;
  if (aSelection)
    aBaseShape = aSelection->value();
  if (!aBaseShape && aSelection->context())
    aBaseShape = aSelection->context()->shape();
  if (!aBaseShape)
    return true; // nothing selected, thus applicable

  // check intersection with all arguments
  bool isOk = true;
  for (std::list<std::string>::const_iterator anIt = theArguments.begin();
       anIt != theArguments.end() && isOk; ++anIt) {
    aSelection = aFeature->selection(*anIt);
// LCOV_EXCL_START
    if (!aSelection) {
      theError = "Error: incorrect type of attribute";
      return false;
    }
// LCOV_EXCL_STOP

    GeomShapePtr aShape;
    if (aSelection)
      aShape = aSelection->value();
    if (!aShape && aSelection->context())
      aShape = aSelection->context()->shape();
    if (aShape) {
      isOk = aBaseShape->isIntersect(aShape);
    }
  }

  if (!isOk)
    theError = "Error: arguments are not intersected";
  return isOk;
}
