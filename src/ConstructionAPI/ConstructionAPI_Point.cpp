// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "ConstructionAPI_Point.h"

#include <GeomAPI_Shape.h>

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
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
                                             const ModelHighAPI_Double& theOffset,
                                             const bool theUseRatio,
                                             const bool theReverse)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setByOffsetOnEdge(theEdge, theOffset, theUseRatio, theReverse);
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
      setByProjectionOnFace(theObject1, theObject2);
    } else if (aType1 == GeomAPI_Shape::VERTEX && aType2 == GeomAPI_Shape::EDGE) {
      // If first object is vertex and second object is edge then set by projection.
      setByProjectionOnEdge(theObject1, theObject2);
    } else if(aType1 == GeomAPI_Shape::EDGE && aType2 == GeomAPI_Shape::EDGE) {
      // If both objects are edges then set by lines intersection.
      setByLinesIntersection(theObject1, theObject2);
    } else if(aType1 == GeomAPI_Shape::EDGE && aType2 == GeomAPI_Shape::FACE) {
      // If first object is edge and second object is face then set by line and plane intersection.
      setByLineAndPlaneIntersection(theObject1, theObject2);
    }
  }
}

//==================================================================================================
ConstructionAPI_Point::ConstructionAPI_Point(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const ModelHighAPI_Selection& theObject1,
                                             const ModelHighAPI_Selection& theObject2,
                                             const ModelHighAPI_Selection& theObject3)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize())
  {
    GeomAPI_Shape::ShapeType aType1 = getShapeType(theObject1);
    GeomAPI_Shape::ShapeType aType2 = getShapeType(theObject2);
    GeomAPI_Shape::ShapeType aType3 = getShapeType(theObject3);
    if (aType1 == GeomAPI_Shape::FACE
        && aType2 == GeomAPI_Shape::FACE
        && aType3 == GeomAPI_Shape::FACE)
    {
      setByPlanesIntersection(theObject1, theObject2, theObject3);
    }
  }
}

//==================================================================================================
ConstructionAPI_Point::ConstructionAPI_Point(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const ModelHighAPI_Selection& theObject,
                                             const bool theIsCircularEdge)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize())
  {
    if (theIsCircularEdge) {
      setByCenterOfCircle(theObject);
    } else {
      setByCenterOfGravity(theObject);
    }
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
  //fillAttribute(ConstructionPlugin_Point::CREATION_METHOD_BY_XYZ(), mycreationMethod);
  fillAttribute(theX, myx);
  fillAttribute(theY, myy);
  fillAttribute(theZ, myz);
  fillAttribute(ConstructionPlugin_Point::CREATION_METHOD_BY_XYZ(), mycreationMethod);

  execute(false);
}

//==================================================================================================
void ConstructionAPI_Point::setByOffsetOnEdge(const ModelHighAPI_Selection& theEdge,
                                              const ModelHighAPI_Double& theOffset,
                                              const bool theUseRatio,
                                              const bool theReverse)
{
  fillAttribute(ConstructionPlugin_Point::CREATION_METHOD_BY_DISTANCE_ON_EDGE(), mycreationMethod);
  fillAttribute(theEdge, myedge);
  if (theUseRatio) {
    fillAttribute(ConstructionPlugin_Point::OFFSET_TYPE_BY_RATIO(), myoffsetType);
    fillAttribute(theOffset, myratio);
  }
  else {
    fillAttribute(ConstructionPlugin_Point::OFFSET_TYPE_BY_DISTANCE(), myoffsetType);
    fillAttribute(theOffset, mydistance);
  }
  fillAttribute(theReverse, myreverse);

  execute();
}

//==================================================================================================
void ConstructionAPI_Point::setByProjectionOnEdge(const ModelHighAPI_Selection& theVertex,
                                                  const ModelHighAPI_Selection& theEdge)
{
  fillAttribute(ConstructionPlugin_Point::CREATION_METHOD_BY_PROJECTION(),
                mycreationMethod);
  fillAttribute(ConstructionPlugin_Point::PROJECTION_TYPE_ON_EDGE(),
                myprojectionType);
  fillAttribute(theVertex, mypoinToProject);
  fillAttribute(theEdge, myedgeForPointProjection);

  execute();
}

