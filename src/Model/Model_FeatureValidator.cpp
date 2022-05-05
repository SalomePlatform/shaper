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

#include <Model_FeatureValidator.h>

#include <Events_InfoMessage.h>

#include <Model_Validator.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_FiltersFeature.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Session.h>

#include <list>
#include <memory>

bool Model_FeatureValidator::isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const std::list<std::string>& /*theArguments*/,
                                     Events_InfoMessage& theError) const
{
  static Model_ValidatorsFactory* aValidators =
    static_cast<Model_ValidatorsFactory*>(ModelAPI_Session::get()->validators());

  std::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  // "Action" features has no data, but still valid. e.g "Remove Part"
  if (!aData->isValid()) {
    if (!theFeature->isAction())
      theError = "There is no data.";
    return theFeature->isAction();
  }
  const std::string kAllTypes = "";
  std::list<std::string> aLtAttributes = aData->attributesIDs(kAllTypes);
  std::list<std::string>::iterator it = aLtAttributes.begin();
  for (; it != aLtAttributes.end(); it++) {
    AttributePtr anAttr = aData->attribute(*it);
    if (!aValidators->isCase(theFeature, anAttr->id()))
      continue; // this attribute is not participated in the current case
    if (!anAttr->isInitialized()) { // attribute is not initialized
      std::map<std::string, std::set<std::string> >::const_iterator aFeatureFind =
        myNotObligatory.find(theFeature->getKind());
      if (aFeatureFind == myNotObligatory.end() || // and it is obligatory for filling
          aFeatureFind->second.find(*it) == aFeatureFind->second.end()) {
        theError = "Attribute \"%1\" is not initialized.";
        theError.addParameter(anAttr->id());
        // workaround for the filters selection feature: do not append the attribute id
        if (std::dynamic_pointer_cast<ModelAPI_FiltersFeature>(theFeature))
          theError.setContext(theFeature->getKind());
        else
          theError.setContext(theFeature->getKind() + ":" + anAttr->id());
        return false;
      }
    }
  }
  return true;
}

void Model_FeatureValidator::registerNotObligatory(std::string theFeature, std::string theAttribute)
{
  std::set<std::string>& anAttrs = myNotObligatory[theFeature];
  anAttrs.insert(theAttribute);
}

bool Model_FeatureValidator::isNotObligatory(std::string theFeature, std::string theAttribute)
{
  std::set<std::string>& anAttrs = myNotObligatory[theFeature];
  return anAttrs.find(theAttribute) != anAttrs.end();
}
