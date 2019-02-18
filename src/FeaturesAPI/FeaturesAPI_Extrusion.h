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

#ifndef FeaturesAPI_Extrusion_H_
#define FeaturesAPI_Extrusion_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Extrusion.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Double;
class ModelHighAPI_Reference;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_Extrusion
/// \ingroup CPPHighAPI
/// \brief Interface for Extrusion feature.
class FeaturesAPI_Extrusion: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Extrusion(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Extrusion(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                 const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                 const ModelHighAPI_Double& theSize);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Extrusion(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                 const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                 const ModelHighAPI_Selection& theDirection,
                                 const ModelHighAPI_Double& theSize);
  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Extrusion(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                 const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                 const ModelHighAPI_Double& theToSize,
                                 const ModelHighAPI_Double& theFromSize);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Extrusion(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                 const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                 const ModelHighAPI_Selection& theDirection,
                                 const ModelHighAPI_Double& theToSize,
                                 const ModelHighAPI_Double& theFromSize);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Extrusion(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                 const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                 const ModelHighAPI_Selection& theToObject,
                                 const ModelHighAPI_Double& theToOffset,
                                 const ModelHighAPI_Selection& theFromObject,
                                 const ModelHighAPI_Double& theFromOffset);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Extrusion(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                 const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                 const ModelHighAPI_Selection& theDirection,
                                 const ModelHighAPI_Selection& theToObject,
                                 const ModelHighAPI_Double& theToOffset,
                                 const ModelHighAPI_Selection& theFromObject,
                                 const ModelHighAPI_Double& theFromOffset);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Extrusion();

  INTERFACE_10(FeaturesPlugin_Extrusion::ID(),
               sketch, FeaturesPlugin_Extrusion::SKETCH_ID(),
               ModelAPI_AttributeReference, /** Sketch launcher */,
               baseObjects, FeaturesPlugin_Extrusion::BASE_OBJECTS_ID(),
               ModelAPI_AttributeSelectionList, /** Base objects */,
               direction, FeaturesPlugin_Extrusion::DIRECTION_OBJECT_ID(),
               ModelAPI_AttributeSelection, /** Direction */,
               creationMethod, FeaturesPlugin_Extrusion::CREATION_METHOD(),
               ModelAPI_AttributeString, /** Creation method */,
               toSize, FeaturesPlugin_Extrusion::TO_SIZE_ID(),
               ModelAPI_AttributeDouble, /** To size */,
               fromSize, FeaturesPlugin_Extrusion::FROM_SIZE_ID(),
               ModelAPI_AttributeDouble, /** From size */,
               toObject, FeaturesPlugin_Extrusion::TO_OBJECT_ID(),
               ModelAPI_AttributeSelection, /** To object */,
               toOffset, FeaturesPlugin_Extrusion::TO_OFFSET_ID(),
               ModelAPI_AttributeDouble, /** To offset */,
               fromObject, FeaturesPlugin_Extrusion::FROM_OBJECT_ID(),
               ModelAPI_AttributeSelection, /** From object */,
               fromOffset, FeaturesPlugin_Extrusion::FROM_OFFSET_ID(),
               ModelAPI_AttributeDouble, /** From offset */)

  /// Modify base attribute of the feature.
  FEATURESAPI_EXPORT
  void setNestedSketch(const ModelHighAPI_Reference& theSketch);

  /// Modify base attribute of the feature.
  FEATURESAPI_EXPORT
  void setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects);

  /// Modify direction_object attribute of the feature.
  FEATURESAPI_EXPORT
  void setDirection(const ModelHighAPI_Selection& theDirection);

  /// Modify CreationMethod, to_size, from_size attributes of the feature.
  FEATURESAPI_EXPORT
  void setSizes(const ModelHighAPI_Double& theToSize,
                const ModelHighAPI_Double& theFromSize);

  /// Modify creation_method, to_size, from_size attributes of the feature.
  FEATURESAPI_EXPORT
  void setSize(const ModelHighAPI_Double& theSize);

  /// Modify creation_method, to_object, to_offset,
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

/// Pointer on Extrusion object.
typedef std::shared_ptr<FeaturesAPI_Extrusion> ExtrusionPtr;

/// \ingroup CPPHighAPI
/// \brief Create Extrusion feature.
FEATURESAPI_EXPORT
ExtrusionPtr addExtrusion(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const std::list<ModelHighAPI_Selection>& theBaseObjects,
                          const ModelHighAPI_Double& theSize);

/// \ingroup CPPHighAPI
/// \brief Create Extrusion feature.
FEATURESAPI_EXPORT
ExtrusionPtr addExtrusion(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const std::list<ModelHighAPI_Selection>& theBaseObjects,
                          const ModelHighAPI_Selection& theDirection,
                          const ModelHighAPI_Double& theSize);

/// \ingroup CPPHighAPI
/// \brief Create Extrusion feature.
FEATURESAPI_EXPORT
ExtrusionPtr addExtrusion(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const std::list<ModelHighAPI_Selection>& theBaseObjects,
                          const ModelHighAPI_Double& theToSize,
                          const ModelHighAPI_Double& theFromSize);

/// \ingroup CPPHighAPI
/// \brief Create Extrusion feature.
FEATURESAPI_EXPORT
ExtrusionPtr addExtrusion(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const std::list<ModelHighAPI_Selection>& theBaseObjects,
                          const ModelHighAPI_Selection& theDirection,
                          const ModelHighAPI_Double& theToSize,
                          const ModelHighAPI_Double& theFromSize);

/// \ingroup CPPHighAPI
/// \brief Create Extrusion feature.
FEATURESAPI_EXPORT
ExtrusionPtr addExtrusion(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const std::list<ModelHighAPI_Selection>& theBaseObjects,
                          const ModelHighAPI_Selection& theToObject,
                          const ModelHighAPI_Double& theToOffset,
                          const ModelHighAPI_Selection& theFromObject,
                          const ModelHighAPI_Double& theFromOffset);

/// \ingroup CPPHighAPI
/// \brief Create Extrusion feature.
FEATURESAPI_EXPORT
ExtrusionPtr addExtrusion(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const std::list<ModelHighAPI_Selection>& theBaseObjects,
                          const ModelHighAPI_Selection& theDirection,
                          const ModelHighAPI_Selection& theToObject,
                          const ModelHighAPI_Double& theToOffset,
                          const ModelHighAPI_Selection& theFromObject,
                          const ModelHighAPI_Double& theFromOffset);

#endif // FeaturesAPI_Extrusion_H_
