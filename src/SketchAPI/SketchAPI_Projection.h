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

#ifndef SRC_SKETCHAPI_SKETCHAPI_PROJECTION_H_
#define SRC_SKETCHAPI_SKETCHAPI_PROJECTION_H_

//--------------------------------------------------------------------------------------
#include "SketchAPI.h"

#include <SketchPlugin_Projection.h>

#include "SketchAPI_SketchEntity.h"
//--------------------------------------------------------------------------------------
class ModelHighAPI_Selection;
//--------------------------------------------------------------------------------------
/**\class SketchAPI_Projection
 * \ingroup CPPHighAPI
 * \brief Interface for Projection feature
 */
class SketchAPI_Projection : public SketchAPI_SketchEntity
{
public:
  /// Constructor without values
  SKETCHAPI_EXPORT
  explicit SketchAPI_Projection(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_Projection(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                       const ModelHighAPI_Selection & theExternalFeature);
  /// Destructor
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_Projection();

  INTERFACE_4(SketchPlugin_Projection::ID(),
              externalFeature, SketchPlugin_Projection::EXTERNAL_FEATURE_ID(),
              ModelAPI_AttributeSelection, /** External feature */,
              projectedFeature, SketchPlugin_Projection::PROJECTED_FEATURE_ID(),
              ModelAPI_AttributeRefAttr, /** Projected feature */,
              external, SketchPlugin_Projection::EXTERNAL_ID(),
              ModelAPI_AttributeSelection, /** External */,
              includeToResult, SketchPlugin_Projection::INCLUDE_INTO_RESULT(),
              ModelAPI_AttributeBoolean, /** Include into result */
  )

  /// Set external feature
  SKETCHAPI_EXPORT
  void setExternalFeature(const ModelHighAPI_Selection & theExternalLine);

  /// Set flag to include projection to result or not
  SKETCHAPI_EXPORT
  void setIncludeToResult(bool theKeepResult);

  /// Set flag to keep the reference to the original shape
  SKETCHAPI_EXPORT
  void setKeepReferenceToOriginal(bool theKeepRefToOriginal);

  /// Returns created feature
  SKETCHAPI_EXPORT
  std::shared_ptr<SketchAPI_SketchEntity> createdFeature() const;

  /// Dump wrapped feature
  SKETCHAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

//! Pointer on Projection object
typedef std::shared_ptr<SketchAPI_Projection> ProjectionPtr;

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_SKETCHAPI_SKETCHAPI_PROJECTION_H_ */
