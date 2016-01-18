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
  /// Constructor based on SketchPlugin constraint
  SketchSolver_ConstraintMulti(ConstraintPtr theConstraint) :
      SketchSolver_Constraint(theConstraint),
      myNumberOfObjects(0),
      myNumberOfCopies(0),
      myAdjusted(false)
  {}

  /// \brief Update constraint
  virtual void update();
  /// \brief Update constraint
  void update(bool isForce);

  /// \brief Tries to remove constraint
  /// \return \c false, if current constraint contains another SketchPlugin constraints (like for multiple coincidence)
  virtual bool remove();

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process()
  { /* do nothing here */ }

  /// \brief Collect entities which translated or rotated (not their copies)
  void getEntities(std::list<EntityWrapperPtr>& theEntities);

  /// \brief Generate list of attributes of constraint in order useful for SolveSpace constraints
  /// \param[out] theValue      numerical characteristic of constraint (e.g. distance)
  /// \param[out] theAttributes list of attributes to be filled
  virtual void getAttributes(double& theValue, std::vector<EntityWrapperPtr>& theAttributes)
  { /* do nothing here */ }

  /// \brief This method is used in derived objects to check consistence of constraint.
  virtual void adjustConstraint();

  /// \brief Update parameters of derived classes
  virtual void updateLocal() = 0;

  /// \brief Returns name of NUMBER_OF_COPIES parameter for corresponding feature
  virtual const std::string& nameNbObjects() = 0;
  
protected:
  /// \brief Convert absolute coordinates to relative coordinates
  virtual void getRelative(double theAbsX, double theAbsY, double& theRelX, double& theRelY) = 0;
  /// \brief Convert relative coordinates to absolute coordinates
  virtual void getAbsolute(double theRelX, double theRelY, double& theAbsX, double& theAbsY) = 0;
  /// \brief Apply transformation for relative coordinates
  virtual void transformRelative(double& theX, double& theY) = 0;

protected:
  int myNumberOfObjects; ///< number of previous initial objects
  int myNumberOfCopies;  ///< number of previous copies of initial objects

  bool myAdjusted; ///< the constraint is already adjusted (to not do it several times)

  std::set<FeaturePtr> myFeatures; ///< list of features and their copies to find whether some of them are disappeared
};

#endif
