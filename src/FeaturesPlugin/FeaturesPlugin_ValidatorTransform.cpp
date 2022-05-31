// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include "FeaturesPlugin_ValidatorTransform.h"

#include <Events_InfoMessage.h>

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>

bool FeaturesPlugin_ValidatorTransform::isValid(const AttributePtr& theAttribute,
                                                const std::list<std::string>& /*theArguments*/,
                                                Events_InfoMessage& theError) const
{
  bool aValid = true;
  std::string anAttributeType = theAttribute->attributeType();
  if (anAttributeType != ModelAPI_AttributeSelectionList::typeId()) {
// LCOV_EXCL_START
    theError = "The attribute with the %1 type is not processed";
    theError.arg(theAttribute->attributeType());
    return false;
// LCOV_EXCL_STOP
  }

  std::shared_ptr<ModelAPI_AttributeSelectionList> aCurSelList =
                           std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);

  DocumentPtr aDocument = theAttribute->owner()->document();
  SessionPtr aMgr = ModelAPI_Session::get();
  bool isPartSetDocument = aDocument == aMgr->moduleDocument();
  bool isCheckCompsolid = !theAttribute->owner()->data()->version().empty();

  std::string anErrorGroupName;
  for(int i = 0; i < aCurSelList->size() && aValid; i++) {
    std::shared_ptr<ModelAPI_AttributeSelection> aSelAttribute = aCurSelList->value(i);
    ResultPtr aResult = aSelAttribute->context();
    if (!aResult) {
      // this could be a whole feature selected
      FeaturePtr aFeature = aSelAttribute->contextFeature();
      if (aFeature.get() && aFeature->results().size() > 0) {
        if (aFeature->firstResult()->groupName() != ModelAPI_ResultBody::group()) {
          anErrorGroupName = aFeature->firstResult()->groupName();
          aValid = false;
          break;
        }
        continue;
      }
      theError = "Invalid selection.";
      return false;
    }
    if (isPartSetDocument) // PartSet document: Result Part is valid
      aValid = aResult->groupName() == ModelAPI_ResultPart::group();
    else { // Part document: Result CompSolid is valid, but the solid in compsolid is forbidden
      aValid = aResult->groupName() == ModelAPI_ResultBody::group();
      if (aValid && isCheckCompsolid) {
        ResultBodyPtr aComp = ModelAPI_Tools::bodyOwner(aResult);
        if (aComp && aComp->shape()->shapeType() == GeomAPI_Shape::COMPSOLID) {
          theError = "Selecting a part of compsolid is forbidden.";
          return false;
        }
      }
    }
    if (!aValid)
      anErrorGroupName = aResult->groupName();
  }
  if (!aValid) {
    std::string aResultGroupName = isPartSetDocument ? ModelAPI_ResultPart::group()
                                                     : ModelAPI_ResultBody::group();
    theError = "Objects from the %1 group can be selected in the %2 document, "
               "but an objects from the %3 group is selected.";
    theError.arg(aResultGroupName).arg(aDocument->kind()).arg(anErrorGroupName);
  }
  return aValid;
}
