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

#include "CollectionAPI_GroupAddition.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

CollectionAPI_GroupAddition::CollectionAPI_GroupAddition(
    const std::shared_ptr<ModelAPI_Feature>& theFeature)
  : ModelHighAPI_Interface(theFeature)
{
  initialize();
}

CollectionAPI_GroupAddition::CollectionAPI_GroupAddition(
    const std::shared_ptr<ModelAPI_Feature>& theFeature,
    const std::list<ModelHighAPI_Selection>& theGroupList)
  : ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setGroupList(theGroupList);
  }
}

CollectionAPI_GroupAddition::~CollectionAPI_GroupAddition()
{
}

void CollectionAPI_GroupAddition::setGroupList(
    const std::list<ModelHighAPI_Selection>& theGroupList)
{
  fillAttribute(theGroupList, mygroupList);
  execute();
}

void CollectionAPI_GroupAddition::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionListPtr anAttrList =
      aBase->selectionList(CollectionPlugin_GroupAddition::LIST_ID());

  theDumper << aBase << " = model.addGroupAddition(" << aDocName << ", "
            << anAttrList << ")" << std::endl;
}

GroupAdditionPtr addGroupAddition(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theGroupList)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      thePart->addFeature(CollectionAPI_GroupAddition::ID());
  return GroupAdditionPtr(new CollectionAPI_GroupAddition(aFeature, theGroupList));
}
