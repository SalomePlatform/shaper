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

#include "SketchPlugin_ExternalValidator.h"
#include "SketchPlugin_Feature.h"

#include <Events_InfoMessage.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_AttributeRefAttr.h>

bool SketchPlugin_ExternalValidator::isValid(const AttributePtr& theAttribute,
                                             const std::list<std::string>& theArguments,
                                             Events_InfoMessage& theError) const
{
  if (theArguments.size() != 1)
    return true;

  // ask whether the feature of the attribute is external
  bool isAttributeExternal = isExternalAttribute(theAttribute);

  // ask whether the feature of the attribute by parameter identifier is external
  std::string aFrontArgument = theArguments.front();
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
  bool isParameterExternal = isExternalAttribute(aFeature->attribute(aFrontArgument));

  // it is not possible that both features, attribute and attribute in parameter, are external
  if (isAttributeExternal && isParameterExternal) {
    theError = "Both features, attribute and attribute in parameter, are external.";
    return false;
  }
  return true;
}

bool SketchPlugin_ExternalValidator::isExternalAttribute(const AttributePtr& theAttribute) const
{
  bool isExternal = false;
  AttributeRefAttrPtr anAttribute =
    std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);

  if (anAttribute.get() != NULL) {
    FeaturePtr anArgumentFeature = ModelAPI_Feature::feature(anAttribute->object());
    if (anArgumentFeature.get() != NULL) {
      std::shared_ptr<SketchPlugin_Feature> aSketchFeature =
                        std::dynamic_pointer_cast<SketchPlugin_Feature>(anArgumentFeature);
      if (aSketchFeature.get() != NULL) {
        isExternal = aSketchFeature->isExternal();
      }
    }
  }
  return isExternal;
}
