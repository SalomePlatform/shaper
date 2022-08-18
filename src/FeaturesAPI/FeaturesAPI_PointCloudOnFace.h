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

#ifndef FeaturesAPI_PointCloudOnFace_H_
#define FeaturesAPI_PointCloudOnFace_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_PointCloudOnFace.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>
#include <ModelHighAPI_Integer.h>


class ModelHighAPI_Selection;

/// \class FeaturesAPI_PointCloudOnFace
/// \ingroup CPPHighAPI
/// \brief Interface for PointCloudOnFace feature.
class FeaturesAPI_PointCloudOnFace: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_PointCloudOnFace(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_PointCloudOnFace(const std::shared_ptr<ModelAPI_Feature>& theFeature,
					const ModelHighAPI_Selection& theFace,
					const ModelHighAPI_Integer& theNumber);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_PointCloudOnFace();

  INTERFACE_2(FeaturesPlugin_PointCloudOnFace::ID(),
              faceSelected, FeaturesPlugin_PointCloudOnFace::FACE_SELECTED_ID(),
              ModelAPI_AttributeSelection, /** face */,
              nbPoints, FeaturesPlugin_PointCloudOnFace::NUMBER_ID(),
              ModelAPI_AttributeInteger, /** Number of points */)

  /// Modify CreationMethod, nb_points attribute of the feature.
  FEATURESAPI_EXPORT
  void setNumberOfPoints(const ModelHighAPI_Integer& theNumber);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;

};

/// Pointer on the PointCloudOnFace object.
typedef std::shared_ptr<FeaturesAPI_PointCloudOnFace> PointCloudPtr;

/// \ingroup CPPHighAPI
/// \brief Create point cloud on a face feature
/// \param thePart the part
/// \param theFace the selected face
/// \param theNumberOfPoints the selected point
FEATURESAPI_EXPORT
PointCloudPtr makeVertexInsideFace(const std::shared_ptr<ModelAPI_Document>& thePart,
                                   const ModelHighAPI_Selection& theFace,
                                   const ModelHighAPI_Integer& theNumber);

#endif // FeaturesAPI_PointCloudOnFace_H_
