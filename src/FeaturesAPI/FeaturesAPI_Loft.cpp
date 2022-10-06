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

#include "FeaturesAPI_Loft.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_Loft::FeaturesAPI_Loft(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_Loft::FeaturesAPI_Loft(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                   const ModelHighAPI_Selection& theFirstObject,
                                   const ModelHighAPI_Selection& theSecondObject)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theFirstObject, myfisrstObject);
    fillAttribute(theSecondObject, mysecondObject);
    execute();
  }
}

//==================================================================================================
FeaturesAPI_Loft::~FeaturesAPI_Loft()
{

}

//==================================================================================================
void FeaturesAPI_Loft::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionPtr anAttrFirstObject =
                                    aBase->selection(FeaturesPlugin_Loft::FIRST_OBJECT_ID());
  AttributeSelectionPtr anAttrSecondObject =
                                    aBase->selection(FeaturesPlugin_Loft::SECOND_OBJECT_ID());

  theDumper << aBase << " = model.addLoft(" << aDocName << ", "
            << anAttrFirstObject << ", " << anAttrSecondObject;

  theDumper << ")" << std::endl;
}

//==================================================================================================
LoftPtr addLoft(const std::shared_ptr<ModelAPI_Document>& thePart,
                const ModelHighAPI_Selection& theFirstObject,
                const ModelHighAPI_Selection& theSecondObject)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Loft::ID());

  return LoftPtr(new FeaturesAPI_Loft(aFeature, theFirstObject, theSecondObject));
}

