// Name   : SketchAPI_Sketch.cpp
// Purpose: 
//
// History:
// 07/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "SketchAPI_Sketch.h"
//--------------------------------------------------------------------------------------
#include <ModelAPI_CompositeFeature.h>
#include <ModelHighAPI_Tools.h>
#include "SketchAPI_Line.h"
//--------------------------------------------------------------------------------------
SketchAPI_Sketch::SketchAPI_Sketch(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

SketchAPI_Sketch::SketchAPI_Sketch(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const std::shared_ptr<GeomAPI_Ax3> & thePlane)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    setPlane(thePlane);
  }
}

SketchAPI_Sketch::SketchAPI_Sketch(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const ModelHighAPI_Selection & theExternal)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    setExternal(theExternal);
  }
}

SketchAPI_Sketch::~SketchAPI_Sketch()
{

}

//--------------------------------------------------------------------------------------
std::shared_ptr<ModelAPI_CompositeFeature> SketchAPI_Sketch::compositeFeature() const
{
  return std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(feature());
}

//--------------------------------------------------------------------------------------
void SketchAPI_Sketch::setPlane(const std::shared_ptr<GeomAPI_Ax3> & thePlane)
{
  fillAttribute(thePlane->origin(), myorigin);
  fillAttribute(thePlane->dirX(), mydirX);
  fillAttribute(thePlane->normal(), mynormal);

  execute();
}

void SketchAPI_Sketch::setExternal(const ModelHighAPI_Selection & theExternal)
{
  fillAttribute(theExternal, myexternal);

  execute();
}

//--------------------------------------------------------------------------------------
SketchPtr addSketch(const std::shared_ptr<ModelAPI_Document> & thePart,
                    const std::shared_ptr<GeomAPI_Ax3> & thePlane)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(SketchAPI_Sketch::ID());
  return SketchPtr(new SketchAPI_Sketch(aFeature, thePlane));
}

SketchPtr addSketch(const std::shared_ptr<ModelAPI_Document> & thePart,
                    const ModelHighAPI_Selection & theExternal)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(SketchAPI_Sketch::ID());
  return SketchPtr(new SketchAPI_Sketch(aFeature, theExternal));
}

//--------------------------------------------------------------------------------------
std::shared_ptr<SketchAPI_Line> SketchAPI_Sketch::addLine(double theX1, double theY1, double theX2, double theY2)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_Line::ID());
  return LinePtr(new SketchAPI_Line(aFeature, theX1, theY1, theX2, theY2));
}
std::shared_ptr<SketchAPI_Line> SketchAPI_Sketch::addLine(
    const std::shared_ptr<GeomAPI_Pnt2d> & theStartPoint,
    const std::shared_ptr<GeomAPI_Pnt2d> & theEndPoint)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_Line::ID());
  return LinePtr(new SketchAPI_Line(aFeature, theStartPoint, theEndPoint));
}
std::shared_ptr<SketchAPI_Line> SketchAPI_Sketch::addLine(const ModelHighAPI_Selection & theExternal)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_Line::ID());
  return LinePtr(new SketchAPI_Line(aFeature, theExternal));
}
std::shared_ptr<SketchAPI_Line> SketchAPI_Sketch::addLine(const std::string & theExternalName)
{
  // TODO(spo): Add constraint SketchConstraintRigid like in PythonAPI. Is it necessary?
  std::shared_ptr<ModelAPI_Feature> aFeature = compositeFeature()->addFeature(SketchPlugin_Line::ID());
  return LinePtr(new SketchAPI_Line(aFeature, theExternalName));
}
//--------------------------------------------------------------------------------------
