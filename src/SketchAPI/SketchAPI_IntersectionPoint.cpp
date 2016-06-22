// Name   : SketchAPI_IntersectionPoint.cpp
// Purpose: 
//
// History:
// 16/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "SketchAPI_IntersectionPoint.h"
//--------------------------------------------------------------------------------------
#include <GeomAPI_Pnt2d.h>
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>
//--------------------------------------------------------------------------------------
SketchAPI_IntersectionPoint::SketchAPI_IntersectionPoint(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: SketchAPI_SketchEntity(theFeature)
{
  initialize();
}

SketchAPI_IntersectionPoint::SketchAPI_IntersectionPoint(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const ModelHighAPI_Selection & theExternal )
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByExternalLine(theExternal);
  }
}

SketchAPI_IntersectionPoint::SketchAPI_IntersectionPoint(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const std::string & theExternalName )
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByExternalLineName(theExternalName);
  }
}

SketchAPI_IntersectionPoint::~SketchAPI_IntersectionPoint()
{

}

//--------------------------------------------------------------------------------------
void SketchAPI_IntersectionPoint::setByExternalLine(const ModelHighAPI_Selection & theExternalLine)
{
  fillAttribute(theExternalLine, externalLine());

  execute();
}

void SketchAPI_IntersectionPoint::setByExternalLineName(const std::string & theExternalLineName)
{
  fillAttribute(ModelHighAPI_Selection("EDGE", theExternalLineName), externalLine());

  execute();
}

//--------------------------------------------------------------------------------------