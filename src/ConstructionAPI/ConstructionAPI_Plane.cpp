// Name   : ConstructionAPI_Plane.cpp
// Purpose: 
//
// History:
// 27/05/16 - Sergey POKHODENKO - Creation of the file

#include "ConstructionAPI_Plane.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
ConstructionAPI_Plane::ConstructionAPI_Plane(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
ConstructionAPI_Plane::ConstructionAPI_Plane(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const ModelHighAPI_Selection& theFace,
                                             const ModelHighAPI_Double& theDistance,
                                             const bool theIsReverse)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setByFaceAndDistance(theFace, theDistance, theIsReverse);
  }
}

//==================================================================================================
ConstructionAPI_Plane::ConstructionAPI_Plane(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const ModelHighAPI_Double& theA,
                                             const ModelHighAPI_Double& theB,
                                             const ModelHighAPI_Double& theC,
                                             const ModelHighAPI_Double& theD)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setByGeneralEquation(theA, theB, theC, theD);
  }
}

//==================================================================================================
ConstructionAPI_Plane::ConstructionAPI_Plane(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const ModelHighAPI_Selection& thePoint1,
                                             const ModelHighAPI_Selection& thePoint2,
                                             const ModelHighAPI_Selection& thePoint3)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setByThreePoints(thePoint1, thePoint2, thePoint3);
  }
}

//==================================================================================================
ConstructionAPI_Plane::ConstructionAPI_Plane(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const ModelHighAPI_Selection& theLine,
                                             const ModelHighAPI_Selection& thePoint,
                                             const bool theIsPerpendicular)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setByLineAndPoint(theLine, thePoint, theIsPerpendicular);
  }
}

//==================================================================================================
ConstructionAPI_Plane::ConstructionAPI_Plane(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const ModelHighAPI_Selection& theObject1,
                                             const ModelHighAPI_Selection& theObject2)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    GeomAPI_Shape::ShapeType aType1 = getShapeType(theObject1);
    GeomAPI_Shape::ShapeType aType2 = getShapeType(theObject2);
    if(aType1 == GeomAPI_Shape::FACE && aType2 == GeomAPI_Shape::VERTEX) {
      setByCoincidentToPoint(theObject1, theObject2);
    } else if(aType1 == GeomAPI_Shape::FACE && aType2 == GeomAPI_Shape::FACE) {
      setByTwoParallelPlanes(theObject1, theObject2);
    }
  }
}

//==================================================================================================
ConstructionAPI_Plane::ConstructionAPI_Plane(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const ModelHighAPI_Selection& thePlane,
                                             const ModelHighAPI_Selection& theAxis,
                                             const ModelHighAPI_Double& theAngle)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setByRotation(thePlane, theAxis, theAngle);
  }
}

//==================================================================================================
ConstructionAPI_Plane::~ConstructionAPI_Plane()
{
}

//==================================================================================================
void ConstructionAPI_Plane::setByFaceAndDistance(const ModelHighAPI_Selection& theFace,
                                                 const ModelHighAPI_Double& theDistance,
                                                 const bool theIsReverse)
{
  fillAttribute(ConstructionPlugin_Plane::CREATION_METHOD_BY_OTHER_PLANE(), mycreationMethod);
  fillAttribute(theFace, myplane);
  fillAttribute(ConstructionPlugin_Plane::CREATION_METHOD_BY_DISTANCE_FROM_OTHER(), mycreationMethodByOtherPlane);
  fillAttribute(theDistance, mydistance);
  fillAttribute(theIsReverse, myreverse);

  execute();
}

//==================================================================================================
void ConstructionAPI_Plane::setByGeneralEquation(const ModelHighAPI_Double& theA,
                                                 const ModelHighAPI_Double& theB,
                                                 const ModelHighAPI_Double& theC,
                                                 const ModelHighAPI_Double& theD)
{
  fillAttribute(ConstructionPlugin_Plane::CREATION_METHOD_BY_GENERAL_EQUATION(), mycreationMethod);
  fillAttribute(theA, myA);
  fillAttribute(theB, myB);
  fillAttribute(theC, myC);
  fillAttribute(theD, myD);

  execute();
}

//==================================================================================================
void ConstructionAPI_Plane::setByThreePoints(const ModelHighAPI_Selection& thePoint1,
                                             const ModelHighAPI_Selection& thePoint2,
                                             const ModelHighAPI_Selection& thePoint3)
{
  fillAttribute(ConstructionPlugin_Plane::CREATION_METHOD_BY_THREE_POINTS(), mycreationMethod);
  fillAttribute(thePoint1, mypoint1);
  fillAttribute(thePoint2, mypoint2);
  fillAttribute(thePoint3, mypoint3);

  execute();
}

