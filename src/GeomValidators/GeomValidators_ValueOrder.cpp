// Copyright (C) 2017-20xx  CEA/DEN, EDF R&D
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

#include "GeomValidators_ValueOrder.h"

#include <Config_PropManager.h>
#include <Events_InfoMessage.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

static double attributeValue(AttributePtr theAttr)
{
  AttributeIntegerPtr anIntAttr = std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(theAttr);
  if (anIntAttr)
    return (double)anIntAttr->value();
  AttributeDoublePtr aDoubleAttr = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theAttr);
  if (aDoubleAttr)
    return aDoubleAttr->value();
  return 0.0;
}

static bool isGreaterOrEqual(AttributePtr theFirstArg, AttributePtr theSecondArg)
{
  if (theFirstArg && theFirstArg->isInitialized() &&
      theSecondArg && theSecondArg->isInitialized())
    return attributeValue(theFirstArg) >= attributeValue(theSecondArg);
  return false;
}

static bool isLessOrEqual(AttributePtr theFirstArg, AttributePtr theSecondArg)
{
  if (theFirstArg && theFirstArg->isInitialized() &&
      theSecondArg && theSecondArg->isInitialized())
    return attributeValue(theFirstArg) <= attributeValue(theSecondArg);
  return false;
}

// Check the attributes are satisfy theCompare function
static bool isValidOrder(const AttributePtr& theAttribute,
                         const std::list<std::string>& theArguments,
                         Events_InfoMessage& theError,
                         bool (*theCompare)(AttributePtr, AttributePtr))
{
  std::string anAttrType = theAttribute->attributeType();
  if (anAttrType != ModelAPI_AttributeDouble::typeId() &&
      anAttrType != ModelAPI_AttributeInteger::typeId()) {
    theError = "Unsupported attribute type (integer or double applicable)";
    return false;
  }

  FeaturePtr anOwner = ModelAPI_Feature::feature(theAttribute->owner());
  if (!anOwner) {
    theError = "Attribute without owner";
    return false;
  }

  for (std::list<std::string>::const_iterator anIt = theArguments.begin();
       anIt != theArguments.end(); ++anIt) {
    // check the argument links to the attribute of the current feature
    AttributePtr aCurAttr = anOwner->attribute(*anIt);
    if (!aCurAttr) {
      theError = "Arguments should be names of attributes of current feature";
      return false;
    }

    // compare values
    if (!(*theCompare)(theAttribute, aCurAttr)) {
      theError = "Attributes have incorrect order";
      return false;
    }
  }

  return true;
}



/// Global instance for validators factory
GeomValidators_GreaterOrEqual MY_GEQ_INSTANCE;
GeomValidators_LessOrEqual MY_LEQ_INSTANCE;

GeomValidators_GreaterOrEqual::GeomValidators_GreaterOrEqual()
{
  // this validator is registered in the factory on this library loading
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  aFactory->registerValidator("GeomValidators_GreaterOrEqual", this);
}

bool GeomValidators_GreaterOrEqual::isValid(const AttributePtr& theAttribute,
                                            const std::list<std::string>& theArguments,
                                            Events_InfoMessage& theError) const
{
  return isValidOrder(theAttribute, theArguments, theError, &isGreaterOrEqual);
}



GeomValidators_LessOrEqual::GeomValidators_LessOrEqual()
{
  // this validator is registered in the factory on this library loading
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  aFactory->registerValidator("GeomValidators_LessOrEqual", this);
}

bool GeomValidators_LessOrEqual::isValid(const AttributePtr& theAttribute,
                                         const std::list<std::string>& theArguments,
                                         Events_InfoMessage& theError) const
{
  return isValidOrder(theAttribute, theArguments, theError, &isLessOrEqual);
}
