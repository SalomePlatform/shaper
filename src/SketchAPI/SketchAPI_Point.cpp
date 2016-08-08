// Name   : SketchAPI_Point.cpp
// Purpose: 
//
// History:
// 15/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "SketchAPI_Point.h"
//--------------------------------------------------------------------------------------
#include <GeomAPI_Pnt2d.h>
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>
//--------------------------------------------------------------------------------------
SketchAPI_Point::SketchAPI_Point(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: SketchAPI_SketchEntity(theFeature)
{
  initialize();
}

SketchAPI_Point::SketchAPI_Point(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    double theX, double theY)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setCoordinates(theX, theY);
  }
}

SketchAPI_Point::SketchAPI_Point(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const std::shared_ptr<GeomAPI_Pnt2d> & thePoint)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setCoordinates(thePoint);
  }
}

SketchAPI_Point::SketchAPI_Point(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const ModelHighAPI_Selection & theExternal )
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByExternal(theExternal);
  }
}

SketchAPI_Point::SketchAPI_Point(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const std::string & theExternalName )
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByExternalName(theExternalName);
  }
}

SketchAPI_Point::~SketchAPI_Point()
{

}

//--------------------------------------------------------------------------------------
void SketchAPI_Point::setCoordinates(
    double theX, double theY)
{
  fillAttribute(coordinates(), theX, theY);

  execute();
}

void SketchAPI_Point::setCoordinates(
    const std::shared_ptr<GeomAPI_Pnt2d> & thePoint)
{
  fillAttribute(thePoint, coordinates());

  execute();
}

void SketchAPI_Point::setByExternal(const ModelHighAPI_Selection & theExternal)
{
  fillAttribute(theExternal, external());

  execute();
}

void SketchAPI_Point::setByExternalName(const std::string & theExternalName)
{
  fillAttribute(ModelHighAPI_Selection("VERTEX", theExternalName), external());

  execute();
}

//--------------------------------------------------------------------------------------

void SketchAPI_Point::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aSketchName = theDumper.parentName(aBase);

  AttributeSelectionPtr anExternal = aBase->selection(SketchPlugin_SketchEntity::EXTERNAL_ID());
  if (anExternal->value()) {
    // point is external
    theDumper << aBase << " = " << aSketchName << ".addPoint(" << anExternal << ")" << std::endl;
  } else {
    // point given by coordinates
    std::shared_ptr<GeomDataAPI_Point2D> aPoint = coordinates();
    theDumper << aBase << " = " << aSketchName << ".addPoint(" << aPoint << ")" << std::endl;
  }
  // dump "auxiliary" flag if necessary
  SketchAPI_SketchEntity::dump(theDumper);
}
