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

#include "FeaturesAPI_BooleanCut.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_BooleanCut::FeaturesAPI_BooleanCut(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_BooleanCut::FeaturesAPI_BooleanCut(
  const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<ModelHighAPI_Selection>& theMainObjects,
  const std::list<ModelHighAPI_Selection>& theToolObjects,
  const ModelHighAPI_Double& theFuzzy)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theMainObjects, mymainObjects);
    fillAttribute(theToolObjects, mytoolObjects);
    fillAttribute(theFuzzy, myfuzzyParam);

    execute(false);
  }
}

//==================================================================================================
FeaturesAPI_BooleanCut::~FeaturesAPI_BooleanCut()
{
}


//==================================================================================================
void FeaturesAPI_BooleanCut::setMainObjects(const std::list<ModelHighAPI_Selection>& theMainObjects)
{
  fillAttribute(theMainObjects, mymainObjects);

  execute();
}

//==================================================================================================
void FeaturesAPI_BooleanCut::setToolObjects(const std::list<ModelHighAPI_Selection>& theToolObjects)
{
  fillAttribute(theToolObjects, mytoolObjects);

  execute();
}

//==================================================================================================
void FeaturesAPI_BooleanCut::setFuzzyValue(const ModelHighAPI_Double& theFuzzy)
{
  fillAttribute(theFuzzy, myfuzzyParam);

  execute();
}

//==================================================================================================
void FeaturesAPI_BooleanCut::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();

  theDumper << aBase << " = model.addCut";

  const std::string& aDocName = theDumper.name(aBase->document());
  AttributeSelectionListPtr anObjects =
    aBase->selectionList(FeaturesPlugin_BooleanCut::OBJECT_LIST_ID());
  AttributeSelectionListPtr aTools =
    aBase->selectionList(FeaturesPlugin_BooleanCut::TOOL_LIST_ID());
  double aFuzzy = aBase->real(FeaturesPlugin_BooleanCut::FUZZY_PARAM_ID())->value();

  theDumper << "(" << aDocName << ", " << anObjects << ", " << aTools;

  theDumper << ", fuzzyParam = " << aFuzzy;

  if (!aBase->data()->version().empty())
    theDumper << ", keepSubResults = True";

  theDumper << ")" << std::endl;
}

//==================================================================================================
BooleanCutPtr addCut(const std::shared_ptr<ModelAPI_Document>& thePart,
                     const std::list<ModelHighAPI_Selection>& theMainObjects,
                     const std::list<ModelHighAPI_Selection>& theToolObjects,
                     const ModelHighAPI_Double& fuzzyParam,
                     const bool keepSubResults)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_BooleanCut::ID());
  if (!keepSubResults)
    aFeature->data()->setVersion("");
  return BooleanCutPtr(new FeaturesAPI_BooleanCut(aFeature, theMainObjects, theToolObjects, fuzzyParam));
}
