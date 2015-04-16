// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintRigid.h
// Created: 30 Mar 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintRigid_H_
#define SketchSolver_ConstraintRigid_H_

#include "SketchSolver.h"
#include <SketchSolver_Constraint.h>

/** \class   SketchSolver_ConstraintRigid
 *  \ingroup Plugins
 *  \brief   Stores data of Rigid (Fixed) constraint
 *
 *  Rigid constraint may have NULL basic SketchPlugin constraint,
 *  because the Rigid constraint may be temporary for correct moving of objects
 */
class SketchSolver_ConstraintRigid : public SketchSolver_Constraint
{
public:
  /// Creates constraint to manage the given constraint from plugin
  SketchSolver_ConstraintRigid(ConstraintPtr theConstraint)
    : SketchSolver_Constraint(theConstraint)
  {}
  /// Creates temporary constraint based on feature
  SketchSolver_ConstraintRigid(FeaturePtr theFeature);

  /// \brief Tries to remove constraint
  /// \return \c false, if current constraint contains another SketchPlugin constraints (like for multiple coincidence)
  virtual bool remove(ConstraintPtr theConstraint = ConstraintPtr());

  /// \brief Returns the type of constraint
  virtual int getType() const
  { return SLVS_C_WHERE_DRAGGED; }

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process();

  /// \brief Generate list of attributes of constraint in order useful for SolveSpace constraints
  /// \param[out] theValue      numerical characteristic of constraint (e.g. distance)
  /// \param[out] theAttributes list of attributes to be filled
  virtual void getAttributes(double& theValue, std::vector<Slvs_hEntity>& theAttributes);

  /// \brief This method is used in derived objects to check consistence of constraint.
  ///        E.g. the distance between line and point may be signed.
  virtual void adjustConstraint();

private:
  /// \brief Fixing line position (start and end points)
  void fixLine(const Slvs_Entity& theLine);
  /// \brief Fixing circle (center and radius)
  void fixCircle(const Slvs_Entity& theCircle);
  /// \brief The arc is fixed differently to avoid SolveSpace problems (overconstraint)
  ///
  /// There will be fixed start and end points and the radius of the arc.
  void fixArc(const Slvs_Entity& theArc);

  /// \brief Fix given point
  void fixPoint(const Slvs_hEntity& thePointID);

  /// \brief Verifies the entity is used in any equal constraint
  /// \param[in]  theEntity entity to be found
  /// \param[out] theEqual  constraint, which uses the entity
  /// \return \c true, if the Equal constrait is found
  bool isUsedInEqual(const Slvs_Entity& theEntity, Slvs_Constraint& theEqual) const;

  /// \brief Check the entity is horizontal of vertical
  bool isAxisParallel(const Slvs_Entity& theEntity) const;

protected:
  FeaturePtr myBaseFeature; ///< fixed feature (when it is set, myBaseConstraint should be NULL)
};

#endif
