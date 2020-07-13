// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

typedef std::pair<std::string, std::string> PairOfStrings;

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
                    const std::wstring& theExternalName);

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
  void setByExternalName(const std::wstring& theExternalName);

  /// Set center.
  SKETCHAPI_EXPORT
  void setCenter(double theX, double theY);

  /// Set center.
  SKETCHAPI_EXPORT
  void setCenter(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter);

  /// Set focus.
  SKETCHAPI_EXPORT
  void setFocus(double theX, double theY);

  /// Set focus.
  SKETCHAPI_EXPORT
  void setFocus(const std::shared_ptr<GeomAPI_Pnt2d>& theFocus);

  /// Set radius.
  SKETCHAPI_EXPORT
  void setMinorRadius(double theRadius);

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

private:
  /// Find all features connected with theEllipse by the internal coincidence.
  /// \param[in]  theEllipse        ellipse or elliptic arc
  /// \param[in]  theMajorAxis      names of attributes composing a major axis of ellipse
  /// \param[in]  theMinorAxis      names of attributes composing a minor axis of ellipse
  /// \param[out] theAttrToFeature  map attribute of ellipse and coincident auxiliary feature
  static void collectAuxiliaryFeatures(FeaturePtr theEllipse,
                                       const PairOfStrings& theMajorAxis,
                                       const PairOfStrings& theMinorAxis,
                                       std::map<std::string, FeaturePtr>& theAttrToFeature);

  /// \brief Dump the construction features (points, axes) for the ellipse.
  /// \param[in] theDumper      dumper instance
  /// \param[in] theAttributes  list of attributes the construction points are
  ///                           connected with, and their dump names
  /// \param[in] theAuxFeatures list of construction features
  static void dumpConstructionEntities(ModelHighAPI_Dumper& theDumper,
                                       const FeaturePtr& theEllipse,
                                       const std::list<PairOfStrings>& theAttributes,
                                       const std::map<std::string, FeaturePtr>& theAuxFeatures);

  /// \brief Create construction features for the ellipse connected
  ///        with corresponding attribute of ellipse.
  static std::list<std::shared_ptr<SketchAPI_SketchEntity> > buildConstructionEntities(
      const FeaturePtr& theEllipse,
      const std::list<PairOfStrings>& theAttributes,
      const std::wstring& theCenter,
      const std::wstring& theFirstFocus,
      const std::wstring& theSecondFocus,
      const std::wstring& theMajorAxisStart,
      const std::wstring& theMajorAxisEnd,
      const std::wstring& theMinorAxisStart,
      const std::wstring& theMinorAxisEnd,
      const std::wstring& theMajorAxis,
      const std::wstring& theMinorAxis);

  friend class SketchAPI_EllipticArc;
};

/// Pointer on Ellipse object.
typedef std::shared_ptr<SketchAPI_Ellipse> EllipsePtr;

#endif // SketchPlugin_Ellipse_H_
