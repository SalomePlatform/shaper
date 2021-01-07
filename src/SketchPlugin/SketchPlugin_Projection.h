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

#ifndef SketchPlugin_Projection_H_
#define SketchPlugin_Projection_H_

#include "SketchPlugin_SketchEntity.h"

class GeomAPI_Curve;

/** \class SketchPlugin_Projection
 *  \ingroup Plugins
 *  \brief Feature for creation of external feature as a projection onto the sketch plane.
 */
class SketchPlugin_Projection : public SketchPlugin_SketchEntity
{
public:
  /// Projection feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_PROJECTION_ID("SketchProjection");
    return MY_PROJECTION_ID;
  }
  /// Returns the kind of a feature
  virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_Projection::ID();
    return MY_KIND;
  }

  static const std::string& EXTERNAL_FEATURE_ID()
  {
    static std::string MY_EXT_FEATURE_ID("ExternalFeature");
    return MY_EXT_FEATURE_ID;
  }

  static const std::string& PROJECTED_FEATURE_ID()
  {
    static std::string MY_PROJ_FEATURE_ID("ProjectedFeature");
    return MY_PROJ_FEATURE_ID;
  }

  static const std::string& INCLUDE_INTO_RESULT()
  {
    static std::string MY_INCLUDE("IncludeToResult");
    return MY_INCLUDE;
  }

  static const std::string& KEEP_REFERENCE_ID()
  {
    static std::string ID("keep_reference");
    return ID;
  }

  static const std::string& MAKE_FIXED()
  {
    static std::string ID("make_fixed");
    return ID;
  }

  static const std::string& FIXED_CONSTRAINT_ID()
  {
    static std::string ID("fixed_constraint");
    return ID;
  }

  /// Returns true because projected feature is always external
  virtual bool isFixed()
  { return true; }

  /// Returns true if the feature and the feature results can be displayed.
  /// \return false
  SKETCHPLUGIN_EXPORT virtual bool canBeDisplayed() const
  {
    return false;
  }

  /// Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Called on change of any argument-attribute of this object: for external point
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Returns true if this feature is used as macro: creates other features and then removed.
  /// This feature may change its macro-state according to selected item.
  /// \returns false by default
  SKETCHPLUGIN_EXPORT virtual bool isMacro() const;

  /// Use plugin manager for features creation
  SketchPlugin_Projection();

protected:
  /// \brief Initializes attributes of derived class.
  virtual void initDerivedClassAttributes();

  /// \brief Initializes attributes of keeping the reference to the original shape.
  virtual void initDerivedClassAttributes2();

private:
  /// \brief Find projection of a feature onto sketch plane
  void computeProjection(const std::string& theID);

  /// \brief Project point to the sketch plane
  bool projectPoint(FeaturePtr& theProjection, const std::shared_ptr<GeomAPI_Pnt>& thePoint);
  /// \brief Project segment to the sketch plane
  bool projectSegment(FeaturePtr& theProjection, const std::shared_ptr<GeomAPI_Edge>& theEdge);
  /// \brief Project any edge to sketch plane
  bool projectEdge(FeaturePtr& theProjection, const std::shared_ptr<GeomAPI_Edge>& theEdge);

  /// \brief Fill attributes of the Arc feature
  bool fillArc(FeaturePtr& theProjection,
               const std::shared_ptr<GeomAPI_Curve>& theArc,
               const std::shared_ptr<GeomAPI_Pln>& thePlane,
               const std::shared_ptr<GeomAPI_Edge>& theOriginalEdge);
  /// \brief Fill attributes of the Circle feature
  bool fillCircle(FeaturePtr& theProjection,
                  const std::shared_ptr<GeomAPI_Curve>& theCircle,
                  const std::shared_ptr<GeomAPI_Pln>& thePlane);
  /// \brief Fill attributes of the Ellipse feature
  bool fillEllipse(FeaturePtr& theProjection,
                   const std::shared_ptr<GeomAPI_Curve>& theEllipse,
                   const std::shared_ptr<GeomAPI_Pln>& thePlane);
  /// \brief Fill attributes of the EllipticArc feature
  bool fillEllipticArc(FeaturePtr& theProjection,
                       const std::shared_ptr<GeomAPI_Curve>& theEllipticArc,
                       const std::shared_ptr<GeomAPI_Pln>& thePlane,
                       const std::shared_ptr<GeomAPI_Edge>& theOriginalEdge);
  /// \brief Fill attributes of the B-spline feature
  bool fillBSpline(FeaturePtr& theProjection,
                   const std::shared_ptr<GeomAPI_Curve>& theCurve,
                   const std::shared_ptr<GeomAPI_Pln>& thePlane);

  /// \brief Delete already calculated projected feature
  ///        if the selection of the projection is changed
  /// \param[in/out] theProjection   projected feature
  /// \param[in] theSupportedTypes   types supported relatively to the base selection
  /// \param[in] theRequestedFeature type of the new feature to be created
  ///                                (remove only if empty string).
  bool rebuildProjectedFeature(FeaturePtr& theProjection,
                               const std::set<std::string>& theSupportedTypes,
                               const std::string& theRequestedFeature = std::string());

  bool myIsComputing;
};

#endif
