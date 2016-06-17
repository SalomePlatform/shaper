// Name   : SketchAPI_Rotation.cpp
// Purpose: 
//
// History:
// 16/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "SketchAPI_Rotation.h"
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Tools.h>
//--------------------------------------------------------------------------------------
SketchAPI_Rotation::SketchAPI_Rotation(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: SketchAPI_SketchEntity(theFeature)
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
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    fillAttribute(theObjects, rotationList());
    fillAttribute(theCenter, center());
    fillAttribute(theAngle, angle());
    fillAttribute(theNumberOfObjects, numberOfObjects());
    if (theFullValue)
      fillAttribute("SingleAngle", valueType());

    execute();
  }
}

SketchAPI_Rotation::~SketchAPI_Rotation()
{

}

//--------------------------------------------------------------------------------------
