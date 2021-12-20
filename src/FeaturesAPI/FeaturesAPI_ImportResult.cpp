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

#include "FeaturesAPI_ImportResult.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//================================================================================================
FeaturesAPI_ImportResult::FeaturesAPI_ImportResult(
  const std::shared_ptr<ModelAPI_Feature>& theFeature) : ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//================================================================================================
FeaturesAPI_ImportResult::FeaturesAPI_ImportResult(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theObjects) : ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setObjects(theObjects);
  }
}

//=================================================================================================
FeaturesAPI_ImportResult::~FeaturesAPI_ImportResult() {}

//=================================================================================================
void FeaturesAPI_ImportResult::setObjects(const std::list<ModelHighAPI_Selection>& theObjects)
{
  fillAttribute(theObjects, myobjects);
  execute();
}

//=================================================================================================
void FeaturesAPI_ImportResult::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());
  AttributeSelectionListPtr anObjects =
    aBase->selectionList(FeaturesPlugin_ImportResult::OBJECTS());

  CompositeFeaturePtr aCompositeFeature =
      std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aBase);
  int aNbOfSubs = aCompositeFeature->numberOfSubs();
  for (int anIndex = 0; anIndex < aNbOfSubs; anIndex++) {
    FeaturePtr aSubFeature = aCompositeFeature->subFeature(anIndex);
    theDumper.name(aSubFeature, false, false, true); //mark as not to dump
  }

  theDumper << aBase << " = model.addImportResult("
            << aDocName << ", " << anObjects << ")" << std::endl;
}

//=================================================================================================
ImportResultPtr addImportResult(const std::shared_ptr<ModelAPI_Document>& thePart,
                                const std::list<ModelHighAPI_Selection>& theObjects)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_ImportResult::ID());
  return ImportResultPtr(new FeaturesAPI_ImportResult(aFeature, theObjects));
}
