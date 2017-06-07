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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef SRC_SKETCHAPI_SKETCHAPI_POINT_H_
#define SRC_SKETCHAPI_SKETCHAPI_POINT_H_

//--------------------------------------------------------------------------------------
#include "SketchAPI.h"

#include <GeomDataAPI_Point2D.h>

#include <SketchPlugin_Point.h>

#include "SketchAPI_SketchEntity.h"
//--------------------------------------------------------------------------------------
class ModelHighAPI_Selection;
//--------------------------------------------------------------------------------------
/**\class SketchAPI_Point
 * \ingroup CPPHighAPI
 * \brief Interface for Point feature
 */
class SketchAPI_Point : public SketchAPI_SketchEntity
{
public:
  /// Constructor without values
  SKETCHAPI_EXPORT
  explicit SketchAPI_Point(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_Point(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                 double theX, double theY);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_Point(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                 const std::shared_ptr<GeomAPI_Pnt2d> & thePoint);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_Point(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                 const ModelHighAPI_Selection & theExternal);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_Point(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                 const std::string & theExternalName);
  /// Destructor
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_Point();

  INTERFACE_2(SketchPlugin_Point::ID(),
              coordinates, SketchPlugin_Point::COORD_ID(),
              GeomDataAPI_Point2D, /** Point coordinates */,
              external, SketchPlugin_Point::EXTERNAL_ID(),
              ModelAPI_AttributeSelection, /** External */
  )

  /// Set by coordinates
  SKETCHAPI_EXPORT
  void setCoordinates(double theX, double theY);

  /// Set by points
  SKETCHAPI_EXPORT
  void setCoordinates(const std::shared_ptr<GeomAPI_Pnt2d> & thePoint);

  /// Set by external
  SKETCHAPI_EXPORT
  void setByExternal(const ModelHighAPI_Selection & theExternal);

  /// Set by external name
  SKETCHAPI_EXPORT
  void setByExternalName(const std::string & theExternalName);

  /// Dump wrapped feature
  SKETCHAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

//! Pointer on Point object
typedef std::shared_ptr<SketchAPI_Point> PointPtr;

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_SKETCHAPI_SKETCHAPI_POINT_H_ */
