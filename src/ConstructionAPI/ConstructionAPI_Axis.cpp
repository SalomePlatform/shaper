// Name   : ConstructionAPI_Axis.cpp
// Purpose: 
//
// History:
// 15/06/16 - Sergey POKHODENKO - Creation of the file

#include "ConstructionAPI_Axis.h"

#include <ModelHighAPI_Tools.h>

//==================================================================================================
ConstructionAPI_Axis::ConstructionAPI_Axis(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
ConstructionAPI_Axis::ConstructionAPI_Axis(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                           const ModelHighAPI_Selection& theObject1,
                                           const ModelHighAPI_Selection& theObject2)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    GeomAPI_Shape::ShapeType aType1 = getShapeType(theObject1);
    GeomAPI_Shape::ShapeType aType2 = getShapeType(theObject2);
    if(aType1 == GeomAPI_Shape::VERTEX && aType2 == GeomAPI_Shape::VERTEX) {
      setByPoints(theObject1, theObject2);
    } else if(aType1 == GeomAPI_Shape::FACE && aType2 == GeomAPI_Shape::VERTEX) {
      setByPlaneAndPoint(theObject1, theObject2);
    } else if(aType1 == GeomAPI_Shape::FACE && aType2 == GeomAPI_Shape::FACE) {
      setByTwoPlanes(theObject1, theObject2);
    }
  }
}

//==================================================================================================
ConstructionAPI_Axis::ConstructionAPI_Axis(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                           const ModelHighAPI_Selection& theObject)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    GeomAPI_Shape::ShapeType aType = getShapeType(theObject);
    if(aType == GeomAPI_Shape::EDGE) {
      setByLine(theObject);
    } else if(aType == GeomAPI_Shape::FACE) {
      setByCylindricalFace(theObject);
    }
  }
}

//==================================================================================================
ConstructionAPI_Axis::ConstructionAPI_Axis(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                           const ModelHighAPI_Selection& thePoint,
                                           const ModelHighAPI_Double& theX,
                                           const ModelHighAPI_Double& theY,
                                           const ModelHighAPI_Double& theZ)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setByPointAndDirection(thePoint, theX, theY, theZ);
  }
}

//==================================================================================================
ConstructionAPI_Axis::ConstructionAPI_Axis(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                           const ModelHighAPI_Double& theDX,
                                           const ModelHighAPI_Double& theDY,
                                           const ModelHighAPI_Double& theDZ)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setByDimensions(theDX, theDY, theDZ);
  }
}

//==================================================================================================
ConstructionAPI_Axis::ConstructionAPI_Axis(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                           const ModelHighAPI_Selection& thePlane1,
                                           const ModelHighAPI_Double& theOffset1,
                                           const bool theReverseOffset1,
                                           const ModelHighAPI_Selection& thePlane2,
                                           const ModelHighAPI_Double& theOffset2,
                                           const bool theReverseOffset2)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setByTwoPlanes(thePlane1, theOffset1, theReverseOffset1, thePlane2, theOffset2, theReverseOffset2);
  }
}

//==================================================================================================
ConstructionAPI_Axis::~ConstructionAPI_Axis()
{
}

//==================================================================================================
void ConstructionAPI_Axis::setByPoints(const ModelHighAPI_Selection& thePoint1,
                                       const ModelHighAPI_Selection& thePoint2)
{
  fillAttribute(ConstructionPlugin_Axis::CREATION_METHOD_BY_TWO_POINTS(), creationMethod());
  fillAttribute(thePoint1, firstPoint());
  fillAttribute(thePoint2, secondPoint());

  execute();
}

//==================================================================================================
void ConstructionAPI_Axis::setByCylindricalFace(const ModelHighAPI_Selection& theCylindricalFace)
{
  fillAttribute(ConstructionPlugin_Axis::CREATION_METHOD_BY_CYLINDRICAL_FACE(), creationMethod());
  fillAttribute(theCylindricalFace, cylindricalFace());

  execute();
}

//==================================================================================================
void ConstructionAPI_Axis::setByPointAndDirection(const ModelHighAPI_Selection& thePoint,
                                                  const ModelHighAPI_Double& theX,
                                                  const ModelHighAPI_Double& theY,
                                                  const ModelHighAPI_Double& theZ)
{
  fillAttribute(ConstructionPlugin_Axis::CREATION_METHOD_BY_POINT_AND_DIRECTION(), creationMethod());
  fillAttribute(thePoint, firstPoint());
  fillAttribute(theX, xDirection());
  fillAttribute(theY, yDirection());
  fillAttribute(theZ, zDirection());

  execute();
}

