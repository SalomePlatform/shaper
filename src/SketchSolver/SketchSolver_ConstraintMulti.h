// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintMulti.h
// Created: 2 Sep 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintMulti_H_
#define SketchSolver_ConstraintMulti_H_

#include "SketchSolver.h"
#include <SketchSolver_Constraint.h>

#include <vector>

/** \class   SketchSolver_ConstraintMulti
 *  \ingroup Plugins
 *  \brief   Common base class for the Multi constraints
 */
class SketchSolver_ConstraintMulti : public SketchSolver_Constraint
{
public:
  SketchSolver_ConstraintMulti(ConstraintPtr theConstraint) :
      SketchSolver_Constraint(theConstraint),
      myNumberOfObjects(0),
      myNumberOfCopies(0),
      myAdjusted(false)
  {}

  virtual int getType() const
  { return SLVS_C_UNKNOWN; }

  /// \brief Update constraint
  virtual void update(ConstraintPtr theConstraint = ConstraintPtr());

  /// \brief Tries to remove constraint
  /// \return \c false, if current constraint contains another SketchPlugin constraints (like for multiple coincidence)
  virtual bool remove(ConstraintPtr theConstraint = ConstraintPtr());

  /// \brief Adds a feature to constraint and create its analogue in SolveSpace
  virtual void addFeature(FeaturePtr theFeature);

  /// \brief Update SketchPlugin attributes using the data obtained from SolveSpace entities
  virtual void refresh()
  {
    myAdjusted = false;
    SketchSolver_Constraint::refresh();
  }

  /// \brief Verifies, the coincidence between points of copied entities appears or disappears,
  ///        and removes or adds fixing of corresponding points.
  void checkCoincidence();

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process()
  { /* do nothing here */ }

  /// \brief Collect entities and their copies, like circles and arcs
  void processEntities(const std::vector< std::vector<Slvs_hEntity> >& theEntAndCopies);

  /// \brief Generate list of attributes of constraint in order useful for SolveSpace constraints
  /// \param[out] theValue      numerical characteristic of constraint (e.g. distance)
  /// \param[out] theAttributes list of attributes to be filled
  virtual void getAttributes(double& theValue, std::vector<Slvs_hEntity>& theAttributes)
  { /* do nothing here */ }

  /// \brief This method is used in derived objects to check consistence of constraint.
  virtual void adjustConstraint();

  /// \brief Update parameters of derived classes
  virtual void updateLocal() = 0;

  /// \brief Returns name of NUMBER_OF_COPIES parameter for corresponding feature
  virtual const std::string& nameNbCopies() = 0;

protected:
  /// \brief Convert absolute coordinates to relative coordinates
  virtual void getRelative(double theAbsX, double theAbsY, double& theRelX, double& theRelY) = 0;
  /// \brief Convert relative coordinates to absolute coordinates
  virtual void getAbsolute(double theRelX, double theRelY, double& theAbsX, double& theAbsY) = 0;
  /// \brief Apply transformation for relative coordinates
  virtual void transformRelative(double& theX, double& theY) = 0;

protected:
  size_t myNumberOfObjects; ///< number of previous initial objects
  size_t myNumberOfCopies;  ///< number of previous copies of initial objects

  std::vector< std::vector<Slvs_hEntity> > myPointsAndCopies; ///< list of initial points and their copies
  std::vector< std::vector<Slvs_hEntity> > myCircsAndCopies;  ///< list of circles and their copies (to change their radii together)

  std::set<Slvs_hEntity> myPointsJustUpdated; ///< list of points touched by user
  std::set<Slvs_hEntity> myInitialPoints;     ///< list of points containing initial objects

  bool myAdjusted; ///< the constraint is already adjusted (to not do it several times)
};

#endif
