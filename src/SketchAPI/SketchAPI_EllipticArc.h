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

#ifndef SketchAPI_EllipticArc_H_
#define SketchAPI_EllipticArc_H_

#include "SketchAPI.h"
#include "SketchAPI_SketchEntity.h"

#include <SketchPlugin_EllipticArc.h>

class ModelHighAPI_Selection;

/// \class SketchAPI_EllipticArc
/// \ingroup CPPHighAPI
/// \brief Interface for Elliptic Arc feature.
class SketchAPI_EllipticArc : public SketchAPI_SketchEntity
{
public:
  /// Constructor without values.
  SKETCHAPI_EXPORT
  explicit SketchAPI_EllipticArc(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_EllipticArc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                        double theCenterX, double theCenterY,
                        double theFocusX, double theFocusY,
                        double theStartX, double theStartY,
                        double theEndX, double theEndY,
                        bool theInversed);

  /// Constructor with values.
  SKETCHAPI_EXPORT
  SketchAPI_EllipticArc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                        const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                        const std::shared_ptr<GeomAPI_Pnt2d>& theFocus,
                        const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                        const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                        bool theInversed);

  /// Constructor with external.
  SKETCHAPI_EXPORT
  SketchAPI_EllipticArc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                        const ModelHighAPI_Selection& theExternal);

  /// Constructor with external.
  SKETCHAPI_EXPORT
  SketchAPI_EllipticArc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                        const std::wstring& theExternalName);

  /// Destructor.
  SKETCHAPI_EXPORT
  virtual ~SketchAPI_EllipticArc();

  INTERFACE_13(SketchPlugin_EllipticArc::ID(),
               center, SketchPlugin_EllipticArc::CENTER_ID(),
               GeomDataAPI_Point2D, /** Center point */,
               startPoint, SketchPlugin_EllipticArc::START_POINT_ID(),
               GeomDataAPI_Point2D, /** Start point of elliptic arc */,
               endPoint, SketchPlugin_EllipticArc::END_POINT_ID(),
               GeomDataAPI_Point2D, /** End point of elliptic arc */,
               reversed, SketchPlugin_EllipticArc::REVERSED_ID(),
               ModelAPI_AttributeBoolean, /** Inversed flag */,
               firstFocus, SketchPlugin_EllipticArc::FIRST_FOCUS_ID(),
               GeomDataAPI_Point2D, /** Focus in positive direction of a major axis */,
               secondFocus, SketchPlugin_EllipticArc::SECOND_FOCUS_ID(),
               GeomDataAPI_Point2D, /** Focus in negative direction of a major axis */,
               majorAxisNegative, SketchPlugin_EllipticArc::MAJOR_AXIS_START_ID(),
               GeomDataAPI_Point2D, /** Start point of major axis */,
               majorAxisPositive, SketchPlugin_EllipticArc::MAJOR_AXIS_END_ID(),
               GeomDataAPI_Point2D, /** End point of major axis */,
               minorAxisNegative, SketchPlugin_EllipticArc::MINOR_AXIS_START_ID(),
               GeomDataAPI_Point2D, /** Start point of minor axis */,
               minorAxisPositive, SketchPlugin_EllipticArc::MINOR_AXIS_END_ID(),
               GeomDataAPI_Point2D, /** End point of minor axis */,
               majorRadius, SketchPlugin_EllipticArc::MAJOR_RADIUS_ID(),
               ModelAPI_AttributeDouble, /** Major radius */,
               minorRadius, SketchPlugin_EllipticArc::MINOR_RADIUS_ID(),
               ModelAPI_AttributeDouble, /** Minor radius */,
               external, SketchPlugin_EllipticArc::EXTERNAL_ID(),
               ModelAPI_AttributeSelection, /** External */)

  /// Set by center, focus and radius.
  SKETCHAPI_EXPORT
  void setByCenterFocusAndPoints(double theCenterX, double theCenterY,
                                 double theFocusX, double theFocusY,
                                 double theStartX, double theStartY,
                                 double theEndX, double theEndY,
                                 bool theInversed);

  /// Set by center, focus and radius.
  SKETCHAPI_EXPORT
  void setByCenterFocusAndPoints(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                 const std::shared_ptr<GeomAPI_Pnt2d>& theFocus,
                                 const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                                 const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                                 bool theInversed);

  /// Set by external.
  SKETCHAPI_EXPORT
  void setByExternal(const ModelHighAPI_Selection& theExternal);

  /// Set by external name.
  SKETCHAPI_EXPORT
  void setByExternalName(const std::wstring& theExternalName);

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
  void setFocus(const std::shared_ptr<GeomAPI_Pnt2d> & theFocus);

  /// Create construction elements (focuses, axes etc.).
  /// Empty value for each parameter shows that the corresponding feature has been removed.
  /// Value "aux" marks this feature as auxiliary.
  /// And the name of the feature shows that it is a regular feature.
  SKETCHAPI_EXPORT
  std::list<std::shared_ptr<SketchAPI_SketchEntity> > construction(
      const std::wstring& center = std::wstring(),
      const std::wstring& firstFocus = std::wstring(),
      const std::wstring& secondFocus = std::wstring(),
      const std::wstring& majorAxisStart = std::wstring(),
      const std::wstring& majorAxisEnd = std::wstring(),
      const std::wstring& minorAxisStart = std::wstring(),
      const std::wstring& minorAxisEnd = std::wstring(),
      const std::wstring& majorAxis = std::wstring(),
      const std::wstring& minorAxis = std::wstring()) const;

  /// Dump wrapped feature
  SKETCHAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Ellipse object.
typedef std::shared_ptr<SketchAPI_EllipticArc> EllipticArcPtr;

#endif // SketchAPI_EllipticArc_H_
