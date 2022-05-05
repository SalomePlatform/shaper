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

#ifndef SRC_SKETCHAPI_SKETCHAPI_LINE_H_
#define SRC_SKETCHAPI_SKETCHAPI_LINE_H_

//--------------------------------------------------------------------------------------
#include "SketchAPI.h"

#include <GeomDataAPI_Point2D.h>

#include <SketchPlugin_Line.h>

#include "SketchAPI_SketchEntity.h"
//--------------------------------------------------------------------------------------
class ModelHighAPI_Selection;
//--------------------------------------------------------------------------------------
/**\class SketchAPI_Line
 * \ingroup CPPHighAPI
 * \brief Interface for Line feature
 */
class SketchAPI_Line : public SketchAPI_SketchEntity
{
public:
  /// Constructor without values
  SKETCHAPI_EXPORT
  explicit SketchAPI_Line(const std::shared_ptr<ModelAPI_Feature> & theFeature);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_Line(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                 double theX1, double theY1, double theX2, double theY2);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_Line(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                 const std::shared_ptr<GeomAPI_Pnt2d> & theStartPoint,
                 const std::shared_ptr<GeomAPI_Pnt2d> & theEndPoint);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_Line(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                 const ModelHighAPI_Selection & theExternal);
  /// Constructor with values
  SKETCHAPI_EXPORT
  SketchAPI_Line(const std::shared_ptr<ModelAPI_Feature> & theFeature,
                 const std::wstring & theExternalName);
  /// Destructor
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_Line();

  INTERFACE_3(SketchPlugin_Line::ID(),
              startPoint, SketchPlugin_Line::START_ID(),
              GeomDataAPI_Point2D, /** Start point */,
              endPoint, SketchPlugin_Line::END_ID(),
              GeomDataAPI_Point2D, /** End point */,
              external, SketchPlugin_Line::EXTERNAL_ID(),
              ModelAPI_AttributeSelection, /** External */
  )

  /// Set by coordinates
  SKETCHAPI_EXPORT
  void setByCoordinates(double theX1, double theY1, double theX2, double theY2);

  /// Set by points
  SKETCHAPI_EXPORT
  void setByPoints(const std::shared_ptr<GeomAPI_Pnt2d> & theStartPoint,
                   const std::shared_ptr<GeomAPI_Pnt2d> & theEndPoint);

  /// Set by external
  SKETCHAPI_EXPORT
  void setByExternal(const ModelHighAPI_Selection & theExternal);

  /// Set by external name
  SKETCHAPI_EXPORT
  void setByExternalName(const std::wstring & theExternalName);

  /// Set start point
  SKETCHAPI_EXPORT
  void setStartPoint(double theX, double theY);

  /// Set start point
  SKETCHAPI_EXPORT
  void setStartPoint(const std::shared_ptr<GeomAPI_Pnt2d> & thePoint);

  /// Set end point
  SKETCHAPI_EXPORT
  void setEndPoint(double theX, double theY);

  /// Set end point
  SKETCHAPI_EXPORT
  void setEndPoint(const std::shared_ptr<GeomAPI_Pnt2d> & thePoint);

  /// Dump wrapped feature
  SKETCHAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

//! Pointer on Line object
typedef std::shared_ptr<SketchAPI_Line> LinePtr;

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_SKETCHAPI_SKETCHAPI_LINE_H_ */
