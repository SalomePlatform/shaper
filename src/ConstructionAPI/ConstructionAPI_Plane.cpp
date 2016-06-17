// Name   : ConstructionAPI_Plane.cpp
// Purpose: 
//
// History:
// 27/05/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "ConstructionAPI_Plane.h"
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Tools.h>
//--------------------------------------------------------------------------------------
ConstructionAPI_Plane::ConstructionAPI_Plane(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

ConstructionAPI_Plane::ConstructionAPI_Plane(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const ModelHighAPI_Selection & theFace,
    const ModelHighAPI_Double & theDistance)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize())
    setFaceAndDistance(theFace, theDistance);
}

ConstructionAPI_Plane::ConstructionAPI_Plane(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const ModelHighAPI_Double & theA,
    const ModelHighAPI_Double & theB,
    const ModelHighAPI_Double & theC,
    const ModelHighAPI_Double & theD)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize())
    setGeneralEquation(theA, theB, theC, theD);
}

ConstructionAPI_Plane::~ConstructionAPI_Plane()
{

}

//--------------------------------------------------------------------------------------
void ConstructionAPI_Plane::setFaceAndDistance(
    const ModelHighAPI_Selection & theFace,
    const ModelHighAPI_Double & theDistance)
{
  fillAttribute(theFace, myface);
  fillAttribute(theDistance, mydistance);

  execute();
}

void ConstructionAPI_Plane::setGeneralEquation(
    const ModelHighAPI_Double & theA,
    const ModelHighAPI_Double & theB,
    const ModelHighAPI_Double & theC,
    const ModelHighAPI_Double & theD)
{
  fillAttribute(theA, myA);
  fillAttribute(theB, myB);
  fillAttribute(theC, myC);
  fillAttribute(theD, myD);

  execute();
}

//--------------------------------------------------------------------------------------
PlanePtr addPlane(const std::shared_ptr<ModelAPI_Document> & thePart,
                  const ModelHighAPI_Selection & theFace,
                  const ModelHighAPI_Double & theDistance)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Plane::ID());
  return PlanePtr(new ConstructionAPI_Plane(aFeature, theFace, theDistance));
}

PlanePtr addPlane(const std::shared_ptr<ModelAPI_Document> & thePart,
                  const ModelHighAPI_Double & theA,
                  const ModelHighAPI_Double & theB,
                  const ModelHighAPI_Double & theC,
                  const ModelHighAPI_Double & theD)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Plane::ID());
  return PlanePtr(new ConstructionAPI_Plane(aFeature, theA, theB, theC, theD));
}
