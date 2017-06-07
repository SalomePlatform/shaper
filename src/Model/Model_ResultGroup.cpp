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

#include <Model_ResultGroup.h>
#include <ModelAPI_AttributeSelectionList.h>

#include <GeomAlgoAPI_CompoundBuilder.h>

#include <Config_PropManager.h>

Model_ResultGroup::Model_ResultGroup(std::shared_ptr<ModelAPI_Data> theOwnerData)
{
  myOwnerData = theOwnerData;
}

void Model_ResultGroup::colorConfigInfo(std::string& theSection, std::string& theName,
                                       std::string& theDefault)
{
  theSection = "Visualization";
  theName = "result_group_color";
  theDefault = DEFAULT_COLOR();
}

std::shared_ptr<GeomAPI_Shape> Model_ResultGroup::shape()
{
  std::shared_ptr<GeomAPI_Shape> aResult;
  if (myOwnerData) {
    AttributeSelectionListPtr aList = myOwnerData->selectionList("group_list");
    if (aList) {
      std::list<std::shared_ptr<GeomAPI_Shape> > aSubs;
      for(int a = aList->size() - 1; a >= 0; a--) {
        std::shared_ptr<GeomAPI_Shape> aSelection = aList->value(a)->value();
        if (aSelection && !aSelection->isNull()) {
          aSubs.push_back(aSelection);
        }
      }
      if (!aSubs.empty()) {
        aResult = GeomAlgoAPI_CompoundBuilder::compound(aSubs);
      }
    }
  }
  return aResult;
}
