// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef FeatureAPI_GlueFaces_H_
#define FeatureAPI_GlueFaces_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_GlueFaces.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Double;
class ModelHighAPI_Dumper;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_GlueFaces
/// \ingroup CPPHighAPI
/// \brief Interface for Glue Faces feature.
class FeaturesAPI_GlueFaces: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_GlueFaces(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_GlueFaces(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                 const std::list<ModelHighAPI_Selection>& theMainObjects,
                                 const ModelHighAPI_Double& theTolerance,
                                 bool theIsKeepNonSolids = true);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_GlueFaces();

  INTERFACE_3(FeaturesPlugin_GlueFaces::ID(),
              mainObjects, FeaturesPlugin_GlueFaces::OBJECTS_LIST_ID(),
              ModelAPI_AttributeSelectionList, /** Main objects */,
              tolerance, FeaturesPlugin_GlueFaces::TOLERANCE_ID(),
              ModelAPI_AttributeDouble, /** Tolerance */,
              keepNonSolids, FeaturesPlugin_GlueFaces::KEEP_NON_SOLIDS_ID(),
              ModelAPI_AttributeBoolean, /** Keep all non-solid shapes */)

  /// Set main objects.
  FEATURESAPI_EXPORT
  void setMainObjects(const std::list<ModelHighAPI_Selection>& theMainObjects);

  /// Set the tolerance.
  FEATURESAPI_EXPORT
  void setTolerance(const ModelHighAPI_Double& theTolerance);

  /// Set flag whether to keep all non-solid shapes.
  FEATURESAPI_EXPORT
  void setKeepNonSolids(bool theFlag);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Glue Faces object.
typedef std::shared_ptr<FeaturesAPI_GlueFaces> GlueFacesPtr;

/// \ingroup CPPHighAPI
/// \brief Create Glue Faces feature.
FEATURESAPI_EXPORT
GlueFacesPtr addGlueFaces(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const std::list<ModelHighAPI_Selection>& theMainObjects,
                          const ModelHighAPI_Double& theTolerance,
                          const bool theKeepNonSolids);

#endif // FeatureAPI_GlueFaces_H_
