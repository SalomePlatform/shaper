// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
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
    fillAttribute(theObjects, translationList());
    fillAttribute(thePoint1, startPoint());
    fillAttribute(thePoint2, endPoint());
    fillAttribute(theNumberOfObjects, numberOfObjects());
    fillAttribute(theFullValue ? "FullValue" : "SingleValue", valueType());

    execute(true);
  }
}

SketchAPI_Translation::~SketchAPI_Translation()
{

}

std::list<std::shared_ptr<SketchAPI_SketchEntity> > SketchAPI_Translation::translated() const
{
  std::list<ObjectPtr> aList = translatedObjects()->list();
  // remove all initial features
  std::list<FeaturePtr> anIntermediate;
  std::list<ObjectPtr>::const_iterator anIt = aList.begin();
  for (; anIt != aList.end(); ++anIt) {
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

  theDumper << aBase << " = " << aSketchName << ".addTranslation("
            << aTransObjects << ", " << aStart << ", " << aEnd << ", " << aNbCopies;
  if (isFullValue)
    theDumper << ", " << isFullValue;
  theDumper << ")" << std::endl;

  // Dump variables for a list of translated features
  theDumper << "[";
  std::list<std::shared_ptr<SketchAPI_SketchEntity> > aList = translated();
  std::list<std::shared_ptr<SketchAPI_SketchEntity> >::const_iterator anIt = aList.begin();
  for (; anIt != aList.end(); ++anIt) {
    if (anIt != aList.begin())
      theDumper << ", ";
    theDumper << (*anIt)->feature();
  }
  theDumper << "] = " << theDumper.name(aBase) << ".translated()" << std::endl;

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
                  << ".setAuxiliary(" << aFeatAux << ")" <<std::endl;
    }
  }
}
