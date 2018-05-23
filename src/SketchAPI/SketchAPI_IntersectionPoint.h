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

#ifndef SRC_SKETCHAPI_SKETCHAPI_INTERSECTIONPOINT_H_
#define SRC_SKETCHAPI_SKETCHAPI_INTERSECTIONPOINT_H_

//--------------------------------------------------------------------------------------
#include "SketchAPI.h"

#include <SketchPlugin_IntersectionPoint.h>

#include "SketchAPI_SketchEntity.h"
//--------------------------------------------------------------------------------------
class ModelHighAPI_Selection;
//--------------------------------------------------------------------------------------
/**\class SketchAPI_IntersectionPoint
 * \ingroup CPPHighAPI
 * \brief Interface for IntersectionPoint feature
 */
class SketchAPI_IntersectionPoint : public SketchAPI_SketchEntity
{
public:
  /// Constructor without values
  SKETCHAPI_EXPORT
  explicit SketchAPI_IntersectionPoint(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_IntersectionPoint(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                 const ModelHighAPI_Selection & theExternal);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_IntersectionPoint(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                 const std::string & theExternalName);
  /// Destructor
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_IntersectionPoint();

  INTERFACE_4(SketchPlugin_IntersectionPoint::ID(),
              coordinates, SketchPlugin_IntersectionPoint::COORD_ID(),
              GeomDataAPI_Point2D, /** IntersectionPoint coordinates */,
              externalFeature, SketchPlugin_IntersectionPoint::EXTERNAL_FEATURE_ID(),
              ModelAPI_AttributeSelection, /** External edge */,
              external, SketchPlugin_IntersectionPoint::EXTERNAL_ID(),
              ModelAPI_AttributeSelection, /** External */,
              includeToResult, SketchPlugin_IntersectionPoint::INCLUDE_INTO_RESULT(),
              ModelAPI_AttributeBoolean, /** Include into result */
  )

  /// Set by external
  SKETCHAPI_EXPORT
  void setByExternalLine(const ModelHighAPI_Selection & theExternalLine);

  /// Set by external name
  SKETCHAPI_EXPORT
  void setByExternalLineName(const std::string & theExternalLineName);

  /// Dump wrapped feature
  SKETCHAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

//! Pointer on IntersectionPoint object
typedef std::shared_ptr<SketchAPI_IntersectionPoint> IntersectionPointPtr;

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_SKETCHAPI_SKETCHAPI_INTERSECTIONPOINT_H_ */
