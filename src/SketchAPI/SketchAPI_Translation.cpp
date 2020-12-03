// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "SketchAPI_Translation.h"
#include <SketchAPI_SketchEntity.h>
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

#include <SketchPlugin_SketchEntity.h>
//--------------------------------------------------------------------------------------
SketchAPI_Translation::SketchAPI_Translation(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

SketchAPI_Translation::SketchAPI_Translation(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const std::list<std::shared_ptr<ModelAPI_Object> > & theObjects,
    const ModelHighAPI_RefAttr & thePoint1,
    const ModelHighAPI_RefAttr & thePoint2,
    const ModelHighAPI_Integer & theNumberOfObjects,
    bool theFullValue)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(thePoint1, startPoint());
    fillAttribute(thePoint2, endPoint());
    fillAttribute(theNumberOfObjects, numberOfObjects());
    fillAttribute(theFullValue ? "FullValue" : "SingleValue", valueType());

    setTranslationList(theObjects);
  }
}

SketchAPI_Translation::~SketchAPI_Translation()
{
}

void SketchAPI_Translation::setTranslationList(
    const std::list<std::shared_ptr<ModelAPI_Object> >& theObjects)
{
  fillAttribute(theObjects, translationList());
  execute(true);
}

std::list<std::shared_ptr<SketchAPI_SketchEntity> > SketchAPI_Translation::translated() const
{
  std::list<ObjectPtr> aList = translationList()->list();
  std::set<ObjectPtr> anOriginalObjects;
  anOriginalObjects.insert(aList.begin(), aList.end());
  // remove all initial features
  std::list<FeaturePtr> anIntermediate;
  aList = translatedObjects()->list();
  std::list<ObjectPtr>::const_iterator anIt = aList.begin();
  for (; anIt != aList.end(); ++anIt) {
    if (anOriginalObjects.find(*anIt) != anOriginalObjects.end())
      continue; // skip initial object
    FeaturePtr aFeature = ModelAPI_Feature::feature(*anIt);
    AttributeBooleanPtr isCopy = aFeature->boolean(SketchPlugin_SketchEntity::COPY_ID());
    if (isCopy.get() && isCopy->value())
      anIntermediate.push_back(aFeature);
  }
  return SketchAPI_SketchEntity::wrap(anIntermediate);
}

//--------------------------------------------------------------------------------------

void SketchAPI_Translation::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aSketchName = theDumper.parentName(aBase);

  AttributeRefListPtr aTransObjects = translationList();
  AttributeRefAttrPtr aStart = startPoint();
  AttributeRefAttrPtr aEnd   = endPoint();
  AttributeIntegerPtr aNbCopies = numberOfObjects();
  bool isFullValue = valueType()->value() != "SingleValue";

  // Check all attributes are already dumped. If not, store the constraint as postponed.
  size_t aFirstNotDumped = theDumper.indexOfFirstNotDumped(aTransObjects);
  if (!theDumper.isDumped(aStart) || !theDumper.isDumped(aEnd) || aFirstNotDumped == 0) {
    theDumper.postpone(aBase);
    return;
  }

  // the number of dumped aTransObjects is not changed, no need to dump anything
  static std::map<FeaturePtr, size_t> aNbDumpedArguments;
  std::map<FeaturePtr, size_t>::iterator aFound = aNbDumpedArguments.find(aBase);
  if (aFound != aNbDumpedArguments.end() && aFound->second == aFirstNotDumped) {
    theDumper.postpone(aBase);
    return;
  }
  else
    aNbDumpedArguments[aBase] = aFirstNotDumped;

  if (theDumper.isDumped(aBase)) {
    // the feature is already dumped, but it was postponed, because of some arguments
    // were not dumped yet, thus, it is necessary to update the list of rotated objects
    theDumper << "\n### Update " << aBase->getKind() << std::endl;
    theDumper << aBase << ".setTranslationList(" << aTransObjects << ")" << std::endl;
  }
  else {
    // the feature is not dumped yet, make the full dump
    theDumper << aBase << " = " << aSketchName << ".addTranslation("
              << aTransObjects << ", " << aStart << ", " << aEnd << ", " << aNbCopies;
    if (isFullValue)
      theDumper << ", " << isFullValue;
    theDumper << ")" << std::endl;
  }

  // Dump variables for a list of translated features
  theDumper << "[";
  std::list<std::shared_ptr<SketchAPI_SketchEntity> > aList = translated();
  std::list<std::shared_ptr<SketchAPI_SketchEntity> >::const_iterator anIt = aList.begin();
  for (size_t anIndex = 0; anIndex < aFirstNotDumped; ++anIndex)
    for (int i = 1; i < aNbCopies->value() && anIt != aList.end(); ++i, ++anIt) {
      if (anIt != aList.begin())
        theDumper << ", ";
      theDumper << (*anIt)->feature();
    }
  theDumper << "] = " << theDumper.name(aBase) << ".translated()" << std::endl;

  if (theDumper.isDumped(aTransObjects)) {
    aNbDumpedArguments.erase(aBase);
    // Set necessary "auxiliary" flag for translated features
    // (flag is set if it differs to base entity)
    std::list<ObjectPtr> aTransList = aTransObjects->list();
    std::list<ObjectPtr>::const_iterator aTrIt = aTransList.begin();
    anIt = aList.begin();
    for (; aTrIt != aTransList.end(); ++aTrIt) {
      FeaturePtr aFeature = ModelAPI_Feature::feature(*aTrIt);
      if (!aFeature)
        continue;
      bool aBaseAux = aFeature->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->value();

      for (int i = 1; i < aNbCopies->value(); ++i, ++anIt) {
        aFeature = (*anIt)->feature();
        bool aFeatAux = aFeature->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->value();
        if (aFeatAux != aBaseAux)
          theDumper << theDumper.name((*anIt)->feature(), false)
                    << ".setAuxiliary(" << aFeatAux << ")" << std::endl;
      }
    }
  }
  else {
    // If all refereced objects are not dumped yet, mark the feature as postponed.
    theDumper.postpone(aBase);
  }
}
