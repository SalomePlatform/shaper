// Name   : ConstructionAPI_Axis.cpp
// Purpose: 
//
// History:
// 15/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "ConstructionAPI_Axis.h"
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Tools.h>
//--------------------------------------------------------------------------------------
ConstructionAPI_Axis::ConstructionAPI_Axis(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

ConstructionAPI_Axis::ConstructionAPI_Axis(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const ModelHighAPI_Selection & thePoint1,
    const ModelHighAPI_Selection & thePoint2)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize())
    setPoints(thePoint1, thePoint2);
}

ConstructionAPI_Axis::ConstructionAPI_Axis(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const ModelHighAPI_Selection & theCylindricalFace)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize())
    setCylindricalFace(theCylindricalFace);
}

ConstructionAPI_Axis::ConstructionAPI_Axis(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const ModelHighAPI_Selection & thePoint,
    const ModelHighAPI_Double & theX,
    const ModelHighAPI_Double & theY,
    const ModelHighAPI_Double & theZ)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize())
    setPointAndDirection(thePoint, theX, theY, theZ);
}

ConstructionAPI_Axis::~ConstructionAPI_Axis()
{

}

//--------------------------------------------------------------------------------------
void ConstructionAPI_Axis::setPoints(
    const ModelHighAPI_Selection & thePoint1,
    const ModelHighAPI_Selection & thePoint2)
{
  fillAttribute("AxisByPointsCase", creationMethod());
  fillAttribute(thePoint1, firstPoint());
  fillAttribute(thePoint2, secondPoint());

  execute();
}

void ConstructionAPI_Axis::setCylindricalFace(
    const ModelHighAPI_Selection & theCylindricalFace)
{
  fillAttribute("AxisByCylindricalFaceCase", creationMethod());
  fillAttribute(theCylindricalFace, cylindricalFace());

  execute();
}

void ConstructionAPI_Axis::setPointAndDirection(
    const ModelHighAPI_Selection & thePoint,
    const ModelHighAPI_Double & theX,
    const ModelHighAPI_Double & theY,
    const ModelHighAPI_Double & theZ)
{
  fillAttribute("AxisByPointAndDirection", creationMethod());
  fillAttribute(thePoint, firstPoint());
  fillAttribute(theX, xDirection());
  fillAttribute(theY, yDirection());
  fillAttribute(theZ, zDirection());

  execute();
}

//--------------------------------------------------------------------------------------
AxisPtr addAxis(const std::shared_ptr<ModelAPI_Document> & thePart,
                const ModelHighAPI_Selection & thePoint1,
                const ModelHighAPI_Selection & thePoint2)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Axis::ID());
  return AxisPtr(new ConstructionAPI_Axis(aFeature, thePoint1, thePoint2));
}

AxisPtr addAxis(const std::shared_ptr<ModelAPI_Document> & thePart,
                const ModelHighAPI_Selection & theCylindricalFace)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Axis::ID());
  return AxisPtr(new ConstructionAPI_Axis(aFeature, theCylindricalFace));
}

AxisPtr addAxis(const std::shared_ptr<ModelAPI_Document> & thePart,
                const ModelHighAPI_Selection & thePoint,
                const ModelHighAPI_Double & theX,
                const ModelHighAPI_Double & theY,
                const ModelHighAPI_Double & theZ)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Axis::ID());
  return AxisPtr(new ConstructionAPI_Axis(aFeature, thePoint, theX, theY, theZ));
}
