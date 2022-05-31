// Copyright (C) 2017-2022  CEA/DEN, EDF R&D
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

// File:        PrimitivesAPI_Sphere.h
// Created:     16 Mar 2017
// Author:      Clarisse Genrault

#ifndef PRIMITIVESAPI_SPHERE_H_
#define PRIMITIVESAPI_SPHERE_H_

#include "PrimitivesAPI.h"

#include <PrimitivesPlugin_Sphere.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Double;
class ModelHighAPI_Selection;

/// \class PrimitivesAPI_Sphere
/// \ingroup CPPHighAPI
/// \brief Interface for primitive Sphere feature.
class PrimitivesAPI_Sphere: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  PRIMITIVESAPI_EXPORT
  explicit PrimitivesAPI_Sphere(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  PRIMITIVESAPI_EXPORT
  explicit PrimitivesAPI_Sphere(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                const ModelHighAPI_Selection& theCenterPoint,
                                const ModelHighAPI_Double& theRadius);

  /// Constructor with values.
  PRIMITIVESAPI_EXPORT
  explicit PrimitivesAPI_Sphere(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                const ModelHighAPI_Double& theRMin,
                                const ModelHighAPI_Double& theRMax,
                                const ModelHighAPI_Double& thePhiMin,
                                const ModelHighAPI_Double& thePhiMax,
                                const ModelHighAPI_Double& theThetaMin,
                                const ModelHighAPI_Double& theThetaMax);

  /// Destructor.
  PRIMITIVESAPI_EXPORT
  virtual ~PrimitivesAPI_Sphere();

  INTERFACE_9(PrimitivesPlugin_Sphere::ID(),
             creationMethod, PrimitivesPlugin_Sphere::CREATION_METHOD(),
             ModelAPI_AttributeString, /** Creation method */,
             centerPoint, PrimitivesPlugin_Sphere::CENTER_POINT_ID(),
             ModelAPI_AttributeSelection, /** Center point */,
             radius, PrimitivesPlugin_Sphere::RADIUS_ID(),
             ModelAPI_AttributeDouble, /** Radius */,
             rmin, PrimitivesPlugin_Sphere::RMIN_ID(),
             ModelAPI_AttributeDouble, /** The minimum radius*/,
             rmax, PrimitivesPlugin_Sphere::RMAX_ID(),
             ModelAPI_AttributeDouble, /** The maximum radius*/,
             phimin, PrimitivesPlugin_Sphere::PHIMIN_ID(),
             ModelAPI_AttributeDouble, /** The minimum phi*/,
             phimax, PrimitivesPlugin_Sphere::PHIMAX_ID(),
             ModelAPI_AttributeDouble, /** The maximum phi*/,
             thetamin, PrimitivesPlugin_Sphere::THETAMIN_ID(),
             ModelAPI_AttributeDouble, /** The minimum theta*/,
             thetamax,PrimitivesPlugin_Sphere::THETAMAX_ID(),
             ModelAPI_AttributeDouble, /** The maximum theta*/)

  /// Set center point
  PRIMITIVESAPI_EXPORT
  void setCenterPoint(const ModelHighAPI_Selection& theCenterPoint);

  /// Set radius
  PRIMITIVESAPI_EXPORT
  void setRadius(const ModelHighAPI_Double& theRadius);

  /// Set minimum and maximum radius
  PRIMITIVESAPI_EXPORT
  void setRadius(const ModelHighAPI_Double& theRMin, const ModelHighAPI_Double& theRMax);

  /// Set minimum and maximum phi
  PRIMITIVESAPI_EXPORT
  void setPhi(const ModelHighAPI_Double& thePhiMin, const ModelHighAPI_Double& thePhiMax);

  /// Set minimum and maximum theta
  PRIMITIVESAPI_EXPORT
  void setTheta(const ModelHighAPI_Double& theThetaMin, const ModelHighAPI_Double& theThetaMax);

  /// Dump wrapped feature
  PRIMITIVESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on primitive Sphere object
typedef std::shared_ptr<PrimitivesAPI_Sphere> SpherePtr;

/// \ingroup CPPHighAPI
/// \brief Create primitive Sphere feature.
PRIMITIVESAPI_EXPORT
SpherePtr addSphere(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const ModelHighAPI_Selection& theCenterPoint,
                    const ModelHighAPI_Double& theRadius);

/// \ingroup CPPHighAPI
/// \brief Create primitive Sphere feature.
PRIMITIVESAPI_EXPORT
SpherePtr addSphere(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const ModelHighAPI_Double& theRadius);

/// \ingroup CPPHighAPI
/// \brief Create primitive Sphere feature.
PRIMITIVESAPI_EXPORT
SpherePtr addSphere(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const ModelHighAPI_Double& theRMin,
                    const ModelHighAPI_Double& theRMax,
                    const ModelHighAPI_Double& thePhiMin,
                    const ModelHighAPI_Double& thePhiMax,
                    const ModelHighAPI_Double& theThetaMin,
                    const ModelHighAPI_Double& theThetaMax);

#endif // PRIMITIVESAPI_SPHERE_H_
