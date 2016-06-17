// Name   : SketchAPI_Translation.cpp
// Purpose: 
//
// History:
// 16/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "SketchAPI_Translation.h"
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Tools.h>
//--------------------------------------------------------------------------------------
SketchAPI_Translation::SketchAPI_Translation(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: SketchAPI_SketchEntity(theFeature)
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
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    fillAttribute(theObjects, translationList());
    fillAttribute(thePoint1, startPoint());
    fillAttribute(thePoint2, endPoint());
    fillAttribute(theNumberOfObjects, numberOfObjects());
    if (theFullValue)
      fillAttribute("SingleValue", valueType());

    execute();
  }
}

SketchAPI_Translation::~SketchAPI_Translation()
{

}

//--------------------------------------------------------------------------------------
