// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintFixed.h
// Created: 30 Mar 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintFixed_H_
#define SketchSolver_ConstraintFixed_H_

#include <SketchSolver_Constraint.h>

/** \class   SketchSolver_ConstraintFixed
 *  \ingroup Plugins
 *  \brief   Stores data of the Fixed constraint
 */
class SketchSolver_ConstraintFixed : public SketchSolver_Constraint
{
public:
  /// Creates constraint to manage the given constraint from plugin
  SketchSolver_ConstraintFixed(ConstraintPtr theConstraint);
#ifndef SUPPORT_NEW_MOVE
  /// Creates temporary constraint based on feature (useful while the feature is being moved)
  SketchSolver_ConstraintFixed(FeaturePtr theFeature);
#endif

  /// \brief Block or unblock events from this constraint
  virtual void blockEvents(bool isBlocked);

#ifndef SUPPORT_NEW_MOVE
  /// \brief Set coordinates of fixed feature to the values where it has been dragged.
  ///        Useful when the feature is being moved.
  void moveFeature();
#endif

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process();

  /// \brief Generate list of attributes of constraint in order useful for constraints
  /// \param[out] theValue      numerical characteristic of constraint (e.g. distance)
  /// \param[out] theAttributes list of attributes to be filled
  virtual void getAttributes(EntityWrapperPtr& , std::vector<EntityWrapperPtr>& )
  {}

#ifdef SUPPORT_NEW_MOVE
  /// \brief Obtain entity to be fixed
  virtual EntityWrapperPtr entityToFix();
#else
  /// \brief Generate list of attributes of constraint in order useful for constraints
  /// \param[out] theBaseEntity  the entity which coordinates should be fixed
  /// \param[out] theFixedEntity the entity containing fixed values
  virtual void getAttributes(EntityWrapperPtr& theBaseEntity,
                             EntityWrapperPtr& theFixedEntity);
#endif

  /// \brief Create Fixed constraint for the feature basing on its type
  /// \param theFeature [in]  feature, converted to solver specific format
  /// \return Fixed constraint
  virtual ConstraintWrapperPtr fixFeature(EntityWrapperPtr theFeature);

#ifndef SUPPORT_NEW_MOVE
public:
#endif
  /// \brief Get list of parameters of current entity
  static GCS::VEC_pD toParameters(const EntityWrapperPtr& theEntity);

#ifdef SUPPORT_NEW_MOVE
protected:
#else
private:
  FeaturePtr myBaseFeature; ///< fixed feature (when it is set, myBaseConstraint should be NULL)

  ConstraintWrapperPtr myConstraint;
  EntityWrapperPtr    myFixedEntity;
#endif
  std::vector<double> myFixedValues;
};

#endif
