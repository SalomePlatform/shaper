// Name   : SketchAPI_Rotation.cpp
// Purpose: 
//
// History:
// 16/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "SketchAPI_Rotation.h"
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>
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

    execute();
  }
}

SketchAPI_Rotation::~SketchAPI_Rotation()
{

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
}
