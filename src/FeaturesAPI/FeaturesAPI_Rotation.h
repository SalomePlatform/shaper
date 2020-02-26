// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef FeaturesAPI_Rotation_H_
#define FeaturesAPI_Rotation_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Rotation.h>

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>
#include <ModelHighAPI_Selection.h>

class ModelHighAPI_Dumper;

/// \class FeaturesAPI_Rotation
/// \ingroup CPPHighAPI
/// \brief Interface for Rotation feature.
class FeaturesAPI_Rotation: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Rotation(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  FeaturesAPI_Rotation(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                       const std::list<ModelHighAPI_Selection>& theMainObjects,
                       const ModelHighAPI_Selection& theAxisObject,
                       const ModelHighAPI_Double& theAngle);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  FeaturesAPI_Rotation(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                       const std::list<ModelHighAPI_Selection>& theMainObjects,
                       const ModelHighAPI_Selection& theCenterPoint,
                       const ModelHighAPI_Selection& theStartPoint,
                       const ModelHighAPI_Selection& theEndPoint);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Rotation();

  INTERFACE_7(FeaturesPlugin_Rotation::ID(),
              creationMethod, FeaturesPlugin_Rotation::CREATION_METHOD(),
              ModelAPI_AttributeString, /** Creation method */,
              mainObjects, FeaturesPlugin_Rotation::OBJECTS_LIST_ID(),
              ModelAPI_AttributeSelectionList, /** Main objects */,
              axisObject, FeaturesPlugin_Rotation::AXIS_OBJECT_ID(),
              ModelAPI_AttributeSelection, /** Axis object */,
              angle, FeaturesPlugin_Rotation::ANGLE_ID(),
              ModelAPI_AttributeDouble, /** Angle */,
              centerPoint, FeaturesPlugin_Rotation::CENTER_POINT_ID(),
              ModelAPI_AttributeSelection, /** Center point */,
              startPoint, FeaturesPlugin_Rotation::START_POINT_ID(),
              ModelAPI_AttributeSelection, /** Start point */,
              endPoint, FeaturesPlugin_Rotation::END_POINT_ID(),
              ModelAPI_AttributeSelection, /** End point */
             )

  /// Set main objects.
  FEATURESAPI_EXPORT
  void setMainObjects(const std::list<ModelHighAPI_Selection>& theMainObjects);

  /// Set axis object.
  FEATURESAPI_EXPORT
  void setAxisObject(const ModelHighAPI_Selection& theAxisObject);

  /// Set angle.
  FEATURESAPI_EXPORT
  void setAngle(const ModelHighAPI_Double& theAngle);

  /// Set center point, start point and end point
  FEATURESAPI_EXPORT
  void setPoints(const ModelHighAPI_Selection& theCenterPoint,
                 const ModelHighAPI_Selection& theStartPoint,
                 const ModelHighAPI_Selection& theEndPoint);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Rotation object.
typedef std::shared_ptr<FeaturesAPI_Rotation> RotationPtr;


#define DUMMY std::pair<ModelHighAPI_Selection, ModelHighAPI_Double>()

/// \ingroup CPPHighAPI
/// \brief Create Rotation feature.
FEATURESAPI_EXPORT RotationPtr addRotation(
    const std::shared_ptr<ModelAPI_Document>& part,
    const std::list<ModelHighAPI_Selection>& objects,
    const ModelHighAPI_Selection& axis = ModelHighAPI_Selection(),
    const std::pair<ModelHighAPI_Selection, ModelHighAPI_Double>& angle = DUMMY,
    const ModelHighAPI_Selection& center = ModelHighAPI_Selection(),
    const ModelHighAPI_Selection& start = ModelHighAPI_Selection(),
    const ModelHighAPI_Selection& end = ModelHighAPI_Selection(),
    const bool keepSubResults = false);

#endif // FeaturesAPI_Rotation_H_
