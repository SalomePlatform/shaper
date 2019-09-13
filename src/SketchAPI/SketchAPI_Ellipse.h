// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef SketchAPI_Ellipse_H_
#define SketchAPI_Ellipse_H_

#include "SketchAPI.h"
#include "SketchAPI_SketchEntity.h"

#include <SketchPlugin_Ellipse.h>

class ModelHighAPI_Selection;

/// \class SketchAPI_Ellipse
/// \ingroup CPPHighAPI
/// \brief Interface for Ellipse feature.
class SketchAPI_Ellipse : public SketchAPI_SketchEntity
{
public:
  /// Constructor without values.
  SKETCHAPI_EXPORT
  explicit SketchAPI_Ellipse(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_Ellipse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                    double theCenterX, double theCenterY,
                    double theFocusX, double theFocusY,
                    double theMinorRadius);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_Ellipse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                    const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                    const std::shared_ptr<GeomAPI_Pnt2d>& theFocus,
                    double theMinorRadius);

  /// Constructor with external.
  SKETCHAPI_EXPORT
  SketchAPI_Ellipse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                    const ModelHighAPI_Selection& theExternal);

  /// Constructor with external.
  SKETCHAPI_EXPORT
  SketchAPI_Ellipse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                    const std::string& theExternalName);

  /// Destructor.
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_Ellipse();

  INTERFACE_10(SketchPlugin_Ellipse::ID(),
               center, SketchPlugin_Ellipse::CENTER_ID(),
               GeomDataAPI_Point2D, /** Center point */,
               firstFocus, SketchPlugin_Ellipse::FIRST_FOCUS_ID(),
               GeomDataAPI_Point2D, /** Focus in positive direction of a major axis */,
               secondFocus, SketchPlugin_Ellipse::SECOND_FOCUS_ID(),
               GeomDataAPI_Point2D, /** Focus in negative direction of a major axis */,
               majorAxisNegative, SketchPlugin_Ellipse::MAJOR_AXIS_START_ID(),
               GeomDataAPI_Point2D, /** Start point of major axis */,
               majorAxisPositive, SketchPlugin_Ellipse::MAJOR_AXIS_END_ID(),
               GeomDataAPI_Point2D, /** End point of major axis */,
               minorAxisNegative, SketchPlugin_Ellipse::MINOR_AXIS_START_ID(),
               GeomDataAPI_Point2D, /** Start point of minor axis */,
               minorAxisPositive, SketchPlugin_Ellipse::MINOR_AXIS_END_ID(),
               GeomDataAPI_Point2D, /** End point of minor axis */,
               majorRadius, SketchPlugin_Ellipse::MAJOR_RADIUS_ID(),
               ModelAPI_AttributeDouble, /** Major radius */,
               minorRadius, SketchPlugin_Ellipse::MINOR_RADIUS_ID(),
               ModelAPI_AttributeDouble, /** Minor radius */,
               external, SketchPlugin_Ellipse::EXTERNAL_ID(),
               ModelAPI_AttributeSelection, /** External */)

  /// Set by center, focus and radius.
  SKETCHAPI_EXPORT
  void setByCenterFocusAndRadius(double theCenterX, double theCenterY,
                                 double theFocusX, double theFocusY,
                                 double theMinorRadius);

  /// Set by center, focus and radius.
  SKETCHAPI_EXPORT
  void setByCenterFocusAndRadius(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                 const std::shared_ptr<GeomAPI_Pnt2d>& theFocus,
                                 double theMinorRadius);

  /// Set by external.
  SKETCHAPI_EXPORT
  void setByExternal(const ModelHighAPI_Selection& theExternal);

  /// Set by external name.
  SKETCHAPI_EXPORT
  void setByExternalName(const std::string& theExternalName);

  /// Set center.
  SKETCHAPI_EXPORT
  void setCenter(double theX, double theY);

  /// Set center.
  SKETCHAPI_EXPORT
  void setCenter(const std::shared_ptr<GeomAPI_Pnt2d> & theCenter);

  /// Set focus.
  SKETCHAPI_EXPORT
  void setFocus(double theX, double theY);

  /// Set focus.
  SKETCHAPI_EXPORT
  void setFocus(const std::shared_ptr<GeomAPI_Pnt2d> & theCenter);

  /// Set radius.
  SKETCHAPI_EXPORT
  void setMinorRadius(double theRadius);

  /// Return major axis of the ellipse
  SKETCHAPI_EXPORT
  ModelHighAPI_Selection majorAxis() const;

  /// Return minor axis of the ellipse
  SKETCHAPI_EXPORT
  ModelHighAPI_Selection minorAxis() const;

  /// Dump wrapped feature
  SKETCHAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Ellipse object.
typedef std::shared_ptr<SketchAPI_Ellipse> EllipsePtr;

#endif // SketchPlugin_Ellipse_H_
