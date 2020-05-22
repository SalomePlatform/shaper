// Copyright (C) 2020  CEA/DEN, EDF R&D
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

#ifndef SketchPlugin_CurveFitting_H_
#define SketchPlugin_CurveFitting_H_

#include <SketchPlugin.h>
#include <SketchPlugin_SketchEntity.h>

#include <GeomAPI_IPresentable.h>

class GeomAPI_Edge;

/**\class SketchPlugin_CurveFitting
 * \ingroup Plugins
 * \brief Feature for creation of the new B-spline curve in sketch
 *        as an interpolation or an approximation of a list of points.
 */
class SketchPlugin_CurveFitting : public SketchPlugin_SketchEntity,
                                  public GeomAPI_IPresentable
{
public:
  /// Interpolation macro feature kind
  inline static const std::string& ID()
  {
    static const std::string ID("SketchCurveFitting");
    return ID;
  }

  /// list of selected points
  inline static const std::string& POINTS_ID()
  {
    static const std::string ID("points");
    return ID;
  }

  /// attribute for the periodic flag
  inline static const std::string& PERIODIC_ID()
  {
    static const std::string ID("periodic");
    return ID;
  }

  /// attribute for the closed flag
  inline static const std::string& CLOSED_ID()
  {
    static const std::string ID("closed");
    return ID;
  }

  /// attribute for the flag of creation a control polygon
  inline static const std::string& NEED_CONTROL_POLYGON_ID()
  {
    static const std::string ID("need_control_poly");
    return ID;
  }

  /// attribute for the type of the operation
  inline static const std::string& TYPE_ID()
  {
    static const std::string ID("type");
    return ID;
  }

  /// value for the type of operation
  inline static const std::string& TYPE_INTERPOLATION_ID()
  {
    static const std::string ID("interpolation_type");
    return ID;
  }

  /// value for the type of operation
  inline static const std::string& TYPE_APPROXIMATION_ID()
  {
    static const std::string ID("approximation_type");
    return ID;
  }

  /// attribute for the precision of the approximation
  inline static const std::string& PRECISION_ID()
  {
    static const std::string ID("precision");
    return ID;
  }

  /// attribute for the closed flag
  inline static const std::string& REORDER_POINTS_ACTION_ID()
  {
    static const std::string ID("reorder_points");
    return ID;
  }

  /// Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_CurveFitting::ID();
    return MY_KIND;
  }

  /// Returns the AIS preview
  virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Reimplemented from ModelAPI_Feature::isMacro().
  /// \returns true
  SKETCHPLUGIN_EXPORT virtual bool isMacro() const {return true;};

  SKETCHPLUGIN_EXPORT virtual bool isPreviewNeeded() const {return false;};

  /// Performs some functionality by action id.
  /// \param[in] theAttributeId action key id.
  /// \return false in case if action not performed.
  SKETCHPLUGIN_EXPORT virtual bool customAction(const std::string& theActionId);

  /// Use plugin manager for features creation
  SketchPlugin_CurveFitting();

protected:
  /// \brief Initializes attributes of derived class.
  virtual void initDerivedClassAttributes();

private:
  /// \brief Create a feature, which passes through the selected points
  FeaturePtr createBSplineFeature();

  /// \brief Create coincidence constraints between selected points and the produced curve.
  void createConstraints(FeaturePtr theProducedFeature);

  /// \brief Reorder point to compose the polyline of the minimal length
  void reorderPoints();

private:
  std::shared_ptr<GeomAPI_Edge> myTransientResult; ///< Interpolation curve
};

#endif
