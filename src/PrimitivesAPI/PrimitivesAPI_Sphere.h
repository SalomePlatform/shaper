// Copyright (C) 2014-201x CEA/DEN, EDF R&D -->

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

  /// Destructor.
  PRIMITIVESAPI_EXPORT
  virtual ~PrimitivesAPI_Sphere();

  INTERFACE_2(PrimitivesPlugin_Sphere::ID(),
             centerPoint, PrimitivesPlugin_Sphere::CENTER_POINT_ID(),
             ModelAPI_AttributeSelection, /** Center point */,
             radius, PrimitivesPlugin_Sphere::RADIUS_ID(),
             ModelAPI_AttributeDouble, /** Radius */)

  /// Set center point
  PRIMITIVESAPI_EXPORT
  void setCenterPoint(const ModelHighAPI_Selection& theCenterPoint);

  /// Set radius
  PRIMITIVESAPI_EXPORT
  void setRadius(const ModelHighAPI_Double& theRadius);

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

#endif // PRIMITIVESAPI_SPHERE_H_
