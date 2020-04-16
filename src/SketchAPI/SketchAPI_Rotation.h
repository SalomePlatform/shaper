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

#ifndef SRC_SKETCHAPI_SKETCHAPI_ROTATION_H_
#define SRC_SKETCHAPI_SKETCHAPI_ROTATION_H_

//--------------------------------------------------------------------------------------
#include "SketchAPI.h"
#include "SketchAPI_SketchEntity.h"

#include <list>

#include <SketchPlugin_MultiRotation.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>
//--------------------------------------------------------------------------------------
class ModelAPI_Object;
class ModelHighAPI_Double;
class ModelHighAPI_Integer;
class ModelHighAPI_RefAttr;
//--------------------------------------------------------------------------------------
/**\class SketchAPI_Rotation
 * \ingroup CPPHighAPI
 * \brief Interface for Rotation feature
 */
class SketchAPI_Rotation : public ModelHighAPI_Interface
{
public:
  /// Constructor without values
  SKETCHAPI_EXPORT
  explicit SketchAPI_Rotation(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_Rotation(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                     const std::list<std::shared_ptr<ModelAPI_Object> > & theObjects,
                     const ModelHighAPI_RefAttr & theCenter,
                     const ModelHighAPI_Double & theAngle,
                     const ModelHighAPI_Integer & theNumberOfObjects,
                     bool theFullValue = false,
                     bool theReversed  = false);
  /// Destructor
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_Rotation();

  INTERFACE_8(SketchPlugin_MultiRotation::ID(),
              rotationList, SketchPlugin_MultiRotation::ROTATION_LIST_ID(),
              ModelAPI_AttributeRefList, /** Rotation list */,
              center, SketchPlugin_MultiRotation::CENTER_ID(),
              ModelAPI_AttributeRefAttr, /** Center */,
              angle, SketchPlugin_MultiRotation::ANGLE_ID(),
              ModelAPI_AttributeDouble, /** Angle */,
              reversed, SketchPlugin_MultiRotation::REVERSED_ID(),
              ModelAPI_AttributeBoolean, /** Negative angle */,
              numberOfObjects, SketchPlugin_MultiRotation::NUMBER_OF_OBJECTS_ID(),
              ModelAPI_AttributeInteger, /** Number of objects */,
              valueType, SketchPlugin_MultiRotation::ANGLE_TYPE(),
              ModelAPI_AttributeString, /** Value type */,
              referenceObjects, SketchPlugin_MultiRotation::ENTITY_A(),
              ModelAPI_AttributeRefList, /** Reference objects */,
              rotatedObjects, SketchPlugin_MultiRotation::ENTITY_B(),
              ModelAPI_AttributeRefList, /** Rotated objects */
  )

  /// List of rotated objects
  SKETCHAPI_EXPORT
  std::list<std::shared_ptr<SketchAPI_SketchEntity> > rotated() const;

  /// Dump wrapped feature
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

//! Pointer on Rotation object
typedef std::shared_ptr<SketchAPI_Rotation> RotationPtr;

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_SKETCHAPI_SKETCHAPI_ROTATION_H_ */
