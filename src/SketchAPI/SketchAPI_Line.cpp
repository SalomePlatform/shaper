// Name   : SketchAPI_Line.cpp
// Purpose: 
//
// History:
// 07/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "SketchAPI_Line.h"
//--------------------------------------------------------------------------------------
#include <GeomAPI_Pnt2d.h>
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>
//--------------------------------------------------------------------------------------
SketchAPI_Line::SketchAPI_Line(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: SketchAPI_SketchEntity(theFeature)
{
  initialize();
}

SketchAPI_Line::SketchAPI_Line(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    double theX1, double theY1, double theX2, double theY2)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByCoordinates(theX1, theY1, theX2, theY2);
  }
}

SketchAPI_Line::SketchAPI_Line(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const std::shared_ptr<GeomAPI_Pnt2d> & theStartPoint,
    const std::shared_ptr<GeomAPI_Pnt2d> & theEndPoint)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByPoints(theStartPoint, theEndPoint);
  }
}

SketchAPI_Line::SketchAPI_Line(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const ModelHighAPI_Selection & theExternal )
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByExternal(theExternal);
  }
}

SketchAPI_Line::SketchAPI_Line(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const std::string & theExternalName )
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByExternalName(theExternalName);
  }
}

SketchAPI_Line::~SketchAPI_Line()
{

}

//--------------------------------------------------------------------------------------
void SketchAPI_Line::setByCoordinates(
    double theX1, double theY1, double theX2, double theY2)
{
  fillAttribute(startPoint(), theX1, theY1);
  fillAttribute(endPoint(), theX2, theY2);

  execute();
}

void SketchAPI_Line::setByPoints(
    const std::shared_ptr<GeomAPI_Pnt2d> & theStartPoint,
    const std::shared_ptr<GeomAPI_Pnt2d> & theEndPoint)
{
  fillAttribute(theStartPoint, startPoint());
  fillAttribute(theEndPoint, endPoint());

  execute();
}

void SketchAPI_Line::setByExternal(const ModelHighAPI_Selection & theExternal)
{
  fillAttribute(theExternal, external());

  execute();
}

void SketchAPI_Line::setByExternalName(const std::string & theExternalName)
{
  fillAttribute(ModelHighAPI_Selection("EDGE", theExternalName), external());

  execute();
}

//--------------------------------------------------------------------------------------
void SketchAPI_Line::setStartPoint(double theX, double theY)
{
  fillAttribute(startPoint(), theX, theY);

  execute();
}
void SketchAPI_Line::setStartPoint(const std::shared_ptr<GeomAPI_Pnt2d> & thePoint)
{
  fillAttribute(thePoint, startPoint());

  execute();
}
void SketchAPI_Line::setEndPoint(double theX, double theY)
{
  fillAttribute(endPoint(), theX, theY);

  execute();
}
void SketchAPI_Line::setEndPoint(const std::shared_ptr<GeomAPI_Pnt2d> & thePoint)
{
  fillAttribute(thePoint, endPoint());

  execute();
}

//--------------------------------------------------------------------------------------

void SketchAPI_Line::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aSketchName = theDumper.parentName(aBase);

  AttributeSelectionPtr anExternal = aBase->selection(SketchPlugin_SketchEntity::EXTERNAL_ID());
  if (anExternal->value()) {
    // line is external
    theDumper << aBase << " = " << aSketchName << ".addLine(" << anExternal << ")" << std::endl;
  } else {
    // segment given by its points
    theDumper << aBase << " = " << aSketchName << ".addLine("
              << startPoint() << ", " << endPoint() << ")" << std::endl;
  }
  // dump "auxiliary" flag if necessary
  SketchAPI_SketchEntity::dump(theDumper);
}
