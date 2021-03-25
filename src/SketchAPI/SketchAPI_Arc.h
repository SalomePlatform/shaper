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

#ifndef SketchAPI_Arc_H_
#define SketchAPI_Arc_H_

#include "SketchAPI.h"
#include "SketchAPI_SketchEntity.h"

#include <GeomDataAPI_Point2D.h>

#include <SketchPlugin_Arc.h>

class ModelHighAPI_RefAttr;
class ModelHighAPI_Selection;

/// \class SketchAPI_Arc
/// \ingroup CPPHighAPI
/// \brief Interface for Arc feature.
class SketchAPI_Arc: public SketchAPI_SketchEntity
{
public:
  /// Constructor without values.
  SKETCHAPI_EXPORT
  explicit SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                double theCenterX, double theCenterY,
                double theStartX, double theStartY,
                double theEndX, double theEndY,
                bool theInversed);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                bool theInversed);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                const ModelHighAPI_Selection& theExternal);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                const std::wstring& theExternalName);

  /// Destructor.
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_Arc();

  INTERFACE_7(SketchPlugin_Arc::ID(),
               center, SketchPlugin_Arc::CENTER_ID(),
               GeomDataAPI_Point2D, /** Center point */,
               startPoint, SketchPlugin_Arc::START_ID(),
               GeomDataAPI_Point2D, /** Start point */,
               endPoint, SketchPlugin_Arc::END_ID(),
               GeomDataAPI_Point2D, /** End point */,
               reversed, SketchPlugin_Arc::REVERSED_ID(),
               ModelAPI_AttributeBoolean, /** Inversed flag */,
               radius, SketchPlugin_Arc::RADIUS_ID(),
               ModelAPI_AttributeDouble, /** Radius */,
               angle, SketchPlugin_Arc::ANGLE_ID(),
               ModelAPI_AttributeDouble, /** Angle */,
               external, SketchPlugin_Arc::EXTERNAL_ID(),
               ModelAPI_AttributeSelection, /** External */)

  /// Set by center and start, end point.
  SKETCHAPI_EXPORT
  void setByCenterStartEnd(double theCenterX, double theCenterY,
                           double theStartX, double theStartY,
                           double theEndX, double theEndY,
                           bool theInversed);

  /// Set by center and start, end point.
  SKETCHAPI_EXPORT
  void setByCenterStartEnd(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                           const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                           const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                           bool theInversed);

  /// Set by external.
  SKETCHAPI_EXPORT
  void setByExternal(const ModelHighAPI_Selection& theExternal);

  /// Set by external name.
  SKETCHAPI_EXPORT
  void setByExternalName(const std::wstring& theExternalName);

  /// Dump wrapped feature
  SKETCHAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Arc object.
typedef std::shared_ptr<SketchAPI_Arc> ArcPtr;

#endif // SketchAPI_Arc_H_
