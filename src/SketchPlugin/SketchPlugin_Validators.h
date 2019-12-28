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

#ifndef SketchPlugin_Validators_H
#define SketchPlugin_Validators_H

#include "SketchPlugin.h"
#include <ModelAPI_AttributeValidator.h>
#include <ModelAPI_FeatureValidator.h>

/**\class SketchPlugin_DistanceAttrValidator
 * \ingroup Validators
 * \brief Validator for the distance input.
 *
 * It just checks that distance is greater than zero.
 */
class SketchPlugin_DistanceAttrValidator : public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};

/**\class SketchPlugin_TangentAttrValidator
 * \ingroup Validators
 * \brief Validator for the tangent constraint input.
 *
 * It just checks that distance is greater than zero.
 */
class SketchPlugin_TangentAttrValidator : public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};

/**\class SketchPlugin_PerpendicularAttrValidator
 * \ingroup Validators
 * \brief Validator for the perpendicular constraint input.
 *
 * Checks that two arcs are not selected for perpendicular.
 */
class SketchPlugin_PerpendicularAttrValidator : public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};


/**\class SketchPlugin_NotFixedValidator
 * \ingroup Validators
 * \brief Validator for the rigid constraint input.
 *
 * It just checks there is no rigid constraint for the current feature.
 */
class SketchPlugin_NotFixedValidator : public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is not used in another rigid constraint
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};

/**\class SketchPlugin_EqualAttrValidator
 * \ingroup Validators
 * \brief Validator for the equal constraint input.
 *
 * It checks that attributes of the Equal constraint are correct.
 */
class SketchPlugin_EqualAttrValidator : public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};

/**\class SketchPlugin_MirrorAttrValidator
 * \ingroup Validators
 * \brief Validator for the mirror constraint input.
 *
 * It checks that attributes of the Mirror constraint are correct.
 */
class SketchPlugin_MirrorAttrValidator : public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute (not used)
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};


/**\class SketchPlugin_CoincidenceAttrValidator
 * \ingroup Validators
 * \brief Validator for the coincidence constraint input.
 *
 * It checks that attributes of the Coincidence constraint are correct.
 */
class SketchPlugin_CoincidenceAttrValidator : public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute (not used)
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};


/**\class SketchPlugin_CopyValidator
 * \ingroup Validators
 * \brief Validator for the constraints which create features.
 *
 * Applicable only for features, which creates another features. It verifies the produced
 * features of current constraint don't become into the list of initial features
 */
class SketchPlugin_CopyValidator : public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute (not used)
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};

/**\class SketchPlugin_SolverErrorValidator
 * \ingroup Validators
 * \brief Validator for the solver error.
 *
 * Simply checks that solver error attribute is empty. Returns the attribute value as an error.
 */
class SketchPlugin_SolverErrorValidator : public ModelAPI_FeatureValidator
{
 public:
  //! returns true if there are no solver errors
  //! \param theFeature the checked feature
  //! \param theArguments arguments of the feature (not used)
  //! \param theError error message
  virtual bool isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;

  /// Returns true if the attribute in feature is not obligatory for the feature execution
  virtual bool isNotObligatory(std::string theFeature, std::string theAttribute);
};

/**\class SketchPlugin_FilletVertexValidator
 * \ingroup Validators
 * \brief Validator for the point for fillet creation.
 *
 * Checks that selected point have exactly two coincident lines.
 */
class SketchPlugin_FilletVertexValidator : public ModelAPI_AttributeValidator
{
public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute (not used)
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};


/**\class SketchPlugin_MiddlePointAttrValidator
 * \ingroup Validators
 * \brief Validator for the middle point constraint input.
 *
 * It checks that attributes of the Middle point constraint are correct.
 */
class SketchPlugin_MiddlePointAttrValidator : public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute (not used)
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};


/**\class SketchPlugin_ArcTangentPointValidator
 * \ingroup Validators
 * \brief Validator for the point where the tangent arc is building.
 *
 * Checks that the point is a start or end point just on line or arc.
 */
class SketchPlugin_ArcTangentPointValidator : public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};

/**\class SketchPlugin_ArcTransversalPointValidator
 * \ingroup Validators
 * \brief Validator for the point where the transversal arc is building.
 *
 * Checks that the point is a start or end point just on line or arc.
 */
class SketchPlugin_ArcTransversalPointValidator : public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};


/**\class SketchPlugin_SplitValidator
 * \ingroup Validators
 * \brief Validator for the entity of the following type:
 * - Linear segment with point(s) coinident to this line
 * - Arc with point(s) coincident to the arc
 * - Circle with at least 2 split-points on this circle
 *
 * Checks that there are coincident point on selected feature.
 */
