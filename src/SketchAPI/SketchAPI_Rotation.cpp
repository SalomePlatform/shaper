// Copyright (C) 2014-20xx CEA/DEN, EDF R&D
// Name   : SketchAPI_Rotation.cpp
// Purpose: 
//
// History:
// 16/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "SketchAPI_Rotation.h"
#include <SketchAPI_SketchEntity.h>
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

#include <SketchPlugin_SketchEntity.h>
//--------------------------------------------------------------------------------------
SketchAPI_Rotation::SketchAPI_Rotation(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

SketchAPI_Rotation::SketchAPI_Rotation(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const std::list<std::shared_ptr<ModelAPI_Object> > & theObjects,
    const ModelHighAPI_RefAttr & theCenter,
    const ModelHighAPI_Double & theAngle,
    const ModelHighAPI_Integer & theNumberOfObjects,
    bool theFullValue)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(theObjects, rotationList());
    fillAttribute(theCenter, center());
    fillAttribute(theAngle, angle());
    fillAttribute(theNumberOfObjects, numberOfObjects());
    fillAttribute(theFullValue ? "FullAngle" : "SingleAngle", valueType());

    execute(true);
  }
}

SketchAPI_Rotation::~SketchAPI_Rotation()
{

}

std::list<std::shared_ptr<ModelHighAPI_Interface> > SketchAPI_Rotation::rotated() const
{
  std::list<ObjectPtr> aList = rotatedObjects()->list();
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

void SketchAPI_Rotation::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aSketchName = theDumper.parentName(aBase);

  AttributeRefListPtr aRotObjects = rotationList();
  AttributeRefAttrPtr aCenter = center();
  AttributeDoublePtr anAngle = angle();
  AttributeIntegerPtr aNbCopies = numberOfObjects();
  bool isFullValue = valueType()->value() != "SingleAngle";

  theDumper << aBase << " = " << aSketchName << ".addRotation("
            << aRotObjects << ", " << aCenter << ", " << anAngle << ", " << aNbCopies;
  if (isFullValue)
    theDumper << ", " << isFullValue;
  theDumper << ")" << std::endl;

  // Dump variables for a list of rotated features
  theDumper << "[";
  std::list<std::shared_ptr<ModelHighAPI_Interface> > aList = rotated();
  std::list<std::shared_ptr<ModelHighAPI_Interface> >::const_iterator anIt = aList.begin();
  for (; anIt != aList.end(); ++anIt) {
    if (anIt != aList.begin())
      theDumper << ", ";
    theDumper << theDumper.name((*anIt)->feature(), false);
  }
  theDumper << "] = " << theDumper.name(aBase) << ".rotated()" << std::endl;
}
