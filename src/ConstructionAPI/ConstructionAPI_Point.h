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

#ifndef SRC_CONSTRUCTIONAPI_CONSTRUCTIONAPI_POINT_H_
#define SRC_CONSTRUCTIONAPI_CONSTRUCTIONAPI_POINT_H_

#include "ConstructionAPI.h"

#include <ConstructionPlugin_Point.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelAPI_AttributeDouble;
class ModelAPI_Document;
class ModelHighAPI_Double;

/// \class ConstructionAPI_Point
/// \ingroup CPPHighAPI
/// \brief Interface for Point feature.
class ConstructionAPI_Point: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  CONSTRUCTIONAPI_EXPORT
  explicit ConstructionAPI_Point(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  CONSTRUCTIONAPI_EXPORT
  ConstructionAPI_Point(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                        const ModelHighAPI_Double& theX,
                        const ModelHighAPI_Double& theY,
                        const ModelHighAPI_Double& theZ);

  /// Constructor with values.
  CONSTRUCTIONAPI_EXPORT
  ConstructionAPI_Point(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                        const ModelHighAPI_Selection& theEdge,
                        const ModelHighAPI_Double& theOffset,
                        const bool theUseRatio = false,
                        const bool theReverse = false);

  /// Constructor with values: intersected objects.
  CONSTRUCTIONAPI_EXPORT
  ConstructionAPI_Point(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                        const ModelHighAPI_Selection& theObject1,
                        const ModelHighAPI_Selection& theObject2);

  /// Destructor.
  CONSTRUCTIONAPI_EXPORT
  virtual ~ConstructionAPI_Point();

  INTERFACE_14(ConstructionPlugin_Point::ID(),
               x, ConstructionPlugin_Point::X(), ModelAPI_AttributeDouble, /** X attribute */,
               y, ConstructionPlugin_Point::Y(), ModelAPI_AttributeDouble, /** Y attribute */,
               z, ConstructionPlugin_Point::Z(), ModelAPI_AttributeDouble, /** Z attribute */,
               creationMethod, ConstructionPlugin_Point::CREATION_METHOD(),
               ModelAPI_AttributeString, /** Creation method */,
               intersectionLine, ConstructionPlugin_Point::INTERSECTION_LINE(),
               ModelAPI_AttributeSelection, /** Line for intersection */,
               intersectionPlane, ConstructionPlugin_Point::INTERSECTION_PLANE(),
               ModelAPI_AttributeSelection, /** Plane for intersection */,
               useOffset, ConstructionPlugin_Point::USE_OFFSET(),
               ModelAPI_AttributeString, /** Use offset */,
               offset, ConstructionPlugin_Point::OFFSET(),
               ModelAPI_AttributeDouble, /** Offset */,
               reverseOffset, ConstructionPlugin_Point::REVERSE_OFFSET(),
               ModelAPI_AttributeBoolean, /** Reverse offset */,
               edge, ConstructionPlugin_Point::EDGE(),
               ModelAPI_AttributeSelection, /** Edge */,
               offsetType, ConstructionPlugin_Point::OFFSET_TYPE(),
               ModelAPI_AttributeString, /** Type of the offset on edge */,
               distance, ConstructionPlugin_Point::DISTANCE(),
               ModelAPI_AttributeDouble, /** Distance */,
               ratio, ConstructionPlugin_Point::RATIO(),
               ModelAPI_AttributeDouble, /** Ratio */,
               reverse, ConstructionPlugin_Point::REVERSE(),
               ModelAPI_AttributeBoolean, /** Reverse */)


  /// Set point values.
  CONSTRUCTIONAPI_EXPORT
  void setByXYZ(const ModelHighAPI_Double & theX,
                const ModelHighAPI_Double & theY,
                const ModelHighAPI_Double & theZ);

  /// Set edge and distance on it for point.
  CONSTRUCTIONAPI_EXPORT
  void setByOffsetOnEdge(const ModelHighAPI_Selection& theEdge,
                         const ModelHighAPI_Double& theOffset,
                         const bool theUseRatio = false,
                         const bool theReverse = false);

  /*
  /// Set point and plane for projection.
  CONSTRUCTIONAPI_EXPORT
  void setByProjection(const ModelHighAPI_Selection& theVertex,
                       const ModelHighAPI_Selection& theFace);

  /// Set lines for intersections.
  CONSTRUCTIONAPI_EXPORT
  void setByLinesIntersection(const ModelHighAPI_Selection& theEdge1,
                              const ModelHighAPI_Selection& theEdge2);
  */
  /// Set line and plane for intersections.
  CONSTRUCTIONAPI_EXPORT
  void setByLineAndPlaneIntersection(const ModelHighAPI_Selection& theEdge,
                                     const ModelHighAPI_Selection& theFace);

  /// Dump wrapped feature
  CONSTRUCTIONAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Point object.
typedef std::shared_ptr<ConstructionAPI_Point> PointPtr;

/// \ingroup CPPHighAPI
/// \brief Create Point feature
CONSTRUCTIONAPI_EXPORT
PointPtr addPoint(const std::shared_ptr<ModelAPI_Document> & thePart,
                  const ModelHighAPI_Double & theX,
                  const ModelHighAPI_Double & theY,
                  const ModelHighAPI_Double & theZ);

/// \ingroup CPPHighAPI
/// \brief Create Point feature
CONSTRUCTIONAPI_EXPORT
PointPtr addPoint(const std::shared_ptr<ModelAPI_Document> & thePart,
                  const ModelHighAPI_Selection& theEdge,
                  const ModelHighAPI_Double& theOffset,
                  const bool theUseRatio = false,
                  const bool theReverse = false);

/// \ingroup CPPHighAPI
/// \brief Create Point feature as an intersection of selected plane (or planar face) and edge
CONSTRUCTIONAPI_EXPORT
PointPtr addPoint(const std::shared_ptr<ModelAPI_Document> & thePart,
                  const ModelHighAPI_Selection& theObject1,
                  const ModelHighAPI_Selection& theObject2);

/// \ingroup CPPHighAPI
/// \brief Create Point feature as an intersection of selected plane (or planar face) and edge
/// with positive distance from the plane and flag to reverse the offset direction.
CONSTRUCTIONAPI_EXPORT
PointPtr addPoint(const std::shared_ptr<ModelAPI_Document> & thePart,
                  const ModelHighAPI_Selection& theObject1,
                  const ModelHighAPI_Selection& theObject2,
                  const ModelHighAPI_Double& theDistanceValue,
                  const bool theReverse = false);

#endif /* SRC_CONSTRUCTIONAPI_CONSTRUCTIONAPI_POINT_H_ */
