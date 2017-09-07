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
}*/

//==================================================================================================
ConstructionAPI_Point::ConstructionAPI_Point(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const ModelHighAPI_Selection& theObject1,
                                             const ModelHighAPI_Selection& theObject2)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    GeomAPI_Shape::ShapeType aType1 = getShapeType(theObject1);
    GeomAPI_Shape::ShapeType aType2 = getShapeType(theObject2);
    /*
    if(aType1 == GeomAPI_Shape::VERTEX && aType2 == GeomAPI_Shape::FACE) {
      // If first object is vertex and second object is face then set by projection.
      setByProjection(theObject1, theObject2);
    } else if(aType1 == GeomAPI_Shape::EDGE && aType2 == GeomAPI_Shape::EDGE) {
      // If both objects are edges then set by lines intersection.
      setByLinesIntersection(theObject1, theObject2);
    } else */if(aType1 == GeomAPI_Shape::EDGE && aType2 == GeomAPI_Shape::FACE) {
      // If first object is edge and second object is face then set by line and plane intersection.
      setByLineAndPlaneIntersection(theObject1, theObject2);
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
}*/

//==================================================================================================
void ConstructionAPI_Point::setByLineAndPlaneIntersection(const ModelHighAPI_Selection& theEdge,
                                                          const ModelHighAPI_Selection& theFace)
{
  fillAttribute(
    ConstructionPlugin_Point::CREATION_METHOD_BY_LINE_AND_PLANE_INTERSECTION(), mycreationMethod);
  fillAttribute(theEdge, myintersectionLine);
  fillAttribute(theFace, myintersectionPlane);
  fillAttribute("", useOffset()); // not used by default
  execute();
}

//==================================================================================================
void ConstructionAPI_Point::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());
  const std::string& aMeth = creationMethod()->value();

  // common part
  theDumper << aBase << " = model.addPoint(" << aDocName << ", ";

  if (aMeth == "" || // default is XYZ
      aMeth == ConstructionPlugin_Point::CREATION_METHOD_BY_XYZ()) {
    theDumper << x() << ", " << y() << ", " << z() << ")" << std::endl;
  } else if (aMeth == ConstructionPlugin_Point::CREATION_METHOD_BY_LINE_AND_PLANE_INTERSECTION()) {
    theDumper << intersectionLine() << ", " <<intersectionPlane() ;
    if (!useOffset()->value().empty()) { // call method with defined offset
      theDumper << ", " << offset() << ", " << reverseOffset();
    }
    theDumper << ")" << std::endl;
  }
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

/*//==================================================================================================
PointPtr addPoint(const std::shared_ptr<ModelAPI_Document> & thePart,
                  const ModelHighAPI_Selection& theEdge,
                  const ModelHighAPI_Double& theDistanceValue,
                  const bool theDistancePercent,
                  const bool theReverse)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Point::ID());
  return PointPtr(new ConstructionAPI_Point(aFeature, theEdge, theDistanceValue, theDistancePercent, theReverse));
}*/

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
