// Copyright (C) 2020-2024  CEA, EDF
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

#include "FeaturesAPI_Defeaturing.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

FeaturesAPI_Defeaturing::FeaturesAPI_Defeaturing(
    const std::shared_ptr<ModelAPI_Feature>& theFeature)
  : ModelHighAPI_Interface(theFeature)
{
  initialize();
}

FeaturesAPI_Defeaturing::FeaturesAPI_Defeaturing(
    const std::shared_ptr<ModelAPI_Feature>& theFeature,
    const std::list<ModelHighAPI_Selection>& theFacesToRemove)
  : ModelHighAPI_Interface(theFeature)
{
  if (initialize())
    setFaces(theFacesToRemove);
}

FeaturesAPI_Defeaturing::~FeaturesAPI_Defeaturing()
{
}

void FeaturesAPI_Defeaturing::setFaces(const std::list<ModelHighAPI_Selection>& theFacesToRemove)
{
  mybaseObjects->clear();
  fillAttribute(theFacesToRemove, mybaseObjects);

  execIfBaseNotEmpty();
}

void FeaturesAPI_Defeaturing::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionListPtr anAttrObjects =
    aBase->selectionList(FeaturesPlugin_Defeaturing::OBJECT_LIST_ID());

  theDumper << aBase << " = model.addDefeaturing(" << aDocName << ", "
            << anAttrObjects << ")" << std::endl;
}

void FeaturesAPI_Defeaturing::execIfBaseNotEmpty()
{
  if (mybaseObjects->size() > 0)
    execute();
}


//==================================================================================================

DefeaturingPtr addDefeaturing(const std::shared_ptr<ModelAPI_Document>& thePart,
                              const std::list<ModelHighAPI_Selection>& theFaces)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Defeaturing::ID());
  return DefeaturingPtr(new FeaturesAPI_Defeaturing(aFeature, theFaces));
}
