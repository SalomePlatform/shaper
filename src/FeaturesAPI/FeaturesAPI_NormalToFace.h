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

#ifndef FeaturesAPI_NormalToFace_H_
#define FeaturesAPI_NormalToFace_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_NormalToFace.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>


class ModelHighAPI_Selection;

/// \class FeaturesAPI_NormalToFace
/// \ingroup CPPHighAPI
/// \brief Interface for NormalToface feature.
class FeaturesAPI_NormalToFace: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_NormalToFace(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_NormalToFace(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                    const ModelHighAPI_Selection& theBaseFace,
                                    const ModelHighAPI_Selection& theOptionalPoint);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_NormalToFace(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                    const ModelHighAPI_Selection& theBaseFace);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_NormalToFace();

  INTERFACE_3(FeaturesPlugin_NormalToFace::ID(),
              faceSelected, FeaturesPlugin_NormalToFace::FACE_SELECTED_ID(),
              ModelAPI_AttributeSelection, /** base face */,
              vertexSelected, FeaturesPlugin_NormalToFace::VERTEX_SELECTED_ID(),
              ModelAPI_AttributeSelection, /** vertex */,
              vertexOption, FeaturesPlugin_NormalToFace::VERTEX_OPTION_ID(),
              ModelAPI_AttributeString, /** use a vertex */)

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;

};

/// Pointer on the NormalToface object.
typedef std::shared_ptr<FeaturesAPI_NormalToFace> NormalPtr;

/// \ingroup CPPHighAPI
/// \brief Create normal to a face feature
/// \param thePart the part
/// \param theBaseFace the selected face
/// \param theOptionalPoint the selected point
FEATURESAPI_EXPORT
NormalPtr getNormal(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const ModelHighAPI_Selection& theBaseFace,
                    const ModelHighAPI_Selection& theOptionalPoint);

/// \ingroup CPPHighAPI
/// \brief Create normal to a face feature
/// \param thePart the part
/// \param theBaseFace the selected face
FEATURESAPI_EXPORT
NormalPtr getNormal(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const ModelHighAPI_Selection& theBaseFace);

#endif // FeaturesAPI_NormalToFace_H_
