// Name   : SketchAPI_Translation.cpp
// Purpose: 
//
// History:
// 16/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "SketchAPI_Translation.h"
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>
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

    execute();
  }
}

SketchAPI_Translation::~SketchAPI_Translation()
{

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
}
