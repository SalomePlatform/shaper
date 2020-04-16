// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "GeomValidators_FeatureKind.h"

#include <Events_InfoMessage.h>

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_Object.h>

#define DEBUG_EXTRUSION_INVALID_SKETCH
#ifdef DEBUG_EXTRUSION_INVALID_SKETCH
  #include <ModelAPI_CompositeFeature.h>
#endif

bool GeomValidators_FeatureKind::isValid(const AttributePtr& theAttribute,
                                      const std::list<std::string>& theArguments,
                                      Events_InfoMessage& theError) const
{
  bool isSketchEntities = true;
  std::set<std::string> anEntityKinds;
  std::string anEntityKindsStr;
  std::list<std::string>::const_iterator anIt = theArguments.begin(), aLast = theArguments.end();
  for (; anIt != aLast; anIt++) {
    anEntityKinds.insert(*anIt);
    if (!anEntityKindsStr.empty())
      anEntityKindsStr += ", ";
    anEntityKindsStr += *anIt;
  }

  std::string anAttributeType = theAttribute->attributeType();
  if (anAttributeType == ModelAPI_AttributeSelection::typeId()) {
    AttributeSelectionPtr aSelectAttr =
                      std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
    ObjectPtr anObject = aSelectAttr->context();
    // a context of the selection attribute is a feature result. It can be a case when the result
    // of the feature is null, e.g. the feature is modified and has not been executed yet.
    // The validator returns an invalid result here. The case is an extrusion built on a sketch
    // feature. A new sketch element creation leads to an empty result.
    if (!anObject.get())
      isSketchEntities = false;
    else {
      FeaturePtr aFeature = ModelAPI_Feature::feature(anObject);
      isSketchEntities = anEntityKinds.find(aFeature->getKind()) != anEntityKinds.end();
    }
  }
  if (!isSketchEntities) {
    theError = "It refers to feature, which kind is not in the list: " + anEntityKindsStr;
  }

  return isSketchEntities;
}