//==================================================================================================
void ConstructionAPI_Point::setByProjectionOnFace(const ModelHighAPI_Selection& theVertex,
                                                  const ModelHighAPI_Selection& theFace)
{
  fillAttribute(ConstructionPlugin_Point::CREATION_METHOD_BY_PROJECTION(),
                mycreationMethod);
  fillAttribute(ConstructionPlugin_Point::PROJECTION_TYPE_ON_FACE(),
                myprojectionType);
  fillAttribute(theVertex, mypoinToProject);
  fillAttribute(theFace, myfaceForPointProjection);

  execute();
}

//==================================================================================================
void ConstructionAPI_Point::setByLinesIntersection(const ModelHighAPI_Selection& theEdge1,
                                                   const ModelHighAPI_Selection& theEdge2)
{
  fillAttribute(ConstructionPlugin_Point::CREATION_METHOD_BY_INTERSECTION(), mycreationMethod);
  fillAttribute(ConstructionPlugin_Point::INTERSECTION_TYPE_BY_LINES(),
                myintersectionType);
  fillAttribute(theEdge1, myintersectionLine1);
  fillAttribute(theEdge2, myintersectionLine2);

  execute();
}

//==================================================================================================
void ConstructionAPI_Point::setByLineAndPlaneIntersection(const ModelHighAPI_Selection& theEdge,
                                                          const ModelHighAPI_Selection& theFace)
{
  fillAttribute(ConstructionPlugin_Point::CREATION_METHOD_BY_INTERSECTION(), mycreationMethod);
  fillAttribute(ConstructionPlugin_Point::INTERSECTION_TYPE_BY_LINE_AND_PLANE(),
                myintersectionType);
  fillAttribute(theEdge, myintersectionLine);
  fillAttribute(theFace, myintersectionPlane);
  fillAttribute("", useOffset()); // not used by default
  execute();
}

//==================================================================================================
void ConstructionAPI_Point::setByPlanesIntersection(const ModelHighAPI_Selection& theFace1,
                                                    const ModelHighAPI_Selection& theFace2,
                                                    const ModelHighAPI_Selection& theFace3)
{
  fillAttribute(ConstructionPlugin_Point::CREATION_METHOD_BY_INTERSECTION(), mycreationMethod);
  fillAttribute(ConstructionPlugin_Point::INTERSECTION_TYPE_BY_PLANES(),
                myintersectionType);
  fillAttribute(theFace1, myintersectionPlane1);
  fillAttribute(theFace2, myintersectionPlane2);
  fillAttribute(theFace3, myintersectionPlane3);

  execute();
}

//==================================================================================================
void ConstructionAPI_Point::setByCenterOfGravity(const ModelHighAPI_Selection& theObject)
{
  fillAttribute(ConstructionPlugin_Point::CREATION_METHOD_BY_GEOMETRICAL_PROPERTY(),
                mycreationMethod);
  fillAttribute(ConstructionPlugin_Point::GEOMETRICAL_PROPERTY_TYPE_BY_CENTER_OF_GRAVITY(),
                mygeometricalPropertyType);
  fillAttribute(theObject, myobjectForCenterOfGravity);
}

//==================================================================================================
void ConstructionAPI_Point::setByCenterOfCircle(const ModelHighAPI_Selection& theObject)
{
  fillAttribute(ConstructionPlugin_Point::CREATION_METHOD_BY_GEOMETRICAL_PROPERTY(),
                mycreationMethod);
  fillAttribute(ConstructionPlugin_Point::GEOMETRICAL_PROPERTY_TYPE_BY_CENTER_OF_CIRCLE(),
                mygeometricalPropertyType);
  fillAttribute(theObject, myobjectForCenterOfCircle);
}

