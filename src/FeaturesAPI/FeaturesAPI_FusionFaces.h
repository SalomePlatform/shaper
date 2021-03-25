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

#ifndef FeaturesAPI_FusionFaces_H_
#define FeaturesAPI_FusionFaces_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_FusionFaces.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Dumper;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_FusionFaces
/// \ingroup CPPHighAPI
/// \brief Interface for FusionFaces feature.
class FeaturesAPI_FusionFaces: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
    explicit FeaturesAPI_FusionFaces(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
    explicit FeaturesAPI_FusionFaces(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                       const ModelHighAPI_Selection& theBase);

  /// Destructor.
  FEATURESAPI_EXPORT
    virtual ~FeaturesAPI_FusionFaces();

  INTERFACE_1(FeaturesPlugin_FusionFaces::ID(),
              base, FeaturesPlugin_FusionFaces::BASE_SHAPE_ID(),
              ModelAPI_AttributeSelection, /** Base */)

  /// Modify base object attribute of the feature.
  FEATURESAPI_EXPORT
  void setBase(const ModelHighAPI_Selection& theBase);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on FusionFaces object.
typedef std::shared_ptr<FeaturesAPI_FusionFaces> FusionFacesPtr;

/// \ingroup CPPHighAPI
/// \brief Create FusionFaces feature.
FEATURESAPI_EXPORT
FusionFacesPtr addFusionFaces(const std::shared_ptr<ModelAPI_Document>& thePart,
                              const ModelHighAPI_Selection& theBase);

#endif // FeaturesAPI_FusionFaces_H_
