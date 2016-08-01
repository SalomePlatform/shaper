// Name   : ConstructionAPI_Point.cpp
// Purpose: 
//
// History:
// 29/03/16 - Sergey POKHODENKO - Creation of the file

#include "ConstructionAPI_Point.h"

#include <GeomAPI_Shape.h>

#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

#include <algorithm>

/*static GeomAPI_Shape::ShapeType shapeTypeByStr(const std::string& theShapeTypeStr);
static GeomAPI_Shape::ShapeType getShapeType(const ModelHighAPI_Selection& theSelection);*/

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

/*//==================================================================================================
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
ConstructionAPI_Point::ConstructionAPI_Point(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const ModelHighAPI_Selection& theObject1,
                                             const ModelHighAPI_Selection& theObject2)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    GeomAPI_Shape::ShapeType aType1 = getShapeType(theObject1);
    GeomAPI_Shape::ShapeType aType2 = getShapeType(theObject2);

    if(aType1 == GeomAPI_Shape::VERTEX && aType2 == GeomAPI_Shape::FACE) {
      // If first object is vertex and second object is face then set by projection.
      setByProjection(theObject1, theObject2);
    } else if(aType1 == GeomAPI_Shape::EDGE && aType2 == GeomAPI_Shape::EDGE) {
      // If both objects are edges then set by lines intersection.
      setByLinesIntersection(theObject1, theObject2);
    } else if(aType1 == GeomAPI_Shape::EDGE && aType2 == GeomAPI_Shape::FACE) {
      // If first object is edge and second object is face then set by line and plane intersection.
      setByLineAndPlaneIntersection(theObject1, theObject2);
    }
  }
}*/

//==================================================================================================
ConstructionAPI_Point::~ConstructionAPI_Point()
{

}

//==================================================================================================
void ConstructionAPI_Point::setByXYZ(const ModelHighAPI_Double& theX,
                                     const ModelHighAPI_Double& theY,
                                     const ModelHighAPI_Double& theZ)
{
  //fillAttribute(ConstructionPlugin_Point::CREATION_METHOD_BY_XYZ(), mycreationMethod);
  fillAttribute(theX, myx);
  fillAttribute(theY, myy);
  fillAttribute(theZ, myz);

  execute();
}

/*//==================================================================================================
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
void ConstructionAPI_Point::setByProjection(const ModelHighAPI_Selection& theVertex,
                                            const ModelHighAPI_Selection& theFace)
{
  fillAttribute(ConstructionPlugin_Point::CREATION_METHOD_BY_PROJECTION(), mycreationMethod);
  fillAttribute(theVertex, mypoint);
  fillAttribute(theFace, myplane);

  execute();
}

//==================================================================================================
void ConstructionAPI_Point::setByLinesIntersection(const ModelHighAPI_Selection& theEdge1,
                                                   const ModelHighAPI_Selection& theEdge2)
{
  fillAttribute(ConstructionPlugin_Point::CREATION_METHOD_BY_LINES_INTERSECTION(), mycreationMethod);
  fillAttribute(theEdge1, myfirstLine);
  fillAttribute(theEdge2, mysecondLine);

  execute();
}

//==================================================================================================
void ConstructionAPI_Point::setByLineAndPlaneIntersection(const ModelHighAPI_Selection& theEdge,
                                                          const ModelHighAPI_Selection& theFace)
{
  fillAttribute(ConstructionPlugin_Point::CREATION_METHOD_BY_LINE_AND_PLANE_INTERSECTION(), mycreationMethod);
  fillAttribute(theEdge, myintersectionLine);
  fillAttribute(theFace, myintersectionPlane);

  execute();
}*/

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

/*//==================================================================================================
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

//==================================================================================================
PointPtr addPoint(const std::shared_ptr<ModelAPI_Document> & thePart,
                  const ModelHighAPI_Selection& theObject1,
                  const ModelHighAPI_Selection& theObject2)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Point::ID());
  return PointPtr(new ConstructionAPI_Point(aFeature, theObject1, theObject2));
}

//==================================================================================================
GeomAPI_Shape::ShapeType shapeTypeByStr(const std::string& theShapeTypeStr)
{
  GeomAPI_Shape::ShapeType aShapeType = GeomAPI_Shape::SHAPE;

  std::string aShapeTypeStr = theShapeTypeStr;
  std::transform(aShapeTypeStr.begin(), aShapeTypeStr.end(), aShapeTypeStr.begin(), ::tolower);

  if(theShapeTypeStr == "compound") {
    aShapeType = GeomAPI_Shape::COMPOUND;
  } else if(theShapeTypeStr == "compsolid") {
    aShapeType = GeomAPI_Shape::COMPSOLID;
  } else if(theShapeTypeStr == "solid") {
    aShapeType = GeomAPI_Shape::SOLID;
  } else if(theShapeTypeStr == "shell") {
    aShapeType = GeomAPI_Shape::SHELL;
  } else if(theShapeTypeStr == "face") {
    aShapeType = GeomAPI_Shape::FACE;
  } else if(theShapeTypeStr == "wire") {
    aShapeType = GeomAPI_Shape::WIRE;
  } else if(theShapeTypeStr == "edge") {
    aShapeType = GeomAPI_Shape::EDGE;
  } else if(theShapeTypeStr == "vertex") {
    aShapeType = GeomAPI_Shape::VERTEX;
  } else if(theShapeTypeStr == "shape") {
    aShapeType = GeomAPI_Shape::SHAPE;
  }

  return aShapeType;
}

//==================================================================================================
GeomAPI_Shape::ShapeType getShapeType(const ModelHighAPI_Selection& theSelection)
{
  GeomAPI_Shape::ShapeType aShapeType = GeomAPI_Shape::SHAPE;

  switch(theSelection.variantType()) {
    case ModelHighAPI_Selection::VT_ResultSubShapePair: {
      ResultSubShapePair aPair = theSelection.resultSubShapePair();
      GeomShapePtr aShape = aPair.second;
      if(!aShape.get()) {
        aShape = aPair.first->shape();
      }
      if(!aShape.get()) {
        return aShapeType;
      }
      aShapeType = aShape->shapeType();
      break;
    }
    case ModelHighAPI_Selection::VT_TypeSubShapeNamePair: {
      TypeSubShapeNamePair aPair = theSelection.typeSubShapeNamePair();
      std::string aType = aPair.first;
      aShapeType = shapeTypeByStr(aType);
      break;
    }
  }

  return aShapeType;
}*/