//==================================================================================================
void ConstructionAPI_Point::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());
  const std::string aMeth = creationMethod()->value();

  // common part
  theDumper << aBase << " = model.addPoint(" << aDocName << ", ";

  if (aMeth == "" || // default is XYZ
      aMeth == ConstructionPlugin_Point::CREATION_METHOD_BY_XYZ()) {
    theDumper << x() << ", " << y() << ", " << z();
  } else if (aMeth == ConstructionPlugin_Point::CREATION_METHOD_BY_INTERSECTION()) {
    const std::string anIntersectionType = intersectionType()->value();
    if (anIntersectionType == ConstructionPlugin_Point::INTERSECTION_TYPE_BY_LINES())
    {
      theDumper << intersectionLine1() << ", " << intersectionLine2();
    }
    else if (anIntersectionType == ConstructionPlugin_Point::INTERSECTION_TYPE_BY_LINE_AND_PLANE())
    {
      theDumper << intersectionLine() << ", " << intersectionPlane();
      if (!useOffset()->value().empty()) { // call method with defined offset
        theDumper << ", " << offset() << ", " << reverseOffset();
      }
    }
    else if (anIntersectionType == ConstructionPlugin_Point::INTERSECTION_TYPE_BY_PLANES())
    {
      theDumper << intersectionPlane1() << ", "
                << intersectionPlane2() << ", "
                << intersectionPlane3();
    }
  } else if (aMeth == ConstructionPlugin_Point::CREATION_METHOD_BY_DISTANCE_ON_EDGE()) {
    theDumper << edge() << ", ";
    if (offsetType()->value() == ConstructionPlugin_Point::OFFSET_TYPE_BY_DISTANCE()) {
      theDumper << distance() << ", " << false;
    }
    else {
      theDumper << ratio() << ", " << true;
    }
    theDumper << ", " << reverse()->value();
  } else if (aMeth == ConstructionPlugin_Point::CREATION_METHOD_BY_PROJECTION()) {
    theDumper << poinToProject() << ", ";
    if (projectionType()->value() == ConstructionPlugin_Point::PROJECTION_TYPE_ON_EDGE()) {
      theDumper << edgeForPointProjection();
    } else {
      theDumper << faceForPointProjection();
    }
  } else if (aMeth == ConstructionPlugin_Point::CREATION_METHOD_BY_GEOMETRICAL_PROPERTY()) {
    if (geometricalPropertyType()->value() ==
          ConstructionPlugin_Point::GEOMETRICAL_PROPERTY_TYPE_BY_CENTER_OF_GRAVITY())
    {
      theDumper << objectForCenterOfGravity();
    }
    else
    {
      theDumper << objectForCenterOfCircle() << ", " << true;
    }
  }

  theDumper << ")" << std::endl;
}

//==================================================================================================
PointPtr addPoint(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const ModelHighAPI_Double& theX,
                  const ModelHighAPI_Double& theY,
                  const ModelHighAPI_Double& theZ)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Point::ID());
  return PointPtr(new ConstructionAPI_Point(aFeature, theX, theY, theZ));
}

//==================================================================================================
PointPtr addPoint(const std::shared_ptr<ModelAPI_Document> & thePart,
                  const ModelHighAPI_Selection& theEdge,
                  const ModelHighAPI_Double& theOffset,
                  const bool theUseRatio,
                  const bool theReverse)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Point::ID());
  return PointPtr(new ConstructionAPI_Point(aFeature, theEdge, theOffset, theUseRatio, theReverse));
}

//==================================================================================================
PointPtr addPoint(const std::shared_ptr<ModelAPI_Document> & thePart,
                  const ModelHighAPI_Selection& theObject1,
                  const ModelHighAPI_Selection& theObject2)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Point::ID());
  return PointPtr(new ConstructionAPI_Point(aFeature, theObject1, theObject2));
}

//==================================================================================================
PointPtr addPoint(const std::shared_ptr<ModelAPI_Document> & thePart,
                  const ModelHighAPI_Selection& theObject1,
                  const ModelHighAPI_Selection& theObject2,
                  const ModelHighAPI_Double& theDistanceValue,
                  const bool theReverse)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Point::ID());
  PointPtr anAPI(new ConstructionAPI_Point(aFeature, theObject1, theObject2));

  fillAttribute(ConstructionPlugin_Point::USE_OFFSET(), anAPI->useOffset());
  fillAttribute(theDistanceValue, anAPI->offset());
  fillAttribute(theReverse, anAPI->reverseOffset());

  return anAPI;
}

//==================================================================================================
PointPtr addPoint(const std::shared_ptr<ModelAPI_Document> & thePart,
                  const ModelHighAPI_Selection& theObject1,
                  const ModelHighAPI_Selection& theObject2,
                  const ModelHighAPI_Selection& theObject3)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Point::ID());
  return PointPtr(new ConstructionAPI_Point(aFeature, theObject1, theObject2, theObject3));
}

//==================================================================================================
PointPtr addPoint(const std::shared_ptr<ModelAPI_Document> & thePart,
                  const ModelHighAPI_Selection& theObject,
                  const bool theIsCircularEdge)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Point::ID());
  return PointPtr(new ConstructionAPI_Point(aFeature, theObject, theIsCircularEdge));
}
