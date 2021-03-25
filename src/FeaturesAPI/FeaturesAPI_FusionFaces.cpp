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

#include "FeaturesAPI_FusionFaces.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_FusionFaces::FeaturesAPI_FusionFaces(
  const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_FusionFaces::FeaturesAPI_FusionFaces(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const ModelHighAPI_Selection& theBase)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setBase(theBase);
  }
}

//==================================================================================================
FeaturesAPI_FusionFaces::~FeaturesAPI_FusionFaces()
{

}

//==================================================================================================
void FeaturesAPI_FusionFaces::setBase(const ModelHighAPI_Selection& theBase)
{
  fillAttribute(theBase, mybase);

  execute();
}

//==================================================================================================
void FeaturesAPI_FusionFaces::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aPartName = theDumper.name(aBase->document());

  AttributeSelectionPtr anAttrBaseShape =
    aBase->selection(FeaturesPlugin_FusionFaces::BASE_SHAPE_ID());

  theDumper << aBase << " = model.addFusionFaces("
            << aPartName << ", " << anAttrBaseShape << ")" << std::endl;
}

//==================================================================================================
FusionFacesPtr addFusionFaces(const std::shared_ptr<ModelAPI_Document>& thePart,
                              const ModelHighAPI_Selection& theBase)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesAPI_FusionFaces::ID());
  return FusionFacesPtr(new FeaturesAPI_FusionFaces(aFeature, theBase));
}
