// Name   : ConstructionAPI_Point.cpp
// Purpose: 
//
// History:
// 29/03/16 - Sergey POKHODENKO - Creation of the file

#include "ConstructionAPI_Point.h"

#include <ModelHighAPI_Tools.h>

//==================================================================================================
ConstructionAPI_Point::ConstructionAPI_Point(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
ConstructionAPI_Point::ConstructionAPI_Point(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const ModelHighAPI_Double& theX,
                                             const ModelHighAPI_Double& theY,
                                             const ModelHighAPI_Double& theZ)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setByXYZ(theX, theY, theZ);
  }
}

//==================================================================================================
ConstructionAPI_Point::ConstructionAPI_Point(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const ModelHighAPI_Selection& theEdge,
                                             const ModelHighAPI_Double& theDistanceValue,
                                             const bool theDistancePercent,
                                             const bool theReverse)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setByDistanceOnEdge(theEdge, theDistanceValue, theDistancePercent, theReverse);
  }
}

//==================================================================================================
ConstructionAPI_Point::~ConstructionAPI_Point()
{

}

//==================================================================================================
void ConstructionAPI_Point::setByXYZ(const ModelHighAPI_Double& theX,
                                     const ModelHighAPI_Double& theY,
                                     const ModelHighAPI_Double& theZ)
{
  fillAttribute(ConstructionPlugin_Point::CREATION_METHOD_BY_XYZ(), mycreationMethod);
  fillAttribute(theX, myx);
  fillAttribute(theY, myy);
  fillAttribute(theZ, myz);

  execute();
}

//==================================================================================================
void ConstructionAPI_Point::setByDistanceOnEdge(const ModelHighAPI_Selection& theEdge,
                                                const ModelHighAPI_Double& theDistanceValue,
                                                const bool theDistancePercent,
                                                const bool theReverse)
{
  fillAttribute(ConstructionPlugin_Point::CREATION_METHOD_BY_DISTANCE_ON_EDGE(), mycreationMethod);
  fillAttribute(theEdge, myedge);
  fillAttribute(theDistanceValue, mydistanceValue);
  fillAttribute(theDistancePercent, mydistancePercent);
  fillAttribute(theReverse, myreverse);

  execute();
}

//==================================================================================================
PointPtr addPoint(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const ModelHighAPI_Double& theX,
                  const ModelHighAPI_Double& theY,
                  const ModelHighAPI_Double& theZ)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Point::ID());
  return PointPtr(new ConstructionAPI_Point(aFeature, theX, theY, theZ));
}

//==================================================================================================
PointPtr addPoint(const std::shared_ptr<ModelAPI_Document> & thePart,
                  const ModelHighAPI_Selection& theEdge,
                  const ModelHighAPI_Double& theDistanceValue,
                  const bool theDistancePercent,
                  const bool theReverse)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Point::ID());
  return PointPtr(new ConstructionAPI_Point(aFeature, theEdge, theDistanceValue, theDistancePercent, theReverse));
}
