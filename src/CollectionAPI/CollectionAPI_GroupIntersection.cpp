// Copyright (C) 2014-2023  CEA/DEN, EDF R&D
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

#include "CollectionAPI_GroupIntersection.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

CollectionAPI_GroupIntersection::CollectionAPI_GroupIntersection(
    const std::shared_ptr<ModelAPI_Feature>& theFeature)
  : ModelHighAPI_Interface(theFeature)
{
  initialize();
}

CollectionAPI_GroupIntersection::CollectionAPI_GroupIntersection(
    const std::shared_ptr<ModelAPI_Feature>& theFeature,
    const std::list<ModelHighAPI_Selection>& theGroupList)
  : ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setGroupList(theGroupList);
  }
}

CollectionAPI_GroupIntersection::~CollectionAPI_GroupIntersection()
{
}

void CollectionAPI_GroupIntersection::setGroupList(
    const std::list<ModelHighAPI_Selection>& theGroupList)
{
  fillAttribute(theGroupList, mygroupList);
  execute();
}

void CollectionAPI_GroupIntersection::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionListPtr anAttrList =
      aBase->selectionList(CollectionPlugin_GroupIntersection::LIST_ID());

  theDumper << aBase << " = model.addGroupIntersection(" << aDocName << ", "
            << anAttrList << ")" << std::endl;
}

GroupIntersectionPtr addGroupIntersection(const std::shared_ptr<ModelAPI_Document>& thePart,
                                          const std::list<ModelHighAPI_Selection>& theGroupList)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
      thePart->addFeature(CollectionAPI_GroupIntersection::ID());
  return GroupIntersectionPtr(new CollectionAPI_GroupIntersection(aFeature, theGroupList));
}
