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

#include "FeaturesAPI_Union.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>


static const double DEFAULT_FUZZY = 1.e-5;


//================================================================================================
FeaturesAPI_Union::FeaturesAPI_Union(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//================================================================================================
FeaturesAPI_Union::FeaturesAPI_Union(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                     const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                     const ModelHighAPI_Double& theFuzzy)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, VAR_NAME(baseObjects));
    fillAttribute(theFuzzy, VAR_NAME(fuzzyParam));

    // Get the evaluated fuzzy parameter from the attribute!!
    bool aUseFuzzy = (fuzzyParam()->value() > 0);
    fillAttribute(aUseFuzzy, VAR_NAME(useFuzzy));
    if (!aUseFuzzy) {
      ModelHighAPI_Double aFuzzy(DEFAULT_FUZZY);
      fillAttribute(aFuzzy, VAR_NAME(fuzzyParam));
    }

    execute();
  }
}

//================================================================================================
FeaturesAPI_Union::~FeaturesAPI_Union()
{

}

//==================================================================================================
void FeaturesAPI_Union::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  fillAttribute(theBaseObjects, VAR_NAME(baseObjects));

  execute();
}

//==================================================================================================
void FeaturesAPI_Union::setUseFuzzy(bool theUseFuzzy)
{
  fillAttribute(theUseFuzzy, VAR_NAME(useFuzzy));

  execute();
}

//==================================================================================================
void FeaturesAPI_Union::setFuzzyValue(const ModelHighAPI_Double& theFuzzy)
{
  fillAttribute(theFuzzy, VAR_NAME(fuzzyParam));

  execute();
}

//==================================================================================================
void FeaturesAPI_Union::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionListPtr anAttrObjects =
    aBase->selectionList(FeaturesPlugin_Union::BASE_OBJECTS_ID());
  bool aUseFuzzy = aBase->boolean(FeaturesPlugin_Union::USE_FUZZY_ID())->value();
  AttributeDoublePtr aFuzzy = aBase->real(FeaturesPlugin_Union::FUZZY_PARAM_ID());

  theDumper << aBase << " = model.addUnion(" << aDocName << ", " << anAttrObjects;

  if (aUseFuzzy)
    theDumper << ", fuzzyParam = " << aFuzzy;

  if (!aBase->data()->version().empty())
    theDumper << ", keepSubResults = True";

  theDumper << ")" << std::endl;
}

//==================================================================================================
UnionPtr addUnion(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                  const ModelHighAPI_Double& theFuzzy,
                  const bool keepSubResults)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Union::ID());
  if (!keepSubResults)
    aFeature->data()->setVersion("");
  return UnionPtr(new FeaturesAPI_Union(aFeature, theBaseObjects, theFuzzy));
}
