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

#include "GeomValidators_Positive.h"

#include <Config_PropManager.h>
#include <Events_InfoMessage.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <cmath>

/// Global instance for validators factory
GeomValidators_Positive MY_POSITIVE_INSTANCE;

GeomValidators_Positive::GeomValidators_Positive()
{
  // this validator is registered in the factory on this library loading
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  aFactory->registerValidator("GeomValidators_Positive", this);
}

bool GeomValidators_Positive::isValid(const AttributePtr& theAttribute,
                                      const std::list<std::string>& theArguments,
                                      Events_InfoMessage& theError) const
{
  double aMinValue = 1.e-5;
  if(theArguments.size() == 1) {
    std::list<std::string>::const_iterator anIt = theArguments.begin();
    char *aErr;
    double aValue = Config_PropManager::stringToDouble((*anIt).c_str());
    if(aValue != 0) {
      // very probably ok
      aMinValue = aValue;
    }
  }

  if (theAttribute->attributeType() == ModelAPI_AttributeDouble::typeId()) {
    AttributeDoublePtr aDouble =
        std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theAttribute);
    if (!aDouble->isInitialized()) {
      theError = "Double is not initialized.";
      return false;
    }
    if (aDouble->value() <= aMinValue) {
      theError = "Value is too small.";
      return false;
    }
  }

  if (theAttribute->attributeType() == ModelAPI_AttributeInteger::typeId()) {
    AttributeIntegerPtr aInteger =
        std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(theAttribute);
    if (!aInteger->isInitialized()) {
      theError = "Integer is not initialized.";
      return false;
    }
    if (aInteger->value() <= floor(aMinValue)) {
      theError = "Integer is not positive.";
      return false;
    }
  }

  return true;
}
