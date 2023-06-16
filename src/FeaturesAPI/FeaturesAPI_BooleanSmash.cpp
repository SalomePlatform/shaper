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

#include "FeaturesAPI_BooleanSmash.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>


static const double DEFAULT_FUZZY = 1.e-5;


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
  const ModelHighAPI_Double& theFuzzy)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theMainObjects, VAR_NAME(mainObjects));
    fillAttribute(theToolObjects, VAR_NAME(toolObjects));
    fillAttribute(theFuzzy, VAR_NAME(fuzzyParam));

    // Get the evaluated fuzzy parameter from the attribute!!
    bool aUseFuzzy = (fuzzyParam()->value() > 0);
    fillAttribute(aUseFuzzy, VAR_NAME(useFuzzy));
    if (!aUseFuzzy) {
      ModelHighAPI_Double aFuzzy(DEFAULT_FUZZY);
      fillAttribute(aFuzzy, VAR_NAME(fuzzyParam));
    }

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
  fillAttribute(theMainObjects, VAR_NAME(mainObjects));

  execute();
}

//==================================================================================================
void FeaturesAPI_BooleanSmash::setToolObjects(
  const std::list<ModelHighAPI_Selection>& theToolObjects)
{
  fillAttribute(theToolObjects, VAR_NAME(toolObjects));

  execute();
}

//==================================================================================================
void FeaturesAPI_BooleanSmash::setUseFuzzy(bool theUseFuzzy)
{
  fillAttribute(theUseFuzzy, VAR_NAME(useFuzzy));

  execute();
}

//==================================================================================================
void FeaturesAPI_BooleanSmash::setFuzzyValue(const ModelHighAPI_Double& theFuzzy)
{
  fillAttribute(theFuzzy, VAR_NAME(fuzzyParam));

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
  bool aUseFuzzy = aBase->boolean(FeaturesPlugin_BooleanSmash::USE_FUZZY_ID())->value();
  AttributeDoublePtr aFuzzy = aBase->real(FeaturesPlugin_BooleanSmash::FUZZY_PARAM_ID());

  theDumper << "(" << aDocName << ", " << anObjects << ", " << aTools;

  if (aUseFuzzy)
    theDumper << ", fuzzyParam = " << aFuzzy;

  if (!aBase->data()->version().empty())
    theDumper << ", keepSubResults = True";

  theDumper << ")" << std::endl;
}

//==================================================================================================
BooleanSmashPtr addSmash(const std::shared_ptr<ModelAPI_Document>& thePart,
                         const std::list<ModelHighAPI_Selection>& theMainObjects,
                         const std::list<ModelHighAPI_Selection>& theToolObjects,
                         const ModelHighAPI_Double& theFuzzy,
                         const bool keepSubResults)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_BooleanSmash::ID());
  if (!keepSubResults)
    aFeature->data()->setVersion("");
  return BooleanSmashPtr(new FeaturesAPI_BooleanSmash(aFeature, theMainObjects, theToolObjects, theFuzzy));
}
