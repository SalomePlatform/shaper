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

#include "FeaturesAPI_BooleanFuse.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>


static const double DEFAULT_FUZZY = 1.e-5;


//==================================================================================================
FeaturesAPI_BooleanFuse::FeaturesAPI_BooleanFuse(
  const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_BooleanFuse::FeaturesAPI_BooleanFuse(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theMainObjects,
  const bool theRemoveEdges,
  const ModelHighAPI_Double& theFuzzy)
  : ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(FeaturesPlugin_BooleanFuse::CREATION_METHOD_SIMPLE(), VAR_NAME(creationMethod));
    fillAttribute(theMainObjects, VAR_NAME(mainObjects));
    fillAttribute(theRemoveEdges, VAR_NAME(removeEdges));
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
FeaturesAPI_BooleanFuse::FeaturesAPI_BooleanFuse(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theMainObjects,
  const std::list<ModelHighAPI_Selection>& theToolObjects,
  const bool theRemoveEdges,
  const ModelHighAPI_Double& theFuzzy)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(FeaturesPlugin_BooleanFuse::CREATION_METHOD_ADVANCED(), VAR_NAME(creationMethod));
    fillAttribute(theMainObjects, VAR_NAME(mainObjects));
    fillAttribute(theToolObjects, VAR_NAME(toolObjects));
    fillAttribute(theRemoveEdges, VAR_NAME(removeEdges));
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
FeaturesAPI_BooleanFuse::~FeaturesAPI_BooleanFuse()
{
}


//==================================================================================================
void FeaturesAPI_BooleanFuse::setMainObjects(
  const std::list<ModelHighAPI_Selection>& theMainObjects)
{
  fillAttribute(theMainObjects, VAR_NAME(mainObjects));

  execute();
}

//==================================================================================================
void FeaturesAPI_BooleanFuse::setToolObjects(
  const std::list<ModelHighAPI_Selection>& theToolObjects)
{
  fillAttribute(FeaturesPlugin_BooleanFuse::CREATION_METHOD_ADVANCED(), VAR_NAME(creationMethod));
  fillAttribute(theToolObjects, VAR_NAME(toolObjects));

  execute();
}

//==================================================================================================
void FeaturesAPI_BooleanFuse::setRemoveEdges(const bool theRemoveEdges)
{
  fillAttribute(theRemoveEdges, VAR_NAME(removeEdges));

  execute();
}

//==================================================================================================
void FeaturesAPI_BooleanFuse::setUseFuzzy(bool theUseFuzzy)
{
  fillAttribute(theUseFuzzy, VAR_NAME(useFuzzy));

  execute();
}

//==================================================================================================
void FeaturesAPI_BooleanFuse::setFuzzyValue(const ModelHighAPI_Double& theFuzzy)
{
  fillAttribute(theFuzzy, VAR_NAME(fuzzyParam));

  execute();
}

//==================================================================================================
void FeaturesAPI_BooleanFuse::setAdvancedMode(const bool theMode)
{
  if (theMode) {
    fillAttribute(FeaturesPlugin_BooleanFuse::CREATION_METHOD_ADVANCED(), VAR_NAME(creationMethod));
  } else {
    fillAttribute(FeaturesPlugin_BooleanFuse::CREATION_METHOD_SIMPLE(), VAR_NAME(creationMethod));
  }

  execute();
}

//==================================================================================================
void FeaturesAPI_BooleanFuse::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();

  theDumper << aBase << " = model.addFuse";

  const std::string& aDocName = theDumper.name(aBase->document());
  AttributeStringPtr aMode = aBase->string(FeaturesPlugin_BooleanFuse::CREATION_METHOD());
  AttributeSelectionListPtr anObjects =
    aBase->selectionList(FeaturesPlugin_BooleanFuse::OBJECT_LIST_ID());
  AttributeSelectionListPtr aTools =
    aBase->selectionList(FeaturesPlugin_BooleanFuse::TOOL_LIST_ID());
  AttributeBooleanPtr aRemoveEdges =
    aBase->boolean(FeaturesPlugin_BooleanFuse::REMOVE_INTERSECTION_EDGES_ID());
  bool aUseFuzzy = aBase->boolean(FeaturesPlugin_BooleanFuse::USE_FUZZY_ID())->value();
  AttributeDoublePtr aFuzzy = aBase->real(FeaturesPlugin_BooleanFuse::FUZZY_PARAM_ID());

  theDumper << "(" << aDocName << ", " << anObjects;

  if (aMode->value() == FeaturesPlugin_BooleanFuse::CREATION_METHOD_ADVANCED()) {
    theDumper << ", " << aTools;
  }

  if (aRemoveEdges->value()) {
    theDumper << ", removeEdges = True";
  }

  if (aUseFuzzy)
    theDumper << ", fuzzyParam = " << aFuzzy;

  if (!aBase->data()->version().empty())
    theDumper << ", keepSubResults = True";

  theDumper << ")" << std::endl;
}

//==================================================================================================
BooleanFusePtr addFuse(const std::shared_ptr<ModelAPI_Document>& thePart,
                       const std::list<ModelHighAPI_Selection>& theMainObjects,
                       const std::pair<std::list<ModelHighAPI_Selection>, bool>& theToolObjects,
                       const bool theRemoveEdges,
                       const ModelHighAPI_Double& theFuzzy,
                       const bool keepSubResults)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_BooleanFuse::ID());
  if (!keepSubResults)
    aFeature->data()->setVersion("");

  bool aRemoveEdges = theRemoveEdges;
  if (theToolObjects.first.empty())
    aRemoveEdges = aRemoveEdges || theToolObjects.second;

  BooleanFusePtr aFuse;
  if (theToolObjects.first.empty())
    aFuse.reset(new FeaturesAPI_BooleanFuse(aFeature, theMainObjects, aRemoveEdges, theFuzzy));
  else {
    aFuse.reset(new FeaturesAPI_BooleanFuse(aFeature, theMainObjects, theToolObjects.first,
                                            aRemoveEdges, theFuzzy));
  }
  return aFuse;
}
