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

#ifndef PlaneGCSSolver_Tools_H_
#define PlaneGCSSolver_Tools_H_

#include <SketchSolver_Constraint.h>
#include <SketchSolver_ConstraintMovement.h>
#include <SketchPlugin_Constraint.h>

class GeomAPI_Circ2d;
class GeomAPI_Ellipse2d;
class GeomAPI_Lin2d;
class GeomAPI_Pnt2d;

class ModelAPI_AttributeDoubleArray;
class ModelAPI_AttributeIntArray;

/** \namespace  PlaneGCSSolver_Tools
 *  \ingroup    Plugins
 *  \brief      Converter tools
 */
namespace PlaneGCSSolver_Tools
{
  /// \brief Creates a solver's constraint using given SketchPlugin constraint
  ///        or returns empty pointer if not all attributes are correct
  SolverConstraintPtr createConstraint(ConstraintPtr theConstraint);

  /// \brief Creates temporary constraint to fix the feature after movement
  std::shared_ptr<SketchSolver_ConstraintMovement>
      createMovementConstraint(FeaturePtr theMovedFeature);
  /// \brief Creates temporary constraint to fix the attribute after movement
  std::shared_ptr<SketchSolver_ConstraintMovement>
      createMovementConstraint(AttributePtr theMovedAttribute);
  /// \brief Creates temporary constraint to fix the point in array after movement
  std::shared_ptr<SketchSolver_ConstraintMovement>
      createMovementConstraint(const std::pair<AttributePtr, int>& theMovedPointInArray);

  /// \brief Creates new constraint using given parameters
  /// \param theConstraint [in]  original constraint
  /// \param theType       [in]  type of constraint
  /// \param theValue      [in]  numeric characteristic of constraint
  ///                            (e.g. distance or radius) if applicable
  /// \param theEntity1    [in]  first attribute of constraint
  /// \param theEntity2    [in]  second attribute of constraint
  /// \param theEntity3    [in]  third attribute of constraint
  /// \param theEntity4    [in]  fourth attribute of constraint
  /// \return Created wrapper of constraints applicable for specific solver.
  ConstraintWrapperPtr createConstraint(ConstraintPtr theConstraint,
                                        const SketchSolver_ConstraintType& theType,
                                        const EntityWrapperPtr& theValue,
                                        const EntityWrapperPtr& theEntity1,
                                        const EntityWrapperPtr& theEntity2 = EntityWrapperPtr(),
                                        const EntityWrapperPtr& theEntity3 = EntityWrapperPtr(),
                                        const EntityWrapperPtr& theEntity4 = EntityWrapperPtr());

  /// \brief Return \c true if the attribute is used in PlaneGCS solver
  /// \param[in] theAttrName  name of the attribute
  /// \param[in] theOwnerName name of the parent feature
  bool isAttributeApplicable(const std::string& theAttrName,
                             const std::string& theOwnerName);

  /// \brief Convert entity to point
  /// \return empty pointer if the entity is not a point
  std::shared_ptr<GeomAPI_Pnt2d> point(EntityWrapperPtr theEntity);
  /// \brief Convert entity to line
  /// \return empty pointer if the entity is not a line
  std::shared_ptr<GeomAPI_Lin2d> line(EntityWrapperPtr theEntity);
  /// \brief Convert entity to circle
  /// \return empty pointer if the entity is not a circle
  std::shared_ptr<GeomAPI_Circ2d> circle(EntityWrapperPtr theEntity);
  /// \brief Convert entity to ellipse
  /// \return empty pointer if the entity is not an ellipse
  std::shared_ptr<GeomAPI_Ellipse2d> ellipse(EntityWrapperPtr theEntity);

  /// \brief Convert entity to line
  /// \return empty pointer if the entity is not a line
  std::shared_ptr<GeomAPI_Lin2d> line(FeaturePtr theFeature);

  /// \brief Update start and end parameters of circular and elliptic arcs
  ///        respectively to start and end points on the arc.
  ///        For the circular arc, the radius is calculated too.
  void recalculateArcParameters(EntityWrapperPtr theArc);

  /// brief Return list of parameters for the given entity
  GCS::SET_pD parameters(const EntityWrapperPtr& theEntity);

  /// \brief Update value in theDest if theSource is differ more than theTolerance
  /// \return \c true if the value was updated.
  bool updateValue(const double& theSource, double& theDest,
                   const double theTolerance = 1.e4 * tolerance);

  double distance(const GCS::Point& thePnt1, const GCS::Point& thePnt2);

  /// \brief Provide an interface to access values in attribute which is an array of values
  class AttributeArray
  {
  public:
    AttributeArray(AttributePtr theAttribute);

    bool isInitialized() const;

    int size() const;

    double value(const int theIndex) const;

  private:
    std::shared_ptr<ModelAPI_AttributeDoubleArray> myDouble;
    std::shared_ptr<ModelAPI_AttributeIntArray> myInteger;
  };
};

#endif
