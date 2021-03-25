// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#ifndef FEATURESAPI_SYMMETRY_H_
#define FEATURESAPI_SYMMETRY_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Symmetry.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Double;
class ModelHighAPI_Dumper;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_Symmetry
/// \ingroup CPPHighAPI
/// \brief Interface for Symmetry feature.
class FeaturesAPI_Symmetry: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Symmetry(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Symmetry(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                const std::list<ModelHighAPI_Selection>& theMainObjects,
                                const ModelHighAPI_Selection& theObject,
                                bool theKeepOriginal = true);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Symmetry();

  INTERFACE_6(FeaturesPlugin_Symmetry::ID(),
              creationMethod, FeaturesPlugin_Symmetry::CREATION_METHOD(),
              ModelAPI_AttributeString, /** Creation method */,
              mainObjects, FeaturesPlugin_Symmetry::OBJECTS_LIST_ID(),
              ModelAPI_AttributeSelectionList, /** Main objects */,
              pointObject, FeaturesPlugin_Symmetry::POINT_OBJECT_ID(),
              ModelAPI_AttributeSelection, /** Point object */,
              axisObject, FeaturesPlugin_Symmetry::AXIS_OBJECT_ID(),
              ModelAPI_AttributeSelection, /** Axis object */,
              planeObject, FeaturesPlugin_Symmetry::PLANE_OBJECT_ID(),
              ModelAPI_AttributeSelection, /** Plane object */,
              keepOriginal, FeaturesPlugin_Symmetry::KEEP_ORIGINAL_RESULT(),
              ModelAPI_AttributeBoolean, /** Keep original shape */)

  /// Set main objects.
  FEATURESAPI_EXPORT
  void setMainObjects(const std::list<ModelHighAPI_Selection>& theMainObjects);

  /// Modify CreationMethod, point_object attribute of the feature.
  FEATURESAPI_EXPORT
  void setPoint(const ModelHighAPI_Selection& thePointObject);

  /// Modify CreationMethod, axis_object attribute of the feature.
  FEATURESAPI_EXPORT
  void setAxis(const ModelHighAPI_Selection& theAxisObject);

  /// Modify CreationMethod, plane_object attribute of the feature.
  FEATURESAPI_EXPORT
  void setPlane(const ModelHighAPI_Selection& thePlaneObject);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Symmetry object.
typedef std::shared_ptr<FeaturesAPI_Symmetry> SymmetryPtr;

/// \ingroup CPPHighAPI
/// \brief Create Symmetry feature.
FEATURESAPI_EXPORT
SymmetryPtr addSymmetry(const std::shared_ptr<ModelAPI_Document>& part,
                        const std::list<ModelHighAPI_Selection>& objects,
                        const ModelHighAPI_Selection& basis,
                        const bool keepOriginal = true,
                        const bool keepSubResults = false);

#endif // FEATURESAPI_SYMMETRY_H_
