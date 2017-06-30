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

#ifndef SRC_CONSTRUCTIONAPI_CONSTRUCTIONAPI_AXIS_H_
#define SRC_CONSTRUCTIONAPI_CONSTRUCTIONAPI_AXIS_H_

#include "ConstructionAPI.h"

#include <ConstructionPlugin_Axis.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Double;
class ModelHighAPI_Selection;

/// \class ConstructionAPI_Axis
/// \ingroup CPPHighAPI
/// \brief Interface for Axis feature
class ConstructionAPI_Axis: public ModelHighAPI_Interface
{
public:
  /// Constructor without values
  CONSTRUCTIONAPI_EXPORT
  explicit ConstructionAPI_Axis(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values
  CONSTRUCTIONAPI_EXPORT
  ConstructionAPI_Axis(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                       const ModelHighAPI_Selection& theObject1,
                       const ModelHighAPI_Selection& theObject2);

  /// Constructor with values
  CONSTRUCTIONAPI_EXPORT
  ConstructionAPI_Axis(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                       const ModelHighAPI_Selection& theObject);

  /// Constructor with values
  CONSTRUCTIONAPI_EXPORT
  ConstructionAPI_Axis(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                       const ModelHighAPI_Selection& thePoint,
                       const ModelHighAPI_Double& theX,
                       const ModelHighAPI_Double& theY,
                       const ModelHighAPI_Double& theZ);

  /// Constructor with values
  CONSTRUCTIONAPI_EXPORT
  ConstructionAPI_Axis(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                       const ModelHighAPI_Double& theDX,
                       const ModelHighAPI_Double& theDY,
                       const ModelHighAPI_Double& theDZ);

  /// Constructor with values
  CONSTRUCTIONAPI_EXPORT
  ConstructionAPI_Axis(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                       const ModelHighAPI_Selection& thePlane1,
                       const ModelHighAPI_Double& theOffset1,
                       const bool theReverseOffset1,
                       const ModelHighAPI_Selection& thePlane2,
                       const ModelHighAPI_Double& theOffset2,
                       const bool theReverseOffset2);

  /// Constructor with values
  CONSTRUCTIONAPI_EXPORT
  ConstructionAPI_Axis(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                       const ModelHighAPI_Selection& thePlane1,
                       const ModelHighAPI_Selection& thePlane2,
                       const ModelHighAPI_Double& theOffset2,
                       const bool theReverseOffset2);

  /// Constructor with values
  CONSTRUCTIONAPI_EXPORT
  ConstructionAPI_Axis(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                       const ModelHighAPI_Selection& thePlane1,
                       const ModelHighAPI_Double& theOffset1,
                       const bool theReverseOffset1,
                       const ModelHighAPI_Selection& thePlane2);

  /// Destructor
  CONSTRUCTIONAPI_EXPORT
  virtual ~ConstructionAPI_Axis();

  INTERFACE_21(ConstructionPlugin_Axis::ID(),
               creationMethod, ConstructionPlugin_Axis::METHOD(),
               ModelAPI_AttributeString, /** Creation method */,
               firstPoint, ConstructionPlugin_Axis::POINT_FIRST(),
               ModelAPI_AttributeSelection, /** First point */,
               secondPoint, ConstructionPlugin_Axis::POINT_SECOND(),
               ModelAPI_AttributeSelection, /** Second point */,
               cylindricalFace, ConstructionPlugin_Axis::CYLINDRICAL_FACE(),
               ModelAPI_AttributeSelection, /** Cylindrical face */,
               xDirection, ConstructionPlugin_Axis::X_DIRECTION(),
               ModelAPI_AttributeDouble, /** X direction */,
               yDirection, ConstructionPlugin_Axis::Y_DIRECTION(),
               ModelAPI_AttributeDouble, /** Y direction */,
               zDirection, ConstructionPlugin_Axis::Z_DIRECTION(),
               ModelAPI_AttributeDouble, /** Z direction */,
               xDimension, ConstructionPlugin_Axis::DX(),
               ModelAPI_AttributeDouble, /** X dimension */,
               yDimension, ConstructionPlugin_Axis::DY(),
               ModelAPI_AttributeDouble, /** Y dimension */,
               zDimension, ConstructionPlugin_Axis::DZ(),
               ModelAPI_AttributeDouble, /** Z dimension */,
               line, ConstructionPlugin_Axis::LINE(),
               ModelAPI_AttributeSelection, /** Line */,
               plane, ConstructionPlugin_Axis::PLANE(),
               ModelAPI_AttributeSelection, /** Plane */,
               point, ConstructionPlugin_Axis::POINT(),
               ModelAPI_AttributeSelection, /** Point */,
               plane1, ConstructionPlugin_Axis::PLANE1(),
               ModelAPI_AttributeSelection, /** Plane 1 */,
               useOffset1, ConstructionPlugin_Axis::USE_OFFSET1(),
               ModelAPI_AttributeString, /** Use offset 1 */,
               offset1, ConstructionPlugin_Axis::OFFSET1(),
               ModelAPI_AttributeDouble, /** Offset 1 */,
               reverseOffset1, ConstructionPlugin_Axis::REVERSE_OFFSET1(),
               ModelAPI_AttributeBoolean, /** Reverse offset 1 */,
               plane2, ConstructionPlugin_Axis::PLANE2(),
               ModelAPI_AttributeSelection, /** Plane 2 */,
               useOffset2, ConstructionPlugin_Axis::USE_OFFSET2(),
               ModelAPI_AttributeString, /** Use offset 2 */,
               offset2, ConstructionPlugin_Axis::OFFSET2(),
               ModelAPI_AttributeDouble, /** Offset 2 */,
               reverseOffset2, ConstructionPlugin_Axis::REVERSE_OFFSET2(),
               ModelAPI_AttributeBoolean, /** Reverse offset 2 */)

  /// Set points
  CONSTRUCTIONAPI_EXPORT
  void setByPoints(const ModelHighAPI_Selection& thePoint1,
                   const ModelHighAPI_Selection& thePoint2);

  /// Set cylindrical face
  CONSTRUCTIONAPI_EXPORT
  void setByCylindricalFace(const ModelHighAPI_Selection& theCylindricalFace);

  /// Set direction
  CONSTRUCTIONAPI_EXPORT
  void setByPointAndDirection(const ModelHighAPI_Selection& thePoint,
                              const ModelHighAPI_Double& theX,
                              const ModelHighAPI_Double& theY,
                              const ModelHighAPI_Double& theZ);

  /// Set dimensions
  CONSTRUCTIONAPI_EXPORT
  void setByDimensions(const ModelHighAPI_Double& theDX,
                       const ModelHighAPI_Double& theDY,
                       const ModelHighAPI_Double& theDZ);

  /// Set by line
  CONSTRUCTIONAPI_EXPORT
  void setByLine(const ModelHighAPI_Selection& theCylindricalFace);

  /// Set by plane and point
  CONSTRUCTIONAPI_EXPORT
  void setByPlaneAndPoint(const ModelHighAPI_Selection& thePlane,
                          const ModelHighAPI_Selection& thePoint);

  /// Set by two planes
  CONSTRUCTIONAPI_EXPORT
  void setByTwoPlanes(const ModelHighAPI_Selection& thePlane1,
                      const ModelHighAPI_Selection& thePlane2);

  /// Set by two planes
  CONSTRUCTIONAPI_EXPORT
  void setByTwoPlanes(const ModelHighAPI_Selection& thePlane1,
                      const ModelHighAPI_Double& theOffset1,
                      const bool theReverseOffset1,
                      const ModelHighAPI_Selection& thePlane2,
                      const ModelHighAPI_Double& theOffset2,
                      const bool theReverseOffset2);

  /// Set by two planes
  CONSTRUCTIONAPI_EXPORT
  void setByTwoPlanes(const ModelHighAPI_Selection& thePlane1,
                      const ModelHighAPI_Selection& thePlane2,
                      const ModelHighAPI_Double& theOffset2,
                      const bool theReverseOffset2);

  /// Set by two planes
  CONSTRUCTIONAPI_EXPORT
  void setByTwoPlanes(const ModelHighAPI_Selection& thePlane1,
                      const ModelHighAPI_Double& theOffset1,
                      const bool theReverseOffset1,
                      const ModelHighAPI_Selection& thePlane2);

  /// Dump wrapped feature
  CONSTRUCTIONAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Axis object
typedef std::shared_ptr<ConstructionAPI_Axis> AxisPtr;

/// \ingroup CPPHighAPI
/// \brief Create Axis feature
CONSTRUCTIONAPI_EXPORT
AxisPtr addAxis(const std::shared_ptr<ModelAPI_Document>& thePart,
                const ModelHighAPI_Selection& theObject1,
                const ModelHighAPI_Selection& theObject2);

/// \ingroup CPPHighAPI
/// \brief Create Axis feature
CONSTRUCTIONAPI_EXPORT
AxisPtr addAxis(const std::shared_ptr<ModelAPI_Document>& thePart,
                const ModelHighAPI_Selection& theObject);

/// \ingroup CPPHighAPI
/// \brief Create Axis feature
CONSTRUCTIONAPI_EXPORT
AxisPtr addAxis(const std::shared_ptr<ModelAPI_Document>& thePart,
                const ModelHighAPI_Selection& thePoint,
                const ModelHighAPI_Double& theX,
                const ModelHighAPI_Double& theY,
                const ModelHighAPI_Double& theZ);

/// \ingroup CPPHighAPI
/// \brief Create Axis feature
CONSTRUCTIONAPI_EXPORT
AxisPtr addAxis(const std::shared_ptr<ModelAPI_Document>& thePart,
                const ModelHighAPI_Double& theDX,
                const ModelHighAPI_Double& theDY,
                const ModelHighAPI_Double& theDZ);

/// \ingroup CPPHighAPI
/// \brief Create Axis feature
CONSTRUCTIONAPI_EXPORT
AxisPtr addAxis(const std::shared_ptr<ModelAPI_Document>& thePart,
                const ModelHighAPI_Selection& thePlane1,
                const ModelHighAPI_Double& theOffset1,
                const bool theReverseOffset1,
                const ModelHighAPI_Selection& thePlane2,
                const ModelHighAPI_Double& theOffset2,
                const bool theReverseOffset2);

/// \ingroup CPPHighAPI
/// \brief Create Axis feature
CONSTRUCTIONAPI_EXPORT
AxisPtr addAxis(const std::shared_ptr<ModelAPI_Document>& thePart,
                const ModelHighAPI_Selection& thePlane1,
                const ModelHighAPI_Selection& thePlane2,
                const ModelHighAPI_Double& theOffset2,
                const bool theReverseOffset2);

/// \ingroup CPPHighAPI
/// \brief Create Axis feature
CONSTRUCTIONAPI_EXPORT
AxisPtr addAxis(const std::shared_ptr<ModelAPI_Document>& thePart,
                const ModelHighAPI_Selection& thePlane1,
                const ModelHighAPI_Double& theOffset1,
                const bool theReverseOffset1,
                const ModelHighAPI_Selection& thePlane2);

#endif /* SRC_CONSTRUCTIONAPI_CONSTRUCTIONAPI_AXIS_H_ */
