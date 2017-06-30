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

#include <GeomValidators_PartitionArguments.h>

#include <Events_InfoMessage.h>

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeSelectionList.h>

//=================================================================================================
bool GeomValidators_PartitionArguments::isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                              const std::list<std::string>& theArguments,
                                              Events_InfoMessage& theError) const
{
  if(theArguments.size() != 3) {
    theError = "Wrong number of arguments (expected 3).";
    return false;
  }

  int anObjectsNb = 0, aToolsNb = 0;
  bool isCombine = true;

  std::list<std::string>::const_iterator anIt = theArguments.begin(), aLast = theArguments.end();

  std::shared_ptr<ModelAPI_AttributeSelectionList> anAttrSelList = theFeature->selectionList(*anIt);
  if(anAttrSelList) {
    anObjectsNb = anAttrSelList->size();
  }
  anIt++;

  anAttrSelList = theFeature->selectionList(*anIt);
  if(anAttrSelList) {
    aToolsNb = anAttrSelList->size();
  }
  anIt++;

  std::shared_ptr<ModelAPI_AttributeBoolean> anAttrBool = theFeature->boolean(*anIt);
  if(anAttrBool) {
    isCombine = anAttrBool->value();
  }

  if((anObjectsNb > 0 && aToolsNb > 0) ||
     (isCombine && anObjectsNb != 0 && (anObjectsNb + aToolsNb > 1))) {
    return true;
  }

  theError = "Not enough arguments";
  return false;
}

//=================================================================================================
bool GeomValidators_PartitionArguments::isNotObligatory(std::string theFeature,
                                                        std::string theAttribute)
{
  if(theAttribute == "tool_objects") {
    return true;
  }

  return false;
}
