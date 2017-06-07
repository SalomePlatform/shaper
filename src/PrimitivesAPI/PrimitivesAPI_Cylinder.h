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

#ifndef PRIMITIVESAPI_CYLINDER_H_
#define PRIMITIVESAPI_CYLINDER_H_

#include "PrimitivesAPI.h"

#include <PrimitivesPlugin_Cylinder.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Double;
class ModelHighAPI_Selection;

/// \class PrimitivesAPI_Box
/// \ingroup CPPHighAPI
/// \brief Interface for primitive Box feature.
class PrimitivesAPI_Cylinder: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  PRIMITIVESAPI_EXPORT
  explicit PrimitivesAPI_Cylinder(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  /*PRIMITIVESAPI_EXPORT
  explicit PrimitivesAPI_Cylinder(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const ModelHighAPI_Double& theDx,
                             const ModelHighAPI_Double& theDy,
                             const ModelHighAPI_Double& theDz);*/

  /// Constructor with values.
  PRIMITIVESAPI_EXPORT
  explicit PrimitivesAPI_Cylinder(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                  const ModelHighAPI_Selection& theBasePoint,
                                  const ModelHighAPI_Selection& theAxis,
                                  const ModelHighAPI_Double& theRadius,
                                  const ModelHighAPI_Double& theHeight);

  /// Constructor with values.
  PRIMITIVESAPI_EXPORT
  explicit PrimitivesAPI_Cylinder(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                  const ModelHighAPI_Selection& theBasePoint,
                                  const ModelHighAPI_Selection& theAxis,
                                  const ModelHighAPI_Double& theRadius,
                                  const ModelHighAPI_Double& theHeight,
                                  const ModelHighAPI_Double& theAngle);

  /// Destructor.
  PRIMITIVESAPI_EXPORT
  virtual ~PrimitivesAPI_Cylinder();

  INTERFACE_6(PrimitivesPlugin_Cylinder::ID(),
             creationMethod, PrimitivesPlugin_Cylinder::CREATION_METHOD(),
             ModelAPI_AttributeString, /** Creation method */,
             basePoint, PrimitivesPlugin_Cylinder::BASE_POINT_ID(),
             ModelAPI_AttributeSelection, /** Dimension in X */,
             axis, PrimitivesPlugin_Cylinder::AXIS_ID(),
             ModelAPI_AttributeSelection, /** Dimension in Y */,
             radius, PrimitivesPlugin_Cylinder::RADIUS_ID(),
             ModelAPI_AttributeDouble, /** Dimension in Z */,
             height, PrimitivesPlugin_Cylinder::HEIGHT_ID(),
             ModelAPI_AttributeDouble, /** First point */,
             angle, PrimitivesPlugin_Cylinder::ANGLE_ID(),
             ModelAPI_AttributeDouble, /** Second point */)

  /// Set dimensions
  PRIMITIVESAPI_EXPORT
  void setSizes(const ModelHighAPI_Double& theRadius,
                const ModelHighAPI_Double& theHeight);

  /// Dump wrapped feature
  PRIMITIVESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on primitive Box object
typedef std::shared_ptr<PrimitivesAPI_Cylinder> CylinderPtr;

/// \ingroup CPPHighAPI
/// \brief Create primitive Box feature.
PRIMITIVESAPI_EXPORT
CylinderPtr addCylinder(const std::shared_ptr<ModelAPI_Document>& thePart,
                        const ModelHighAPI_Selection& theBasePoint,
                        const ModelHighAPI_Selection& theAxis,
                        const ModelHighAPI_Double& theRadius,
                        const ModelHighAPI_Double& theHeight);

/// \ingroup CPPHighAPI
/// \brief Create primitive Box feature.
PRIMITIVESAPI_EXPORT
CylinderPtr addCylinder(const std::shared_ptr<ModelAPI_Document>& thePart,
                        const ModelHighAPI_Selection& theBasePoint,
                        const ModelHighAPI_Selection& theAxis,
                        const ModelHighAPI_Double& theRadius,
                        const ModelHighAPI_Double& theHeight,
                        const ModelHighAPI_Double& theAngle);

/// \ingroup CPPHighAPI
/// \brief Create primitive Box feature.
PRIMITIVESAPI_EXPORT
CylinderPtr addCylinder(const std::shared_ptr<ModelAPI_Document>& thePart,
                        const ModelHighAPI_Double& theRadius,
                        const ModelHighAPI_Double& theHeight);

/// \ingroup CPPHighAPI
/// \brief Create primitive Box feature.
PRIMITIVESAPI_EXPORT
CylinderPtr addCylinder(const std::shared_ptr<ModelAPI_Document>& thePart,
                        const ModelHighAPI_Double& theRadius,
                        const ModelHighAPI_Double& theHeight,
                        const ModelHighAPI_Double& theAngle);

#endif // PRIMITIVESAPI_CYLINDER_H_