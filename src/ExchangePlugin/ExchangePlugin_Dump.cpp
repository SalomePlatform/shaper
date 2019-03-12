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

#include <ExchangePlugin_Dump.h>

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_ResultPart.h>

#include <ModelHighAPI_Dumper.h>

#include <Config_ModuleReader.h>


ExchangePlugin_Dump::ExchangePlugin_Dump()
{
}

ExchangePlugin_Dump::~ExchangePlugin_Dump()
{
}

void ExchangePlugin_Dump::initAttributes()
{
  data()->addAttribute(FILE_PATH_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(FILE_FORMAT_ID(), ModelAPI_AttributeString::typeId());

  data()->addAttribute(TOPOLOGICAL_NAMING_DUMP_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(GEOMETRIC_DUMP_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(WEAK_NAMING_DUMP_ID(), ModelAPI_AttributeBoolean::typeId());

  // default values
  boolean(TOPOLOGICAL_NAMING_DUMP_ID())->setValue(true);
  boolean(GEOMETRIC_DUMP_ID())->setValue(true);
  boolean(WEAK_NAMING_DUMP_ID())->setValue(false);
}

void ExchangePlugin_Dump::execute()
{
  AttributeStringPtr aFilePathAttr =
      this->string(ExchangePlugin_Dump::FILE_PATH_ID());
  std::string aFilePath = aFilePathAttr->value();
  if (aFilePath.empty())
    return;

  dump(aFilePath);
}

void ExchangePlugin_Dump::dump(const std::string& theFileName)
{
  // load DumpAssistant from Python side
  Config_ModuleReader::loadScript("salome.shaper.model.dump");

  DocumentPtr aDoc = ModelAPI_Session::get()->moduleDocument();

  int aFeaturesNb = aDoc->size(ModelAPI_Feature::group());
  if(aFeaturesNb > 1) {
    FeaturePtr aLastFeature =
        ModelAPI_Feature::feature(aDoc->object(ModelAPI_Feature::group(), aFeaturesNb - 1));
    if(aDoc->currentFeature(true) != aLastFeature) {
        setError("Dump cannot be done. Please move the history line to the end before dumping.");
        return;
    }
  }

  DocumentPtr anActiveDoc = ModelAPI_Session::get()->activeDocument();
  aFeaturesNb = anActiveDoc->size(ModelAPI_Feature::group());
  if(aFeaturesNb > 1) {
    FeaturePtr aLastFeature =
        ModelAPI_Feature::feature(anActiveDoc->object(ModelAPI_Feature::group(), aFeaturesNb - 1));
    if(anActiveDoc->currentFeature(true) != aLastFeature) {
      setError("Dump cannot be done. Please move the history line to the end before dumping.");
      return;
    }
  }

  std::list<FeaturePtr> aFeatures = aDoc->allFeatures();
  for(std::list<FeaturePtr>::const_iterator aFeatIt = aFeatures.begin();
      aFeatIt != aFeatures.end();
      ++aFeatIt) {
    ResultPartPtr aResultPart =
      std::dynamic_pointer_cast<ModelAPI_ResultPart>((*aFeatIt)->firstResult());
    if(aResultPart.get()) {
      if(!aResultPart->isActivated()) {
        setError("Error: Not all parts are loaded. Can not dump.");
        return;
      }
    }
  }

  // process selected types of the dump
  ModelHighAPI_Dumper* aDumper = ModelHighAPI_Dumper::getInstance();
  if (!aDumper)
    setError("An error occured while dumping to " + theFileName);

  static const int THE_TYPES_SIZE = 3;
  bool aTypes[THE_TYPES_SIZE] = {
    boolean(TOPOLOGICAL_NAMING_DUMP_ID())->value(),
    boolean(GEOMETRIC_DUMP_ID())->value(),
    boolean(WEAK_NAMING_DUMP_ID())->value()
  };
  int aNbSelectedTypes = 0;
  for (int i = 0; i < THE_TYPES_SIZE; ++i)
    if (aTypes[i])
      ++aNbSelectedTypes;

  if (boolean(TOPOLOGICAL_NAMING_DUMP_ID())->value()) {
    ModelHighAPI_Dumper::DumpStorage aTopoNameStorage;
    aDumper->addCustomStorage(aTopoNameStorage);
  }
  if (boolean(GEOMETRIC_DUMP_ID())->value()) {
    ModelHighAPI_Dumper::DumpStorageGeom aGeomSelectionStorage;
    if (aNbSelectedTypes > 1)
      aGeomSelectionStorage.setFilenameSuffix("_geo");
    aDumper->addCustomStorage(aGeomSelectionStorage);
  }
  if (boolean(WEAK_NAMING_DUMP_ID())->value()) {
    ModelHighAPI_Dumper::DumpStorageWeak aWeakNamingStorage;
    if (aNbSelectedTypes > 1)
      aWeakNamingStorage.setFilenameSuffix("_weak");
    aDumper->addCustomStorage(aWeakNamingStorage);
  }

  if (!aDumper->process(aDoc, theFileName))
    setError("An error occured while dumping to " + theFileName);
}
