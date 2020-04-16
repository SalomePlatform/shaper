// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "FeaturesAPI_RemoveResults.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>


//==================================================================================================
FeaturesAPI_RemoveResults::FeaturesAPI_RemoveResults(
  const std::shared_ptr<ModelAPI_Feature>& theFeature)
  : ModelHighAPI_Interface(theFeature)
{}

//==================================================================================================
FeaturesAPI_RemoveResults::FeaturesAPI_RemoveResults(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theResults)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setRemoved(theResults);
  }
}

//==================================================================================================
FeaturesAPI_RemoveResults::~FeaturesAPI_RemoveResults()
{}

//==================================================================================================
void FeaturesAPI_RemoveResults::setRemoved(
  const std::list<ModelHighAPI_Selection>& theResults)
{
  fillAttribute(theResults, myremoved);
}

//==================================================================================================
void FeaturesAPI_RemoveResults::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionListPtr anAttrResults =
    aBase->selectionList(FeaturesPlugin_RemoveResults::RESULTS_ID());

  theDumper << aBase << " = model.addRemoveResults("
            << aDocName << ", " << anAttrResults << ")" << std::endl;
}

//==================================================================================================
RemoveResultsPtr addRemoveResults(const std::shared_ptr<ModelAPI_Document>& thePart,
                                  const std::list<ModelHighAPI_Selection>& theResults)
{
  std::shared_ptr<ModelAPI_Feature> aFeature =
    thePart->addFeature(FeaturesAPI_RemoveResults::ID());
  return RemoveResultsPtr(new FeaturesAPI_RemoveResults(aFeature, theResults));
}
