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

#include "FeaturesAPI_Recover.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Reference.h>
#include <ModelHighAPI_Tools.h>

//=================================================================================================
FeaturesAPI_Recover::FeaturesAPI_Recover(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//=================================================================================================
FeaturesAPI_Recover::FeaturesAPI_Recover(const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const ModelHighAPI_Reference& theBaseFeature,
  const std::list<ModelHighAPI_Selection>& theRecoveredList,
  const bool theRecoverCompound)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    std::string aMethod = theRecoverCompound ? FeaturesPlugin_Recover::METHOD_COMPOUND()
                                             : FeaturesPlugin_Recover::METHOD_DEFAULT();
    fillAttribute(aMethod, theFeature->string(FeaturesPlugin_Recover::METHOD()));

    setBaseFeature(theBaseFeature);
    setRecoveredList(theRecoveredList);
  }
}

//=================================================================================================
FeaturesAPI_Recover::~FeaturesAPI_Recover()
{}

//=================================================================================================
void FeaturesAPI_Recover::setBaseFeature(const ModelHighAPI_Reference& theBaseFeature)
{

  fillAttribute(theBaseFeature.feature(), mybaseFeature);
  execute();
}

//=================================================================================================
void FeaturesAPI_Recover::setRecoveredList(const std::list<ModelHighAPI_Selection>& theRecoverList)
{
  fillAttribute(theRecoverList, myrecoveredList);
  execute();
}

//==================================================================================================
void FeaturesAPI_Recover::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeReferencePtr anAttrBaseFeature =
    aBase->reference(FeaturesPlugin_Recover::BASE_FEATURE());
  AttributeRefListPtr anAttrRecoveredEntities =
    aBase->reflist(FeaturesPlugin_Recover::RECOVERED_ENTITIES());

  FeaturePtr aFeature = ModelAPI_Feature::feature(anAttrBaseFeature->value());

  theDumper << aBase << " = model.addRecover(" << aDocName << ", "
            << aFeature << ", " << anAttrRecoveredEntities;

  AttributeStringPtr aMethod = aBase->string(FeaturesPlugin_Recover::METHOD());
  if (aMethod && aMethod->isInitialized() &&
      aMethod->value() != FeaturesPlugin_Recover::METHOD_DEFAULT())
    theDumper << ", " << true;

  theDumper << ")" << std::endl;
}

//=================================================================================================
RecoverPtr addRecover(const std::shared_ptr<ModelAPI_Document>& thePart,
  const ModelHighAPI_Reference& theBaseFeature,
  const std::list<ModelHighAPI_Selection>& theRecoveredList, const bool theRecoverCompound)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Recover::ID());
  return RecoverPtr(new FeaturesAPI_Recover(
    aFeature, theBaseFeature, theRecoveredList, theRecoverCompound));
}
