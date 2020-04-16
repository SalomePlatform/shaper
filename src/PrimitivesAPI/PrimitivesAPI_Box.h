// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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