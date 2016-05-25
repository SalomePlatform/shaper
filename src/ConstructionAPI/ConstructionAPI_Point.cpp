// Name   : ConstructionAPI_Point.cpp
// Purpose: 
//
// History:
// 29/03/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "ConstructionAPI_Point.h"
//--------------------------------------------------------------------------------------
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>

#include <ModelHighAPI_Double.h>
//--------------------------------------------------------------------------------------
ConstructionAPI_Point::ConstructionAPI_Point(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

ConstructionAPI_Point::ConstructionAPI_Point(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const ModelHighAPI_Double & theX,
    const ModelHighAPI_Double & theY,
    const ModelHighAPI_Double & theZ)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    setPoint(theX, theY, theZ);
    execute();
  }
}

ConstructionAPI_Point::~ConstructionAPI_Point()
{

}

//--------------------------------------------------------------------------------------
bool ConstructionAPI_Point::initialize()
{
  if (!myFeature) {
    throwException("ConstructionAPI_Point exception: The feature is NULL.");
    return false;
  }

  if (feature()->getKind() != "Point") {
    throwException("ConstructionAPI_Point exception: Wrong feature kind.");
    return false;
  }

  // data() throws exceptions if the attribute is invalid
  myX = feature()->real("x");
  myY = feature()->real("y");
  myZ = feature()->real("z");

  if (!myX || !myY || !myZ)
    return false;

  return true;
}

//--------------------------------------------------------------------------------------
void ConstructionAPI_Point::setPoint(const ModelHighAPI_Double & theX,
                                     const ModelHighAPI_Double & theY,
                                     const ModelHighAPI_Double & theZ)
{
  theX.fillAttribute(myX);
  theY.fillAttribute(myY);
  theZ.fillAttribute(myZ);
}

//--------------------------------------------------------------------------------------
std::shared_ptr<ModelAPI_AttributeDouble> ConstructionAPI_Point::x() const
{
  return myX;
}
std::shared_ptr<ModelAPI_AttributeDouble> ConstructionAPI_Point::y() const
{
  return myY;
}
std::shared_ptr<ModelAPI_AttributeDouble> ConstructionAPI_Point::z() const
{
  return myZ;
}

//--------------------------------------------------------------------------------------
// TODO(spo): make add* as static functions of the class
PointPtr addPoint(
    std::shared_ptr<ModelAPI_Document> thePart)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature("Point");
  return PointPtr(new ConstructionAPI_Point(aFeature));
}

PointPtr addPoint(
    std::shared_ptr<ModelAPI_Document> thePart,
    const ModelHighAPI_Double& theX,
    const ModelHighAPI_Double& theY,
    const ModelHighAPI_Double& theZ)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature("Point");
  return PointPtr(new ConstructionAPI_Point(aFeature, theX, theY, theZ));
}
