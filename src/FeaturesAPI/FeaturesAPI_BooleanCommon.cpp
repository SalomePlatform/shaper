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

#include "FeaturesAPI_BooleanCommon.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_BooleanCommon::FeaturesAPI_BooleanCommon(
  const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_BooleanCommon::FeaturesAPI_BooleanCommon(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theMainObjects)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(FeaturesPlugin_BooleanCommon::CREATION_METHOD_SIMPLE(), mycreationMethod);
    fillAttribute(theMainObjects, mymainObjects);

    execute(false);
  }
}

//==================================================================================================
FeaturesAPI_BooleanCommon::FeaturesAPI_BooleanCommon(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theMainObjects,
  const std::list<ModelHighAPI_Selection>& theToolObjects)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(FeaturesPlugin_BooleanCommon::CREATION_METHOD_ADVANCED(), mycreationMethod);
    fillAttribute(theMainObjects, mymainObjects);
    fillAttribute(theToolObjects, mytoolObjects);

    execute(false);
  }
}

//==================================================================================================
FeaturesAPI_BooleanCommon::~FeaturesAPI_BooleanCommon()
{
}


//==================================================================================================
void FeaturesAPI_BooleanCommon::setMainObjects(
  const std::list<ModelHighAPI_Selection>& theMainObjects)
{
  fillAttribute(theMainObjects, mymainObjects);

  execute();
}

//==================================================================================================
void FeaturesAPI_BooleanCommon::setToolObjects(
  const std::list<ModelHighAPI_Selection>& theToolObjects)
{
  fillAttribute(FeaturesPlugin_BooleanCommon::CREATION_METHOD_ADVANCED(), mycreationMethod);
  fillAttribute(theToolObjects, mytoolObjects);

  execute();
}

//==================================================================================================
void FeaturesAPI_BooleanCommon::setAdvancedMode(const bool theMode)
{
  if (theMode) {
    fillAttribute(FeaturesPlugin_BooleanCommon::CREATION_METHOD_ADVANCED(), mycreationMethod);
  } else {
    fillAttribute(FeaturesPlugin_BooleanCommon::CREATION_METHOD_SIMPLE(), mycreationMethod);
  }

  execute();
}

//==================================================================================================
void FeaturesAPI_BooleanCommon::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();

  theDumper << aBase << " = model.addCommon";

  const std::string& aDocName = theDumper.name(aBase->document());
  AttributeStringPtr aMode = aBase->string(FeaturesPlugin_BooleanCommon::CREATION_METHOD());
  AttributeSelectionListPtr anObjects =
    aBase->selectionList(FeaturesPlugin_BooleanCommon::OBJECT_LIST_ID());
  AttributeSelectionListPtr aTools =
    aBase->selectionList(FeaturesPlugin_BooleanCommon::TOOL_LIST_ID());

  theDumper << "(" << aDocName << ", " << anObjects;

  if (aMode->value() == FeaturesPlugin_BooleanCommon::CREATION_METHOD_ADVANCED()) {
    theDumper << ", " << aTools;
  }

  if (!aBase->data()->version().empty())
    theDumper << ", keepSubResults = True";

  theDumper << ")" << std::endl;
}

//==================================================================================================
BooleanCommonPtr addCommon(const std::shared_ptr<ModelAPI_Document>& thePart,
                           const std::list<ModelHighAPI_Selection>& theMainObjects,
                           const std::list<ModelHighAPI_Selection>& theToolObjects,
                           const bool keepSubResults)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_BooleanCommon::ID());
  if (!keepSubResults)
    aFeature->data()->setVersion("");
  BooleanCommonPtr aCommon;
  if (theToolObjects.empty())
    aCommon.reset(new FeaturesAPI_BooleanCommon(aFeature, theMainObjects));
  else
    aCommon.reset(new FeaturesAPI_BooleanCommon(aFeature, theMainObjects, theToolObjects));
  return aCommon;
}
