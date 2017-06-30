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

#ifndef FeaturesAPI_Revolution_H_
#define FeaturesAPI_Revolution_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Revolution.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Double;
class ModelHighAPI_Reference;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_Revolution
/// \ingroup CPPHighAPI
/// \brief Interface for Revolution feature.
class FeaturesAPI_Revolution: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Revolution(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Revolution(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Selection& theAxis,
                                  const ModelHighAPI_Double& theAngle);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Revolution(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Selection& theAxis,
                                  const ModelHighAPI_Double& theToAngle,
                                  const ModelHighAPI_Double& theFromAngle);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Revolution(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                  const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                  const ModelHighAPI_Selection& theAxis,
                                  const ModelHighAPI_Selection& theToObject,
                                  const ModelHighAPI_Double& theToOffset,
                                  const ModelHighAPI_Selection& theFromObject,
                                  const ModelHighAPI_Double& theFromOffset);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Revolution();

  INTERFACE_10(FeaturesPlugin_Revolution::ID(),
               sketch, FeaturesPlugin_Revolution::SKETCH_ID(),
               ModelAPI_AttributeReference, /** Sketch launcher */,
               baseObjects, FeaturesPlugin_Revolution::BASE_OBJECTS_ID(),
               ModelAPI_AttributeSelectionList, /** Base objects */,
               axis, FeaturesPlugin_Revolution::AXIS_OBJECT_ID(),
               ModelAPI_AttributeSelection, /** Axis */,
               creationMethod, FeaturesPlugin_Revolution::CREATION_METHOD(),
               ModelAPI_AttributeString, /** Creation method */,
               toAngle, FeaturesPlugin_Revolution::TO_ANGLE_ID(),
               ModelAPI_AttributeDouble, /** To angle */,
               fromAngle, FeaturesPlugin_Revolution::FROM_ANGLE_ID(),
               ModelAPI_AttributeDouble, /** From angle */,
               toObject, FeaturesPlugin_Revolution::TO_OBJECT_ID(),
               ModelAPI_AttributeSelection, /** To object */,
               toOffset, FeaturesPlugin_Revolution::TO_OFFSET_ID(),
               ModelAPI_AttributeDouble, /** To offset */,
               fromObject, FeaturesPlugin_Revolution::FROM_OBJECT_ID(),
               ModelAPI_AttributeSelection, /** From object */,
               fromOffset, FeaturesPlugin_Revolution::FROM_OFFSET_ID(),
               ModelAPI_AttributeDouble, /** From offset */)

  /// Modify base attribute of the feature.
  FEATURESAPI_EXPORT
  void setNestedSketch(const ModelHighAPI_Reference& theSketch);

  /// Modify base attribute of the feature.
  FEATURESAPI_EXPORT
  void setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Modify axis_object attribute of the feature.
  FEATURESAPI_EXPORT
  void setAxis(const ModelHighAPI_Selection& theAxis);

  /// Modify CreationMethod, to_angle, from_angle attributes of the feature.
  FEATURESAPI_EXPORT
  void setAngles(const ModelHighAPI_Double& theToAngle,
                 const ModelHighAPI_Double& theFromAngle);

  /// Modify CreationMethod, to_angle, from_angle attributes of the feature.
  FEATURESAPI_EXPORT
  void setAngle(const ModelHighAPI_Double& theAngle);

  /// Modify CreationMethod, to_object, to_offset,
  /// from_object, from_offset attributes of the feature.
  FEATURESAPI_EXPORT
  void setPlanesAndOffsets(const ModelHighAPI_Selection& theToObject,
                           const ModelHighAPI_Double& theToOffset,
                           const ModelHighAPI_Selection& theFromObject,
                           const ModelHighAPI_Double& theFromOffset);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;

private:
  void execIfBaseNotEmpty();
};

/// Pointer on Revolution object.
typedef std::shared_ptr<FeaturesAPI_Revolution> RevolutionPtr;

/// \ingroup CPPHighAPI
/// \brief Create Revolution feature.
FEATURESAPI_EXPORT
RevolutionPtr addRevolution(const std::shared_ptr<ModelAPI_Document>& thePart,
                            const std::list<ModelHighAPI_Selection>& theBaseObjects,
                            const ModelHighAPI_Selection& theAxis,
                            const ModelHighAPI_Double& theAngle);

/// \ingroup CPPHighAPI
/// \brief Create Revolution feature.
FEATURESAPI_EXPORT
RevolutionPtr addRevolution(const std::shared_ptr<ModelAPI_Document>& thePart,
                            const std::list<ModelHighAPI_Selection>& theBaseObjects,
                            const ModelHighAPI_Selection& theAxis,
                            const ModelHighAPI_Double& theToAngle,
                            const ModelHighAPI_Double& theFromAngle);

/// \ingroup CPPHighAPI
/// \brief Create Revolution feature.
FEATURESAPI_EXPORT
RevolutionPtr addRevolution(const std::shared_ptr<ModelAPI_Document>& thePart,
                            const std::list<ModelHighAPI_Selection>& theBaseObjects,
                            const ModelHighAPI_Selection& theAxis,
                            const ModelHighAPI_Selection& theToObject,
                            const ModelHighAPI_Double& theToOffset,
                            const ModelHighAPI_Selection& theFromObject,
                            const ModelHighAPI_Double& theFromOffset);

#endif // FeaturesAPI_Revolution_H_