//==================================================================================================
void ConstructionAPI_Axis::setByDimensions(const ModelHighAPI_Double& theDX,
                                           const ModelHighAPI_Double& theDY,
                                           const ModelHighAPI_Double& theDZ)
{
  fillAttribute(ConstructionPlugin_Axis::CREATION_METHOD_BY_DIMENSIONS(), creationMethod());
  fillAttribute(theDX, xDimension());
  fillAttribute(theDY, yDimension());
  fillAttribute(theDZ, zDimension());

  execute();
}

//==================================================================================================
void ConstructionAPI_Axis::setByLine(const ModelHighAPI_Selection& theLine)
{
  fillAttribute(ConstructionPlugin_Axis::CREATION_METHOD_BY_LINE(), creationMethod());
  fillAttribute(theLine, line());

  execute();
}

//==================================================================================================
void ConstructionAPI_Axis::setByPlaneAndPoint(const ModelHighAPI_Selection& thePlane,
                                              const ModelHighAPI_Selection& thePoint)
{
  fillAttribute(ConstructionPlugin_Axis::CREATION_METHOD_BY_PLANE_AND_POINT(), creationMethod());
  fillAttribute(thePlane, plane());
  fillAttribute(thePoint, point());

  execute();
}

//==================================================================================================
void ConstructionAPI_Axis::setByTwoPlanes(const ModelHighAPI_Selection& thePlane1,
                                          const ModelHighAPI_Selection& thePlane2)
{
  fillAttribute(ConstructionPlugin_Axis::CREATION_METHOD_BY_TWO_PLANES(), creationMethod());
  fillAttribute(thePlane1, plane1());
  fillAttribute("", useOffset1());
  fillAttribute(thePlane2, plane2());
  fillAttribute("", useOffset2());

  execute();
}

//==================================================================================================
void ConstructionAPI_Axis::setByTwoPlanes(const ModelHighAPI_Selection& thePlane1,
                                          const ModelHighAPI_Double& theOffset1,
                                          const bool theReverseOffset1,
                                          const ModelHighAPI_Selection& thePlane2,
                                          const ModelHighAPI_Double& theOffset2,
                                          const bool theReverseOffset2)
{
  fillAttribute(ConstructionPlugin_Axis::CREATION_METHOD_BY_TWO_PLANES(), creationMethod());
  fillAttribute(thePlane1, plane1());
  fillAttribute(ConstructionPlugin_Axis::USE_OFFSET1(), useOffset1());
  fillAttribute(theOffset1, offset1());
  fillAttribute(theReverseOffset1, reverseOffset1());
  fillAttribute(thePlane2, plane2());
  fillAttribute(ConstructionPlugin_Axis::USE_OFFSET2(), useOffset2());
  fillAttribute(theOffset2, offset2());
  fillAttribute(theReverseOffset2, reverseOffset2());

  execute();
}

//==================================================================================================
AxisPtr addAxis(const std::shared_ptr<ModelAPI_Document>& thePart,
                const ModelHighAPI_Selection& thePoint1,
                const ModelHighAPI_Selection& thePoint2)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Axis::ID());
  return AxisPtr(new ConstructionAPI_Axis(aFeature, thePoint1, thePoint2));
}

//==================================================================================================
AxisPtr addAxis(const std::shared_ptr<ModelAPI_Document>& thePart,
                const ModelHighAPI_Selection& theObject)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Axis::ID());
  return AxisPtr(new ConstructionAPI_Axis(aFeature, theObject));
}

//==================================================================================================
AxisPtr addAxis(const std::shared_ptr<ModelAPI_Document>& thePart,
                const ModelHighAPI_Selection& thePoint,
                const ModelHighAPI_Double& theX,
                const ModelHighAPI_Double& theY,
                const ModelHighAPI_Double& theZ)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Axis::ID());
  return AxisPtr(new ConstructionAPI_Axis(aFeature, thePoint, theX, theY, theZ));
}

//==================================================================================================
AxisPtr addAxis(const std::shared_ptr<ModelAPI_Document>& thePart,
                const ModelHighAPI_Double& theDX,
                const ModelHighAPI_Double& theDY,
                const ModelHighAPI_Double& theDZ)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Axis::ID());
  return AxisPtr(new ConstructionAPI_Axis(aFeature, theDX, theDY, theDZ));
}

//==================================================================================================
AxisPtr addAxis(const std::shared_ptr<ModelAPI_Document>& thePart,
                const ModelHighAPI_Selection& thePlane1,
                const ModelHighAPI_Double& theOffset1,
                const bool theReverseOffset1,
                const ModelHighAPI_Selection& thePlane2,
                const ModelHighAPI_Double& theOffset2,
                const bool theReverseOffset2)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Axis::ID());
  return AxisPtr(new ConstructionAPI_Axis(aFeature, thePlane1, theOffset1, theReverseOffset1,
                                                    thePlane2, theOffset2, theReverseOffset2));
}
