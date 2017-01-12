// Copyright (C) 2014-2016 CEA/DEN, EDF R&D -->

// File:        PrimitivesAPI_Box.h
// Created:     28 June 2016
// Author:      Clarisse Genrault (CEA)

#ifndef PRIMITIVESAPI_BOX_H_
#define PRIMITIVESAPI_BOX_H_

#include "PrimitivesAPI.h"

#include <PrimitivesPlugin_Box.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Double;
class ModelHighAPI_Selection;

/// \class PrimitivesAPI_Box
/// \ingroup CPPHighAPI
/// \brief Interface for primitive Box feature.
class PrimitivesAPI_Box: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  PRIMITIVESAPI_EXPORT
  explicit PrimitivesAPI_Box(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  PRIMITIVESAPI_EXPORT
  explicit PrimitivesAPI_Box(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const ModelHighAPI_Double& theDx,
                             const ModelHighAPI_Double& theDy,
                             const ModelHighAPI_Double& theDz);

  /// Constructor with values.
  PRIMITIVESAPI_EXPORT
  explicit PrimitivesAPI_Box(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const ModelHighAPI_Selection& theFirstPoint,
                             const ModelHighAPI_Selection& theSecondPoint);

  /// Destructor.
  PRIMITIVESAPI_EXPORT
  virtual ~PrimitivesAPI_Box();

  INTERFACE_6(PrimitivesPlugin_Box::ID(),
             creationMethod, PrimitivesPlugin_Box::CREATION_METHOD(),
             ModelAPI_AttributeString, /** Creation method */,
             dx, PrimitivesPlugin_Box::DX_ID(),
             ModelAPI_AttributeDouble, /** Dimension in X */,
             dy, PrimitivesPlugin_Box::DY_ID(),
             ModelAPI_AttributeDouble, /** Dimension in Y */,
             dz, PrimitivesPlugin_Box::DZ_ID(),
             ModelAPI_AttributeDouble, /** Dimension in Z */,
             firstPoint, PrimitivesPlugin_Box::POINT_FIRST_ID(),
             ModelAPI_AttributeSelection, /** First point */,
             secondPoint, PrimitivesPlugin_Box::POINT_SECOND_ID(),
             ModelAPI_AttributeSelection, /** Second point */)

  /// Set dimensions
  PRIMITIVESAPI_EXPORT
  void setDimensions(const ModelHighAPI_Double& theDx,
                     const ModelHighAPI_Double& theDy,
                     const ModelHighAPI_Double& theDz);

  /// Set points
  PRIMITIVESAPI_EXPORT
  void setPoints(const ModelHighAPI_Selection& theFirstPoint,
                 const ModelHighAPI_Selection& theSecondPoint);

  /// Dump wrapped feature
  PRIMITIVESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on primitive Box object
typedef std::shared_ptr<PrimitivesAPI_Box> BoxPtr;

/// \ingroup CPPHighAPI
/// \brief Create primitive Box feature.
PRIMITIVESAPI_EXPORT
BoxPtr addBox(const std::shared_ptr<ModelAPI_Document>& thePart,
              const ModelHighAPI_Double& theDx,
              const ModelHighAPI_Double& theDy,
              const ModelHighAPI_Double& theDz);

/// \ingroup CPPHighAPI
/// \brief Create primitive Box feature.
PRIMITIVESAPI_EXPORT
BoxPtr addBox(const std::shared_ptr<ModelAPI_Document>& thePart,
              const ModelHighAPI_Selection& theFirstPoint,
              const ModelHighAPI_Selection& theSecondPoint);

#endif // PRIMITIVESAPI_BOX_H_