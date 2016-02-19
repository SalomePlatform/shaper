// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_Constraint.h
// Created: 27 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_Constraint_H_
#define SketchSolver_Constraint_H_

#include "SketchSolver.h"
#include <SketchSolver_Storage.h>

#include <SketchPlugin_Constraint.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefAttr.h>

#include <string>
#include <vector>

/** \class   SketchSolver_Constraint
 *  \ingroup Plugins
 *  \brief   Converts SketchPlugin constraint to the constraint applicable for solver
 */
class SketchSolver_Constraint
{
protected:
  /// Default constructor
  SketchSolver_Constraint()
    : myGroupID(GID_UNKNOWN),
      myType(CONSTRAINT_UNKNOWN)
  {}

public:
  /// Constructor based on SketchPlugin constraint
  SKETCHSOLVER_EXPORT SketchSolver_Constraint(ConstraintPtr theConstraint);

  virtual ~SketchSolver_Constraint() {}

  /// \brief Initializes parameters and start constraint creation
  /// \param theStorage  [in]  storage where to place new constraint
  /// \param theGroupID  [in]  group for constraint
  /// \param theSketchID [in] sketch for constraint
  void process(StoragePtr theStorage, const GroupID& theGroupID, const EntityID& theSketchID);

  /// \brief Update constraint
  SKETCHSOLVER_EXPORT virtual void update();

  /// \brief Tries to remove constraint
  /// \return \c false, if current constraint contains another SketchPlugin constraints (like for multiple coincidence)
  SKETCHSOLVER_EXPORT virtual bool remove();

  /// \brief Obtain a type of SketchPlugin constraint
  SKETCHSOLVER_EXPORT static SketchSolver_ConstraintType TYPE(ConstraintPtr theConstraint);

  /// \brief Returns the type of constraint
  virtual SketchSolver_ConstraintType getType() const
  { return myType; }

  /// \brief Verify the feature or any its attribute is used by constraint
  virtual bool isUsed(FeaturePtr theFeature) const;
  /// \brief Verify the attribute is used by constraint
  virtual bool isUsed(AttributePtr theAttribute) const;

  /// \brief Shows error message
  const std::string& error() const
  { return myErrorMsg; }

protected:
  /// \brief Converts SketchPlugin constraint to a list of solver's constraints
  virtual void process();

  /// \brief Generate list of attributes of constraint in order useful for constraints
  /// \param[out] theValue      numerical characteristic of constraint (e.g. distance)
  /// \param[out] theAttributes list of attributes to be filled
  virtual void getAttributes(double& theValue, std::vector<EntityWrapperPtr>& theAttributes);

  /// \brief This method is used in derived objects to check consistency of constraint.
  ///        E.g. the distance between line and point may be signed.
  virtual void adjustConstraint()
  {}

  /// \brief Removes last error
  void cleanErrorMsg()
  { myErrorMsg.clear(); }

protected:
  GroupID       myGroupID;  ///< identifier of the group, the constraint belongs to
  EntityID      mySketchID; ///< identifier of the sketch, the constraint belongs to
  ConstraintPtr myBaseConstraint; ///< base SketchPlugin constraint
  StoragePtr    myStorage; ///< storage, which contains all information about entities and constraints
  SketchSolver_ConstraintType myType; ///< type of constraint

  std::string   myErrorMsg; ///< error message
};

typedef std::shared_ptr<SketchSolver_Constraint> SolverConstraintPtr;

#endif
