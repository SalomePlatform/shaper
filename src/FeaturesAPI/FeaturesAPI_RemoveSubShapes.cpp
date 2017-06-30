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

#include "FeaturesAPI_RemoveSubShapes.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_RemoveSubShapes::FeaturesAPI_RemoveSubShapes(
  const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_RemoveSubShapes::FeaturesAPI_RemoveSubShapes(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const ModelHighAPI_Selection& theBase)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setBase(theBase);

    execute();
  }
}

//==================================================================================================
FeaturesAPI_RemoveSubShapes::~FeaturesAPI_RemoveSubShapes()
{

}

//==================================================================================================
void FeaturesAPI_RemoveSubShapes::setBase(const ModelHighAPI_Selection& theBase)
{
  fillAttribute(theBase, mybase);

  execute();
}

//==================================================================================================
void FeaturesAPI_RemoveSubShapes::setSubShapesToKeep(
  const std::list<ModelHighAPI_Selection>& theSubShapes)
{
  fillAttribute(theSubShapes, mysubshapes);

  execute();
}

//==================================================================================================
void FeaturesAPI_RemoveSubShapes::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionPtr anAttrBaseShape =
    aBase->selection(FeaturesPlugin_RemoveSubShapes::BASE_SHAPE_ID());
  AttributeSelectionListPtr anAttrSubShapes =
    aBase->selectionList(FeaturesPlugin_RemoveSubShapes::SUBSHAPES_ID());

  theDumper << aBase << " = model.addRemoveSubShapes(" << aDocName <<
    ", " << anAttrBaseShape << ")" << std::endl;
  theDumper << aBase << ".setSubShapesToKeep(" << anAttrSubShapes << ")" << std::endl;
}

//==================================================================================================
RemoveSubShapesPtr addRemoveSubShapes(const std::shared_ptr<ModelAPI_Document>& thePart,
                                      const ModelHighAPI_Selection& theBase)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesAPI_RemoveSubShapes::ID());
  return RemoveSubShapesPtr(new FeaturesAPI_RemoveSubShapes(aFeature, theBase));
}