//==================================================================================================
void ConstructionAPI_Plane::setByLineAndPoint(const ModelHighAPI_Selection& theLine,
                                              const ModelHighAPI_Selection& thePoint,
                                              const bool theIsPerpendicular)
{
  fillAttribute(ConstructionPlugin_Plane::CREATION_METHOD_BY_LINE_AND_POINT(), mycreationMethod);
  fillAttribute(theLine, myline);
  fillAttribute(thePoint, mypoint);
  fillAttribute(theIsPerpendicular, myperpendicular);

  execute();
}

//==================================================================================================
void ConstructionAPI_Plane::setByTwoParallelPlanes(const ModelHighAPI_Selection& thePlane1,
                                                   const ModelHighAPI_Selection& thePlane2)
{
  fillAttribute(ConstructionPlugin_Plane::CREATION_METHOD_BY_TWO_PARALLEL_PLANES(), mycreationMethod);
  fillAttribute(thePlane1, myplane1);
  fillAttribute(thePlane2, myplane2);

  execute();
}

//==================================================================================================
void ConstructionAPI_Plane::setByCoincidentToPoint(const ModelHighAPI_Selection& thePlane,
                                                   const ModelHighAPI_Selection& thePoint)
{
  fillAttribute(ConstructionPlugin_Plane::CREATION_METHOD_BY_OTHER_PLANE(), mycreationMethod);
  fillAttribute(thePlane, myplane);
  fillAttribute(ConstructionPlugin_Plane::CREATION_METHOD_BY_COINCIDENT_TO_POINT(), mycreationMethodByOtherPlane);
  fillAttribute(thePoint, mycoincidentPoint);

  execute();
}

//==================================================================================================
void ConstructionAPI_Plane::setByRotation(const ModelHighAPI_Selection& thePlane,
                                          const ModelHighAPI_Selection& theAxis,
                                          const ModelHighAPI_Double& theAngle)
{
  fillAttribute(ConstructionPlugin_Plane::CREATION_METHOD_BY_OTHER_PLANE(), mycreationMethod);
  fillAttribute(thePlane, myplane);
  fillAttribute(ConstructionPlugin_Plane::CREATION_METHOD_BY_ROTATION(), mycreationMethodByOtherPlane);
  fillAttribute(theAxis, myaxis);
  fillAttribute(theAngle, myangle);

  execute();
}

