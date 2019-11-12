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

#include "ExchangeAPI_Import.h"
//--------------------------------------------------------------------------------------
#include <ExchangePlugin_ImportPart.h>
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Services.h>
#include <ModelHighAPI_Tools.h>
//--------------------------------------------------------------------------------------
#include <algorithm>

ExchangeAPI_Import::ExchangeAPI_Import(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

ExchangeAPI_Import::ExchangeAPI_Import(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const std::string & theFilePath)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize())
    setFilePath(theFilePath);
}

ExchangeAPI_Import::~ExchangeAPI_Import()
{

}

//--------------------------------------------------------------------------------------
void ExchangeAPI_Import::setFilePath(const std::string & theFilePath)
{
  fillAttribute(theFilePath, myfilePath);

  execute();
}

//--------------------------------------------------------------------------------------
void ExchangeAPI_Import::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  std::string aPartName = theDumper.name(aBase->document());

  std::string aFilePath = aBase->string(ExchangePlugin_ImportFeature::FILE_PATH_ID())->value();
  std::string aFrom = "\\";
  std::string aTo = "\\\\";
  for(std::size_t aPos = aFilePath.find(aFrom);
      aPos != std::string::npos;
      aPos = aFilePath.find(aFrom, aPos)) {
    aFilePath.replace(aPos, aFrom.size(), aTo);
    aPos += aTo.size();
  }

  theDumper << aBase << " = model.addImport(" << aPartName << ", \""
            << aFilePath << "\")" << std::endl;
  // to make import have results
  theDumper << "model.do()" << std::endl;

  CompositeFeaturePtr aCompositeFeature =
    std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aBase);
  if(aCompositeFeature.get()) {
    int aNbOfSubs = aCompositeFeature->numberOfSubs();
    for(int anIndex = 0; anIndex < aNbOfSubs; ++anIndex) {
      std::string aSubFeatureGet =
        theDumper.name(aBase) + ".subFeature(" + std::to_string((long long)anIndex) + ")";
      theDumper.dumpSubFeatureNameAndColor(aSubFeatureGet, aCompositeFeature->subFeature(anIndex));
    }
  }
}

//--------------------------------------------------------------------------------------
ImportPtr addImport(
    const std::shared_ptr<ModelAPI_Document> & thePart,
    const std::string & theFilePath)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ExchangeAPI_Import::ID());
  return ImportPtr(new ExchangeAPI_Import(aFeature, theFilePath));
}

void importPart(const std::shared_ptr<ModelAPI_Document> & thePart,
                const std::string & theFilePath,
                const ModelHighAPI_Reference & theAfterThis)
{
  static const bool THE_VISIBLE_FEATURE = false;
  FeaturePtr aCurrentFeature;
  if (theAfterThis.feature()) {
    aCurrentFeature = thePart->currentFeature(THE_VISIBLE_FEATURE);
    thePart->setCurrentFeature(theAfterThis.feature(), THE_VISIBLE_FEATURE);
  }

  FeaturePtr aFeature = thePart->addFeature(ExchangePlugin_ImportPart::ID());
  aFeature->string(ExchangePlugin_ImportPart::FILE_PATH_ID())->setValue(theFilePath);
  // restart transaction to execute and delete the macro-feature
  apply();

  // restore current feature
  if (aCurrentFeature)
    thePart->setCurrentFeature(aCurrentFeature, THE_VISIBLE_FEATURE);
}