class SketchPlugin_SplitValidator : public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};

/**\class SketchPlugin_TrimValidator
 * \ingroup Validators
 * \brief Validator for the entity of the following type:
 * - Linear segment with point(s) coinident to this line or intersected it
 * - Arc with point(s) coincident to the arc or intersected it
 * - Circle with at least 2 split-points on this circle or intersected it
 *
 * Checks that there are coincident point on selected feature.
 */
class SketchPlugin_TrimValidator : public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};

/**\class SketchPlugin_IntersectionValidator
 * \ingroup Validators
 * \brief Validator for the attribute to be intersected with the sketch plane.
 */
class SketchPlugin_IntersectionValidator : public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};

/**\class SketchPlugin_ProjectionValidator
 * \ingroup Validators
 * \brief Validator for the attribute to be projected onto the sketch plane.
 */
class SketchPlugin_ProjectionValidator : public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};

/**\class SketchPlugin_DifferentReferenceValidator
 * \ingroup Validators
 * \brief Validator for attributes of a sketch feature.
 *
 * It checks that at least one of specified attributes
 * refers to another feature in respect to each other.
 */
class SketchPlugin_DifferentReferenceValidator : public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};

/**\class SketchPlugin_DifferentPointReferenceValidator
 * \ingroup Validators
 * \brief Validator for attributes of a sketch feature.
 *
 * It checks that at least two of specified attributes refer to different points.
 */
class SketchPlugin_DifferentPointReferenceValidator : public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};

/**\class SketchPlugin_CirclePassedPointValidator
 * \ingroup Validators
 * \brief Validator for passed point of MacroCircle feature.
 *
 * Checks that passed point does not refer to the feature, the center is coincident to.
 */
class SketchPlugin_CirclePassedPointValidator : public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>&,
                       Events_InfoMessage& theError) const;
};

/**\class SketchPlugin_ThirdPointValidator
 * \ingroup Validators
 * \brief Validator for the third point of MacroCircle feature.
 *
 * Checks that third point does not lie on a line passed through the first two points.
 * Checks that third point does not refer to feature lying between the first two points.
 */
class SketchPlugin_ThirdPointValidator : public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;

private:
  //! returns true if three points have not been placed on the same line
  bool arePointsNotOnLine(const FeaturePtr& theMacroFeature,
                          Events_InfoMessage& theError) const;

  //! returns true if the first two points have not been separated
  //! by a feature referred by thrid point
  bool arePointsNotSeparated(const FeaturePtr& theMacroFeature,
                             const std::list<std::string>& theArguments,
                             Events_InfoMessage& theError) const;
};

/**\class SketchPlugin_ArcEndPointValidator
 * \ingroup Validators
 * \brief Validator for the end point of MacroArc feature.
 *
 * Checks that third point does not lie on a point.
 */
class SketchPlugin_ArcEndPointValidator: public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};

/**\class SketchPlugin_ArcEndPointIntersectionValidator
 * \ingroup Validators
 * \brief Validator for the end point of MacroArc feature.
 *
 * Checks that third point does lie on edge which intersects arc.
 */
class SketchPlugin_ArcEndPointIntersectionValidator: public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};

/**\class SketchPlugin_HasNoConstraint
 * \ingroup Validators
 * \brief Validator for checking whether the feature has constraint.
 *
 * Checks that feature of the attribute does not have constraint with some kinds.
 * The kinds of constraints should be described in parameters of the validator
 * Validator processes the ModelAPI_AttributeRefAttr attribute kind
 */
class SketchPlugin_HasNoConstraint: public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};

/**\class SketchPlugin_ReplicationReferenceValidator
 * \ingroup Validators
 * \brief Validator checking that the replications features (Mirror,
 *        Multi-Rotation, Mutli-Translation) do not refer to the shapes they produce.
 */
class SketchPlugin_ReplicationReferenceValidator: public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};

/**\class SketchPlugin_SketchFeatureValidator
 * \ingroup Validators
 * \brief Validator for checking whether the feature referred by attribute is a sketch feature.
 */
class SketchPlugin_SketchFeatureValidator: public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};

/**\class SketchPlugin_MultiRotationAngleValidator
 * \ingroup Validators
 * \brief Validator for checking whether the angle of MultiRotation is in range [0, 360].
 */
class SketchPlugin_MultiRotationAngleValidator : public ModelAPI_AttributeValidator
{
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};

/**\class SketchPlugin_BSplineValidator
 * \ingroup Validators
 * \brief Validator for checking poles/weights of B-spline curve.
 */
class SketchPlugin_BSplineValidator : public ModelAPI_AttributeValidator
{
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  //! \param theError error message
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};

#endif
