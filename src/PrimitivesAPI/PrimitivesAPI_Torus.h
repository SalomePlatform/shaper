// Copyright (C) 2017-2020  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// File:        PrimitivesAPI_Torus.h
// Created:     20 Mar 2017
// Author:      Clarisse Genrault

#ifndef PRIMITIVESAPI_TORUS_H_
#define PRIMITIVESAPI_TORUS_H_

#include "PrimitivesAPI.h"

#include <PrimitivesPlugin_Torus.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Double;
class ModelHighAPI_Selection;

/// \class PrimitivesAPI_Torus
/// \ingroup CPPHighAPI
/// \brief Interface for primitive Cone feature.
class PrimitivesAPI_Torus: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  PRIMITIVESAPI_EXPORT
  explicit PrimitivesAPI_Torus(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  PRIMITIVESAPI_EXPORT
  explicit PrimitivesAPI_Torus(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                               const ModelHighAPI_Selection& theBasePoint,
                               const ModelHighAPI_Selection& theAxis,
                               const ModelHighAPI_Double& theRadius,
                               const ModelHighAPI_Double& theRingRadius);

  /// Destructor.
  PRIMITIVESAPI_EXPORT
  virtual ~PrimitivesAPI_Torus();

  INTERFACE_4(PrimitivesPlugin_Torus::ID(),
             basePoint, PrimitivesPlugin_Torus::BASE_POINT_ID(),
             ModelAPI_AttributeSelection, /** Base point */,
             axis, PrimitivesPlugin_Torus::AXIS_ID(),
             ModelAPI_AttributeSelection, /** Axis */,
             radius, PrimitivesPlugin_Torus::RADIUS_ID(),
             ModelAPI_AttributeDouble, /** Radius */,
             ringRadius, PrimitivesPlugin_Torus::RING_RADIUS_ID(),
             ModelAPI_AttributeDouble, /** Ring radius */)

  /// Set radius
  PRIMITIVESAPI_EXPORT
  void setRadius(const ModelHighAPI_Double& theRadius,
                 const ModelHighAPI_Double& theRingRadius);

  /// Dump wrapped feature
  PRIMITIVESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on primitive Cone object
typedef std::shared_ptr<PrimitivesAPI_Torus> TorusPtr;

/// \ingroup CPPHighAPI
/// \brief Create primitive Cone feature.
PRIMITIVESAPI_EXPORT
TorusPtr addTorus(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const ModelHighAPI_Selection& theBasePoint,
                  const ModelHighAPI_Selection& theAxis,
                  const ModelHighAPI_Double& theRadius,
                  const ModelHighAPI_Double& theRingRadius);

/// \ingroup CPPHighAPI
/// \brief Create primitive Cone feature.
PRIMITIVESAPI_EXPORT
TorusPtr addTorus(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const ModelHighAPI_Double& theRadius,
                  const ModelHighAPI_Double& theRingRadius);

#endif // PRIMITIVESAPI_TORUS_H_