//==================================================================================================
void ConstructionAPI_Plane::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  theDumper << aBase << " = model.addPlane(" << aDocName;

  std::string aCreationMethod = aBase->string(ConstructionPlugin_Plane::CREATION_METHOD())->value();

  if(aCreationMethod == ConstructionPlugin_Plane::CREATION_METHOD_BY_GENERAL_EQUATION()) {
    AttributeDoublePtr anAttrA = aBase->real(ConstructionPlugin_Plane::A());
    AttributeDoublePtr anAttrB = aBase->real(ConstructionPlugin_Plane::B());
    AttributeDoublePtr anAttrC = aBase->real(ConstructionPlugin_Plane::C());
    AttributeDoublePtr anAttrD = aBase->real(ConstructionPlugin_Plane::D());

    theDumper << ", " << anAttrA << ", " << anAttrB << ", " << anAttrC << ", " << anAttrD;
  } else if(aCreationMethod == ConstructionPlugin_Plane::CREATION_METHOD_BY_THREE_POINTS()) {
    AttributeSelectionPtr anAttrPnt1 = aBase->selection(ConstructionPlugin_Plane::POINT1());
    AttributeSelectionPtr anAttrPnt2 = aBase->selection(ConstructionPlugin_Plane::POINT2());
    AttributeSelectionPtr anAttrPnt3 = aBase->selection(ConstructionPlugin_Plane::POINT3());

    theDumper << ", " << anAttrPnt1 << ", " << anAttrPnt2 << ", " << anAttrPnt3;
  } else if(aCreationMethod == ConstructionPlugin_Plane::CREATION_METHOD_BY_LINE_AND_POINT()) {
    AttributeSelectionPtr anAttrLine = aBase->selection(ConstructionPlugin_Plane::LINE());
    AttributeSelectionPtr anAttrPoint = aBase->selection(ConstructionPlugin_Plane::POINT());
    AttributeBooleanPtr anAttrPerpendicular = aBase->boolean(ConstructionPlugin_Plane::PERPENDICULAR());

    theDumper << ", " << anAttrLine << ", " << anAttrPoint << ", " << anAttrPerpendicular;
  } else if(aCreationMethod == ConstructionPlugin_Plane::CREATION_METHOD_BY_OTHER_PLANE()) {
    AttributeSelectionPtr anAttrPlane = aBase->selection(ConstructionPlugin_Plane::PLANE());

    std::string aCreationMethodOption =
        aBase->string(ConstructionPlugin_Plane::CREATION_METHOD_BY_OTHER_PLANE_OPTION())->value();
    if(aCreationMethodOption == ConstructionPlugin_Plane::CREATION_METHOD_BY_DISTANCE_FROM_OTHER()) {
      AttributeDoublePtr anAttrDistance = aBase->real(ConstructionPlugin_Plane::DISTANCE());
      AttributeBooleanPtr anAttrReverse = aBase->boolean(ConstructionPlugin_Plane::REVERSE());

      theDumper << ", " << anAttrPlane << ", " << anAttrDistance << ", " << anAttrReverse;
    } else if(aCreationMethodOption == ConstructionPlugin_Plane::CREATION_METHOD_BY_COINCIDENT_TO_POINT()) {
      AttributeSelectionPtr anAttrPoint = aBase->selection(ConstructionPlugin_Plane::COINCIDENT_POINT());

      theDumper << ", " << anAttrPlane << ", " << anAttrPoint;
    } else if(aCreationMethodOption == ConstructionPlugin_Plane::CREATION_METHOD_BY_ROTATION()) {
      AttributeSelectionPtr anAttrAxis = aBase->selection(ConstructionPlugin_Plane::AXIS());
      AttributeDoublePtr anAttrAngle = aBase->real(ConstructionPlugin_Plane::ANGLE());

      theDumper << ", " << anAttrPlane << ", " << anAttrAxis << ", " << anAttrAngle;
    }
  } else if(aCreationMethod == ConstructionPlugin_Plane::CREATION_METHOD_BY_TWO_PARALLEL_PLANES()) {
    AttributeSelectionPtr anAttrPlane1 = aBase->selection(ConstructionPlugin_Plane::PLANE1());
    AttributeSelectionPtr anAttrPlane2 = aBase->selection(ConstructionPlugin_Plane::PLANE2());

    theDumper << ", " << anAttrPlane1 << ", " << anAttrPlane2;
  }

  theDumper << ")" << std::endl;
}

//==================================================================================================
PlanePtr addPlane(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const ModelHighAPI_Selection& theFace,
                  const ModelHighAPI_Double& theDistance,
                  const bool theIsReverse)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Plane::ID());
  return PlanePtr(new ConstructionAPI_Plane(aFeature, theFace, theDistance, theIsReverse));
}

//==================================================================================================
PlanePtr addPlane(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const ModelHighAPI_Double& theA,
                  const ModelHighAPI_Double& theB,
                  const ModelHighAPI_Double& theC,
                  const ModelHighAPI_Double& theD)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Plane::ID());
  return PlanePtr(new ConstructionAPI_Plane(aFeature, theA, theB, theC, theD));
}

//==================================================================================================
PlanePtr addPlane(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const ModelHighAPI_Selection& thePoint1,
                  const ModelHighAPI_Selection& thePoint2,
                  const ModelHighAPI_Selection& thePoint3)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Plane::ID());
  return PlanePtr(new ConstructionAPI_Plane(aFeature, thePoint1, thePoint2, thePoint3));
}

//==================================================================================================
PlanePtr addPlane(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const ModelHighAPI_Selection& theLine,
                  const ModelHighAPI_Selection& thePoint,
                  const bool theIsPerpendicular)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Plane::ID());
  return PlanePtr(new ConstructionAPI_Plane(aFeature, theLine, thePoint, theIsPerpendicular));
}

//==================================================================================================
PlanePtr addPlane(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const ModelHighAPI_Selection& theObject1,
                  const ModelHighAPI_Selection& theObject2)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Plane::ID());
  return PlanePtr(new ConstructionAPI_Plane(aFeature, theObject1, theObject2));
}

//==================================================================================================
PlanePtr addPlane(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const ModelHighAPI_Selection& thePlane,
                  const ModelHighAPI_Selection& theAxis,
                  const ModelHighAPI_Double& theAngle)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Plane::ID());
  return PlanePtr(new ConstructionAPI_Plane(aFeature, thePlane, theAxis, theAngle));
}
