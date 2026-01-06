// Copyright (C) 2014-2026  CEA, EDF
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

#include "FeaturesAPI_RemoveExtraEdges.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_RemoveExtraEdges::FeaturesAPI_RemoveExtraEdges
            (const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_RemoveExtraEdges::FeaturesAPI_RemoveExtraEdges
            (const std::shared_ptr<ModelAPI_Feature>& theFeature,
             const ModelHighAPI_Selection& theBase,
             const bool theDoUniteFaces)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(theBase, mybase);
    fillAttribute(theDoUniteFaces, myuniteFaces);

    execute();
  }
}

//==================================================================================================
FeaturesAPI_RemoveExtraEdges::~FeaturesAPI_RemoveExtraEdges()
{
}

//==================================================================================================
void FeaturesAPI_RemoveExtraEdges::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aPartName = theDumper.name(aBase->document());

  AttributeSelectionPtr anAttrBaseShape =
    aBase->selection(FeaturesPlugin_RemoveExtraEdges::BASE_SHAPE_ID());

  AttributeBooleanPtr anAttrUniteFaces =
    aBase->boolean(FeaturesPlugin_RemoveExtraEdges::UNITE_FACES_ID());

  theDumper << aBase << " = model.addRemoveExtraEdges(" << aPartName << ", "
            << anAttrBaseShape << ", " << anAttrUniteFaces << ")" << std::endl;
}

//==================================================================================================
RemoveExtraEdgesPtr addRemoveExtraEdges(const std::shared_ptr<ModelAPI_Document>& thePart,
                                        const ModelHighAPI_Selection& theBase,
                                        const bool theDoUniteFaces)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesAPI_RemoveExtraEdges::ID());
  return RemoveExtraEdgesPtr(new FeaturesAPI_RemoveExtraEdges(aFeature, theBase, theDoUniteFaces));
}
