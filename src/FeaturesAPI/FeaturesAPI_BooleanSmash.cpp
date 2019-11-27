// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include "FeaturesAPI_BooleanSmash.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_BooleanSmash::FeaturesAPI_BooleanSmash(
  const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_BooleanSmash::FeaturesAPI_BooleanSmash(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theMainObjects,
  const std::list<ModelHighAPI_Selection>& theToolObjects,
  const int theVersion)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theVersion, theFeature->integer(FeaturesPlugin_Boolean::VERSION_ID()));
    fillAttribute(theMainObjects, mymainObjects);
    fillAttribute(theToolObjects, mytoolObjects);

    execute(false);
  }
}

//==================================================================================================
FeaturesAPI_BooleanSmash::~FeaturesAPI_BooleanSmash()
{
}


//==================================================================================================
void FeaturesAPI_BooleanSmash::setMainObjects(
  const std::list<ModelHighAPI_Selection>& theMainObjects)
{
  fillAttribute(theMainObjects, mymainObjects);

  execute();
}

//==================================================================================================
void FeaturesAPI_BooleanSmash::setToolObjects(
  const std::list<ModelHighAPI_Selection>& theToolObjects)
{
  fillAttribute(theToolObjects, mytoolObjects);

  execute();
}

//==================================================================================================
void FeaturesAPI_BooleanSmash::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();

  theDumper << aBase << " = model.addSmash";

  const std::string& aDocName = theDumper.name(aBase->document());
  AttributeSelectionListPtr anObjects =
    aBase->selectionList(FeaturesPlugin_BooleanSmash::OBJECT_LIST_ID());
  AttributeSelectionListPtr aTools =
    aBase->selectionList(FeaturesPlugin_BooleanSmash::TOOL_LIST_ID());
  AttributeIntegerPtr aVersion =
    aBase->integer(FeaturesPlugin_BooleanSmash::VERSION_ID());

  theDumper << "(" << aDocName << ", " << anObjects << ", " << aTools;

  if (aVersion && aVersion->isInitialized() &&
      aVersion->value() == FeaturesPlugin_VersionedBoolean::THE_VERSION_1)
    theDumper << ", keepSubResults = True";

  theDumper << ")" << std::endl;
}

//==================================================================================================
BooleanSmashPtr addSmash(const std::shared_ptr<ModelAPI_Document>& thePart,
                         const std::list<ModelHighAPI_Selection>& theMainObjects,
                         const std::list<ModelHighAPI_Selection>& theToolObjects,
                         const bool keepSubResults)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_BooleanSmash::ID());
  int aVersion = keepSubResults ? FeaturesPlugin_VersionedBoolean::THE_VERSION_1
                                : FeaturesPlugin_VersionedBoolean::THE_VERSION_0;
  return BooleanSmashPtr(new FeaturesAPI_BooleanSmash(aFeature,
                                                      theMainObjects,
                                                      theToolObjects,
                                                      aVersion));
}
