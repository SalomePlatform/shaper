// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include "CollectionAPI_GroupSubstraction.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

CollectionAPI_GroupSubstraction::CollectionAPI_GroupSubstraction(
    const std::shared_ptr<ModelAPI_Feature>& theFeature)
  : ModelHighAPI_Interface(theFeature)
{
  initialize();
}

CollectionAPI_GroupSubstraction::CollectionAPI_GroupSubstraction(
    const std::shared_ptr<ModelAPI_Feature>& theFeature,
    const std::list<ModelHighAPI_Selection>& theObjectsList,
    const std::list<ModelHighAPI_Selection>& theToolsList)
  : ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setObjectsList(theObjectsList);
    setToolsList(theToolsList);
  }
}

CollectionAPI_GroupSubstraction::~CollectionAPI_GroupSubstraction()
{
}

void CollectionAPI_GroupSubstraction::setObjectsList(
    const std::list<ModelHighAPI_Selection>& theGroupList)
{
  fillAttribute(theGroupList, myobjectsList);
  execute();
}

void CollectionAPI_GroupSubstraction::setToolsList(
    const std::list<ModelHighAPI_Selection>& theGroupList)
{
  fillAttribute(theGroupList, mytoolsList);
  execute();
}

void CollectionAPI_GroupSubstraction::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionListPtr anObjectsList =
      aBase->selectionList(CollectionPlugin_GroupSubstraction::LIST_ID());
  AttributeSelectionListPtr aToolsList =
      aBase->selectionList(CollectionPlugin_GroupSubstraction::TOOLS_ID());

  theDumper << aBase << " = model.addGroupSubstraction(" << aDocName << ", "
            << anObjectsList << ", " << aToolsList << ")" << std::endl;
}

GroupSubstractionPtr addGroupSubstraction(const std::shared_ptr<ModelAPI_Document>& thePart,
                                        const std::list<ModelHighAPI_Selection>& theObjectsList,
                                        const std::list<ModelHighAPI_Selection>& theToolsList)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      thePart->addFeature(CollectionAPI_GroupSubstraction::ID());
  return GroupSubstractionPtr(new CollectionAPI_GroupSubstraction(
      aFeature, theObjectsList, theToolsList));
}
