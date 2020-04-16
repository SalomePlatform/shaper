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

#ifndef SolveSpaceSolver_ConstraintWrapper_H_
#define SolveSpaceSolver_ConstraintWrapper_H_

#include <SketchSolver_IConstraintWrapper.h>
#include <SolveSpaceSolver_Solver.h>


/**
 *  Wrapper providing operations with SovleSpace constraints.
 */
class SolveSpaceSolver_ConstraintWrapper : public SketchSolver_IConstraintWrapper
{
public:
  SolveSpaceSolver_ConstraintWrapper(const ConstraintPtr&   theOriginal,
                                     const Slvs_Constraint& theConstraint);

  /// \brief Return SolveSpace constraint
  const Slvs_Constraint& constraint() const
  { return mySlvsConstraint; }
  /// \brief Return SolveSpace constraint to change
  Slvs_Constraint& changeConstraint()
  { return mySlvsConstraint; }

  /// \brief Return ID of current entity
  virtual ConstraintID id() const;

  /// \brief Change group for the constraint
  virtual void setGroup(const GroupID& theGroup);
  /// \brief Return identifier of the group the constraint belongs to
  virtual GroupID group() const
  { return (GroupID)mySlvsConstraint.group; }

  /// \brief Return type of current entity
  virtual SketchSolver_ConstraintType type() const;

  /// \brief Verify the feature is used in the constraint
  virtual bool isUsed(FeaturePtr theFeature) const;
  /// \brief Verify the attribute is used in the constraint
  virtual bool isUsed(AttributePtr theAttribute) const;

  /// \brief Compare current constraint with other
  virtual bool isEqual(const ConstraintWrapperPtr& theOther);

  /// \brief Update values of parameters of this constraint by the parameters of given one
  /// \return \c true if some parameters change their values
  virtual bool update(const std::shared_ptr<SketchSolver_IConstraintWrapper>& theOther);

private:
  Slvs_Constraint mySlvsConstraint;
};

#endif
