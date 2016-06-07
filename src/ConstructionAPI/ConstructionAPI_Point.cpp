// Name   : ConstructionAPI_Point.cpp
// Purpose: 
//
// History:
// 29/03/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "ConstructionAPI_Point.h"
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Tools.h>
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
  if (initialize())
    setPoint(theX, theY, theZ);
}

ConstructionAPI_Point::~ConstructionAPI_Point()
{

}

//--------------------------------------------------------------------------------------
void ConstructionAPI_Point::setPoint(const ModelHighAPI_Double & theX,
                                     const ModelHighAPI_Double & theY,
                                     const ModelHighAPI_Double & theZ)
{
  fillAttribute(theX, myx);
  fillAttribute(theY, myy);
  fillAttribute(theZ, myz);

  execute();
}

//--------------------------------------------------------------------------------------
// TODO(spo): make add* as static functions of the class
PointPtr addPoint(
    const std::shared_ptr<ModelAPI_Document> & thePart,
    const ModelHighAPI_Double& theX,
    const ModelHighAPI_Double& theY,
    const ModelHighAPI_Double& theZ)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Point::ID());
  return PointPtr(new ConstructionAPI_Point(aFeature, theX, theY, theZ));
}
