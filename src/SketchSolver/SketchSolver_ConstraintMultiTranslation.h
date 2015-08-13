// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintMultiTranslation.h
// Created: 1 Apr 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintMultiTranslation_H_
#define SketchSolver_ConstraintMultiTranslation_H_

#include "SketchSolver.h"
#include <SketchSolver_Constraint.h>

/** \class   SketchSolver_ConstraintMultiTranslation
 *  \ingroup Plugins
 *  \brief   Convert translated features to the list of SolveSpace constraints
 */
class SketchSolver_ConstraintMultiTranslation : public SketchSolver_Constraint
{
public:
  SketchSolver_ConstraintMultiTranslation(ConstraintPtr theConstraint) :
      SketchSolver_Constraint(theConstraint),
      myNumberOfObjects(0),
      myNumberOfCopies(0),
      myTranslationLine(SLVS_E_UNKNOWN)
  {}

  virtual int getType() const
  { return SLVS_C_MULTI_TRANSLATION; }

  /// \brief Update constraint
  virtual void update(ConstraintPtr theConstraint = ConstraintPtr());

  /// \brief Tries to remove constraint
  /// \return \c false, if current constraint contains another SketchPlugin constraints (like for multiple coincidence)
  virtual bool remove(ConstraintPtr theConstraint = ConstraintPtr());

  /// \brief Adds a feature to constraint and create its analogue in SolveSpace
  virtual void addFeature(FeaturePtr theFeature);

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process();

  /// \brief Generate list of attributes of constraint in order useful for SolveSpace constraints
  /// \param[out] theValue      numerical characteristic of constraint (e.g. distance)
  /// \param[out] theAttributes list of attributes to be filled
  virtual void getAttributes(double& theValue, std::vector<Slvs_hEntity>& theAttributes)
  { /* do nothing here */ }

  /// \brief Generate list of translated entities
  /// \param[out] theStartPoint ID of start point of translation
  /// \param[out] theEndPoint   ID of final point of translation
  /// \param[out] thePoints     list of IDs of initial points and their translated copies
  /// \param[out] theEntities   list of IDs of entities and their translated copies
  void getAttributes(Slvs_hEntity& theStartPoint, Slvs_hEntity& theEndPoint,
                     std::vector< std::vector<Slvs_hEntity> >& thePoints,
                     std::vector< std::vector<Slvs_hEntity> >& theEntities);

  /// \brief This method is used in derived objects to check consistence of constraint.
  virtual void adjustConstraint();

private:
  size_t myNumberOfObjects; ///< number of previous initial objects
  size_t myNumberOfCopies;  ///< number of previous copies of initial objects
  Slvs_hEntity myTranslationLine; ///< ID of translation line
  std::vector< std::vector<Slvs_hEntity> > myPointsAndCopies; ///< list of initial points and their translated copies
  std::vector< std::vector<Slvs_hEntity> > myCircsAndCopies;  ///< list of circles and their copies (to change their radii together)

  std::set<Slvs_hEntity> myPointsJustUpdated; ///< list of points touched by user
};

#endif
