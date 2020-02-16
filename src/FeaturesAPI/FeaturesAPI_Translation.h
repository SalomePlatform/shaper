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

#ifndef FeaturesAPI_Translation_H_
#define FeaturesAPI_Translation_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Translation.h>

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>
#include <ModelHighAPI_Selection.h>

class ModelHighAPI_Dumper;

/// \class FeaturesAPI_Translation
/// \ingroup CPPHighAPI
/// \brief Interface for Translation feature.
class FeaturesAPI_Translation: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Translation(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Translation(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                   const std::list<ModelHighAPI_Selection>& theMainObjects,
                                   const ModelHighAPI_Selection& theAxisObject,
                                   const ModelHighAPI_Double& theDistance);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Translation(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                   const std::list<ModelHighAPI_Selection>& theMainObjects,
                                   const ModelHighAPI_Double& theDx,
                                   const ModelHighAPI_Double& theDy,
                                   const ModelHighAPI_Double& theDz);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Translation(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                   const std::list<ModelHighAPI_Selection>& theMainObjects,
                                   const ModelHighAPI_Selection& theStartPoint,
                                   const ModelHighAPI_Selection& theEndPoint);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Translation();

  INTERFACE_9(FeaturesPlugin_Translation::ID(),
              creationMethod, FeaturesPlugin_Translation::CREATION_METHOD(),
              ModelAPI_AttributeString, /** Creation method */,
              mainObjects, FeaturesPlugin_Translation::OBJECTS_LIST_ID(),
              ModelAPI_AttributeSelectionList, /** Main objects */,
              axisObject, FeaturesPlugin_Translation::AXIS_OBJECT_ID(),
              ModelAPI_AttributeSelection, /** Axis object */,
              distance, FeaturesPlugin_Translation::DISTANCE_ID(),
              ModelAPI_AttributeDouble, /** Distance */,
              dx, FeaturesPlugin_Translation::DX_ID(),
              ModelAPI_AttributeDouble, /** Dimension in X */,
              dy, FeaturesPlugin_Translation::DY_ID(),
              ModelAPI_AttributeDouble, /** Dimension in Y */,
              dz, FeaturesPlugin_Translation::DZ_ID(),
              ModelAPI_AttributeDouble, /** Dimension in Z */,
              startPoint, FeaturesPlugin_Translation::START_POINT_ID(),
              ModelAPI_AttributeSelection, /** Start point object */,
              endPoint, FeaturesPlugin_Translation::END_POINT_ID(),
              ModelAPI_AttributeSelection, /** End point object */
             )

  /// Set main objects.
  FEATURESAPI_EXPORT
  void setMainObjects(const std::list<ModelHighAPI_Selection>& theMainObjects);

  /// Modify CreationMethod, axis_object, distance attributes of the feature.
  FEATURESAPI_EXPORT
  void setAxisAndDistance(const ModelHighAPI_Selection& theAxisObject,
                          const ModelHighAPI_Double& theDistance);

  /// Modify CreationMethod, dx, dy, dz attributes of the feature.
  FEATURESAPI_EXPORT
  void setDimensions(const ModelHighAPI_Double& theDx,
                     const ModelHighAPI_Double& theDy,
                     const ModelHighAPI_Double& theDz);

  /// Modify CreationMethod, start_point, end_point attributes of the feature.
  FEATURESAPI_EXPORT
  void setPoints(const ModelHighAPI_Selection& theStartPoint,
                 const ModelHighAPI_Selection& theEndPoint);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Translation object.
typedef std::shared_ptr<FeaturesAPI_Translation> TranslationPtr;

#define DUMMY std::pair<ModelHighAPI_Selection, ModelHighAPI_Double>()

/// \ingroup CPPHighAPI
/// \brief Create Translation feature.
FEATURESAPI_EXPORT TranslationPtr addTranslation(
    const std::shared_ptr<ModelAPI_Document>& part,
    const std::list<ModelHighAPI_Selection>& objects,
    const std::pair<ModelHighAPI_Selection, ModelHighAPI_Double>& deprecated1 = DUMMY,
    const std::pair<ModelHighAPI_Selection, ModelHighAPI_Double>& deprecated2 = DUMMY,
    const std::pair<ModelHighAPI_Selection, ModelHighAPI_Double>& deprecated3 = DUMMY,
    const ModelHighAPI_Selection& axis = ModelHighAPI_Selection(),
    const ModelHighAPI_Double& distance = ModelHighAPI_Double(0.0),
    const std::list<ModelHighAPI_Double>& vector = std::list<ModelHighAPI_Double>(),
    const ModelHighAPI_Selection& startPoint = ModelHighAPI_Selection(),
    const ModelHighAPI_Selection& endPoint = ModelHighAPI_Selection(),
    const bool keepSubResults = false );

#endif // FeaturesAPI_Translation_H_
