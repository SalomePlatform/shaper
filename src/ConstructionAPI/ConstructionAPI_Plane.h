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

#ifndef SRC_CONSTRUCTIONAPI_CONSTRUCTIONAPI_PLANE_H_
#define SRC_CONSTRUCTIONAPI_CONSTRUCTIONAPI_PLANE_H_

#include "ConstructionAPI.h"

#include <ConstructionPlugin_Plane.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Double;
class ModelHighAPI_Selection;

/// \class ConstructionAPI_Plane
/// \ingroup CPPHighAPI
/// \brief Interface for Plane feature
class ConstructionAPI_Plane: public ModelHighAPI_Interface
{
public:
  /// Constructor without values
  CONSTRUCTIONAPI_EXPORT
  explicit ConstructionAPI_Plane(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values
  CONSTRUCTIONAPI_EXPORT
  ConstructionAPI_Plane(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                        const ModelHighAPI_Selection& theFace,
                        const ModelHighAPI_Double& theDistance,
                        const bool theIsReverse);

  /// Constructor with values
  CONSTRUCTIONAPI_EXPORT
  ConstructionAPI_Plane(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                        const ModelHighAPI_Double& theA,
                        const ModelHighAPI_Double& theB,
                        const ModelHighAPI_Double& theC,
                        const ModelHighAPI_Double& theD);

  /// Constructor with values
  CONSTRUCTIONAPI_EXPORT
  ConstructionAPI_Plane(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                        const ModelHighAPI_Selection& thePoint1,
                        const ModelHighAPI_Selection& thePoint2,
                        const ModelHighAPI_Selection& thePoint3);

  /// Constructor with values
  CONSTRUCTIONAPI_EXPORT
  ConstructionAPI_Plane(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                        const ModelHighAPI_Selection& theLine,
                        const ModelHighAPI_Selection& thePoint,
                        const bool theIsPerpendicular);

  /// Constructor with values
  CONSTRUCTIONAPI_EXPORT
  ConstructionAPI_Plane(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                        const ModelHighAPI_Selection& theObject1,
                        const ModelHighAPI_Selection& theObject2);

  /// Constructor with values
  CONSTRUCTIONAPI_EXPORT
  ConstructionAPI_Plane(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                        const ModelHighAPI_Selection& thePlane,
                        const ModelHighAPI_Selection& theAxis,
                        const ModelHighAPI_Double& theAngle);

  /// Destructor
  CONSTRUCTIONAPI_EXPORT
  virtual ~ConstructionAPI_Plane();

  INTERFACE_20(ConstructionPlugin_Plane::ID(),
               creationMethod, ConstructionPlugin_Plane::CREATION_METHOD(),
               ModelAPI_AttributeString, /** Creation method */,
               A, ConstructionPlugin_Plane::A(),
               ModelAPI_AttributeDouble, /** Parameter A for general equation */,
               B, ConstructionPlugin_Plane::B(),
               ModelAPI_AttributeDouble, /** Parameter B for general equation */,
               C, ConstructionPlugin_Plane::C(),
               ModelAPI_AttributeDouble, /** Parameter C for general equation */,
               D, ConstructionPlugin_Plane::D(),
               ModelAPI_AttributeDouble, /** Parameter D for general equation */,
               point1, ConstructionPlugin_Plane::POINT1(),
               ModelAPI_AttributeSelection, /** Point 1 for plane */,
               point2, ConstructionPlugin_Plane::POINT2(),
               ModelAPI_AttributeSelection, /** Point 2 for plane */,
               point3, ConstructionPlugin_Plane::POINT3(),
               ModelAPI_AttributeSelection, /** Point 3 for plane */,
               line, ConstructionPlugin_Plane::LINE(),
               ModelAPI_AttributeSelection, /** Line for plane */,
               point, ConstructionPlugin_Plane::POINT(),
               ModelAPI_AttributeSelection, /** Point for plane */,
               perpendicular, ConstructionPlugin_Plane::PERPENDICULAR(),
               ModelAPI_AttributeBoolean, /** Perpendicular flag */,
               creationMethodByOtherPlane,
               ConstructionPlugin_Plane::CREATION_METHOD_BY_OTHER_PLANE_OPTION(),
               ModelAPI_AttributeString, /** Creation method  by other plane*/,
               plane, ConstructionPlugin_Plane::PLANE(),
               ModelAPI_AttributeSelection, /** Plane face */,
               distance, ConstructionPlugin_Plane::DISTANCE(),
               ModelAPI_AttributeDouble, /** Distance */,
               reverse, ConstructionPlugin_Plane::REVERSE(),
               ModelAPI_AttributeBoolean, /** Reverse flag */,
               coincidentPoint, ConstructionPlugin_Plane::COINCIDENT_POINT(),
               ModelAPI_AttributeSelection, /** Coincident point */,
               axis, ConstructionPlugin_Plane::AXIS(),
               ModelAPI_AttributeSelection, /** Axis for rotation */,
               angle, ConstructionPlugin_Plane::ANGLE(),
               ModelAPI_AttributeDouble, /** Rotation angle */,
               plane1, ConstructionPlugin_Plane::PLANE1(),
               ModelAPI_AttributeSelection, /** Plane 1 */,
               plane2, ConstructionPlugin_Plane::PLANE2(),
               ModelAPI_AttributeSelection, /** Plane 2 */)

  /// Set face and distance
  CONSTRUCTIONAPI_EXPORT
  void setByFaceAndDistance(const ModelHighAPI_Selection& theFace,
                            const ModelHighAPI_Double& theDistance,
                            const bool theIsReverse);

  /// Set GeneralEquation parameters of the feature
  CONSTRUCTIONAPI_EXPORT
  void setByGeneralEquation(const ModelHighAPI_Double& theA,
                            const ModelHighAPI_Double& theB,
                            const ModelHighAPI_Double& theC,
                            const ModelHighAPI_Double& theD);

  /// Set by three points.
  CONSTRUCTIONAPI_EXPORT
  void setByThreePoints(const ModelHighAPI_Selection& thePoint1,
                        const ModelHighAPI_Selection& thePoint2,
                        const ModelHighAPI_Selection& thePoint3);

  /// Set by line and point.
  CONSTRUCTIONAPI_EXPORT
  void setByLineAndPoint(const ModelHighAPI_Selection& theLine,
                         const ModelHighAPI_Selection& thePoint,
                         const bool theIsPerpendicular);

  /// Set by two parallel planes.
  CONSTRUCTIONAPI_EXPORT
  void setByTwoParallelPlanes(const ModelHighAPI_Selection& thePlane1,
                              const ModelHighAPI_Selection& thePlane2);

  /// Set by coincident to point.
  CONSTRUCTIONAPI_EXPORT
  void setByCoincidentToPoint(const ModelHighAPI_Selection& thePlane,
                              const ModelHighAPI_Selection& thePoint);

  /// Set by rotation.
  CONSTRUCTIONAPI_EXPORT
  void setByRotation(const ModelHighAPI_Selection& thePlane,
                     const ModelHighAPI_Selection& theAxis,
                     const ModelHighAPI_Double& theAngle);

  /// Dump wrapped feature
  CONSTRUCTIONAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Plane object
typedef std::shared_ptr<ConstructionAPI_Plane> PlanePtr;

/// \ingroup CPPHighAPI
/// \brief Create Plane feature
CONSTRUCTIONAPI_EXPORT
PlanePtr addPlane(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const ModelHighAPI_Selection& theFace,
                  const ModelHighAPI_Double& theDistance,
                  const bool theIsReverse);

/// \ingroup CPPHighAPI
/// \brief Create Plane feature
CONSTRUCTIONAPI_EXPORT
PlanePtr addPlane(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const ModelHighAPI_Double& theA,
                  const ModelHighAPI_Double& theB,
                  const ModelHighAPI_Double& theC,
                  const ModelHighAPI_Double& theD);

/// \ingroup CPPHighAPI
/// \brief Create Plane feature
CONSTRUCTIONAPI_EXPORT
PlanePtr addPlane(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const ModelHighAPI_Selection& thePoint1,
                  const ModelHighAPI_Selection& thePoint2,
                  const ModelHighAPI_Selection& thePoint3);

/// \ingroup CPPHighAPI
/// \brief Create Plane feature
CONSTRUCTIONAPI_EXPORT
PlanePtr addPlane(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const ModelHighAPI_Selection& theLine,
                  const ModelHighAPI_Selection& thePoint,
                  const bool theIsPerpendicular);

/// \ingroup CPPHighAPI
/// \brief Create Plane feature
CONSTRUCTIONAPI_EXPORT
PlanePtr addPlane(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const ModelHighAPI_Selection& theObject1,
                  const ModelHighAPI_Selection& theObject2);

/// \ingroup CPPHighAPI
/// \brief Create Plane feature
CONSTRUCTIONAPI_EXPORT
PlanePtr addPlane(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const ModelHighAPI_Selection& thePlane,
                  const ModelHighAPI_Selection& theAxis,
                  const ModelHighAPI_Double& theAngle);

#endif /* SRC_CONSTRUCTIONAPI_CONSTRUCTIONAPI_PLANE_H_ */
