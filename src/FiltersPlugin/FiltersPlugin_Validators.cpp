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

#include "FiltersPlugin_Validators.h"

#include <CollectionPlugin_Group.h>

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Session.h>

bool FiltersPlugin_ShapeTypeValidator::isValid(const AttributePtr& theAttribute,
                                               const std::list<std::string>& theArguments,
                                               Events_InfoMessage& theError) const
{
  if (!theAttribute)
    return false;
  FeaturePtr aFilterFeature = ModelAPI_Feature::feature(theAttribute->owner());
  if (!aFilterFeature)
    return false;

  // iterate on groups and find the one having the current filters feature
  DocumentPtr aCurDoc = ModelAPI_Session::get()->activeDocument();
  std::list<FeaturePtr> aFeatList = aCurDoc->allFeatures();
  for (std::list<FeaturePtr>::iterator anIt = aFeatList.begin(); anIt != aFeatList.end(); ++anIt)
    if ((*anIt)->getKind() == CollectionPlugin_Group::ID()) {
      AttributeSelectionListPtr aSelList =
          (*anIt)->selectionList(CollectionPlugin_Group::LIST_ID());
      if (aSelList->filters() == aFilterFeature) {
        TypeOfShape aType = shapeType(aSelList->selectionType());
        if (isValidAttribute(theAttribute, aType, theError))
          return true;
      }
    }

  return false;
}
