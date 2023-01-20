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

#include "FeaturesAPI_Partition.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>


static const double DEFAULT_FUZZY = 1.e-5;


//==================================================================================================
FeaturesAPI_Partition::FeaturesAPI_Partition(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_Partition::FeaturesAPI_Partition(
    const std::shared_ptr<ModelAPI_Feature>& theFeature,
    const std::list<ModelHighAPI_Selection>& theBaseObjects,
    const ModelHighAPI_Double& theFuzzy)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    bool aUseFuzzy = (theFuzzy.value() > 0);
    ModelHighAPI_Double aFuzzy = (aUseFuzzy ? theFuzzy : ModelHighAPI_Double(DEFAULT_FUZZY));
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(aUseFuzzy, myuseFuzzy);
    fillAttribute(aFuzzy, myfuzzyParam);
    execute();
  }
}

//==================================================================================================
FeaturesAPI_Partition::~FeaturesAPI_Partition()
{

}

//==================================================================================================
void FeaturesAPI_Partition::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  fillAttribute(theBaseObjects, mybaseObjects);

  execute();
}

//==================================================================================================
void FeaturesAPI_Partition::setUseFuzzy(bool theUseFuzzy)
{
  fillAttribute(theUseFuzzy, myuseFuzzy);

  execute();
}

//==================================================================================================
void FeaturesAPI_Partition::setFuzzy(const ModelHighAPI_Double& theFuzzy)
{
  fillAttribute(theFuzzy, myfuzzyParam);

  execute();
}

//==================================================================================================
void FeaturesAPI_Partition::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionListPtr anAttrObjects =
    aBase->selectionList(FeaturesPlugin_Partition::BASE_OBJECTS_ID());
  bool aUseFuzzy = aBase->boolean(FeaturesPlugin_Partition::USE_FUZZY_ID())->value();
  AttributeDoublePtr aFuzzy = aBase->real(FeaturesPlugin_Partition::FUZZY_PARAM_ID());

  theDumper << aBase << " = model.addPartition(" << aDocName << ", " << anAttrObjects;

  if (aUseFuzzy)
    theDumper << ", fuzzyParam = " << aFuzzy;

  if (!aBase->data()->version().empty())
    theDumper << ", keepSubResults = True";

  theDumper << ")" << std::endl;
}

//==================================================================================================
PartitionPtr addPartition(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const std::list<ModelHighAPI_Selection>& theBaseObjects,
                          const ModelHighAPI_Double& fuzzyParam,
                          const bool keepSubResults)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Partition::ID());
  if (!keepSubResults)
    aFeature->data()->setVersion("");
  return PartitionPtr(new FeaturesAPI_Partition(aFeature, theBaseObjects, fuzzyParam));
}
