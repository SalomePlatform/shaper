// Copyright (C) 2014-2024  CEA, EDF
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

#include <GeomValidators_MinObjectsSelected.h>

#include <Events_InfoMessage.h>

#include <BuildPlugin_Interpolation.h>

#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeSelectionList.h>

//=================================================================================================
bool GeomValidators_MinObjectsSelected::isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                const std::list<std::string>& theArguments,
                                                Events_InfoMessage& theError) const
{
  if (theArguments.size() != 2) {
// LCOV_EXCL_START
    theError =
      "Error: Wrong number of arguments (expected 2): selection list id and min number of objects";
    return false;
// LCOV_EXCL_STOP
  }
  //"Interpolation"
  if (theFeature->name().substr(0, 6) == L"Interp")
  {
    AttributeStringPtr anAttr =theFeature->string(BuildPlugin_Interpolation::CREATION_METHOD_ID());
    if (anAttr->isInitialized())
      if (anAttr->value() == BuildPlugin_Interpolation::CREATION_METHOD_ANALYTICAL_ID())
        return true;
  }

  std::string aSelectionListId = theArguments.front();
  AttributeSelectionListPtr anAttrSelList = theFeature->selectionList(aSelectionListId);
  if (!anAttrSelList.get()) {
// LCOV_EXCL_START
    theError = "Error: Could not get attribute \"%1\".";
    theError.arg(aSelectionListId);
    return false;
// LCOV_EXCL_STOP
  }
  int anObjectsNb = anAttrSelList->size();

  int aMinObjectsNb = atoi(theArguments.back().c_str());

  if (anObjectsNb < aMinObjectsNb) {
    theError = "Error: Attribute \"%1\" should contain at least %2 items.";
    theError.arg(aSelectionListId).arg(aMinObjectsNb);
    return false;
  }

  return true;
}
