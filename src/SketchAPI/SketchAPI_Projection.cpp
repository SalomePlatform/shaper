// Name   : SketchAPI_Projection.cpp
// Purpose: 
//
// History:
// 16/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "SketchAPI_Projection.h"
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Tools.h>
//--------------------------------------------------------------------------------------
SketchAPI_Projection::SketchAPI_Projection(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: SketchAPI_SketchEntity(theFeature)
{
  initialize();
}

SketchAPI_Projection::SketchAPI_Projection(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const ModelHighAPI_Selection & theExternalFeature )
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setExternalFeature(theExternalFeature);
  }
}

SketchAPI_Projection::~SketchAPI_Projection()
{

}

//--------------------------------------------------------------------------------------
void SketchAPI_Projection::setExternalFeature(const ModelHighAPI_Selection & theExternalFeature)
{
  fillAttribute(theExternalFeature, externalFeature());

  execute();
}

//--------------------------------------------------------------------------------------
