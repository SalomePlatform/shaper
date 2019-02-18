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

#ifndef SketchSolver_ConstraintCollinear_H_
#define SketchSolver_ConstraintCollinear_H_

#include <SketchSolver_Constraint.h>

/** \class   SketchSolver_ConstraintCollinear
 *  \ingroup Plugins
 *  \brief   Converts collinear constraint to the constraint applicable for solver
 */
class SketchSolver_ConstraintCollinear : public SketchSolver_Constraint
{
public:
  /// Constructor based on SketchPlugin constraint
  SketchSolver_ConstraintCollinear(ConstraintPtr theConstraint)
    : SketchSolver_Constraint(theConstraint),
      myInSolver(false)
  {
    for (int i = 0; i < 4; ++i)
      myIsConstraintApplied[i] = false;
  }

  /// \brief Notify this object about the feature is changed somewhere
  virtual void notify(const FeaturePtr&      theFeature,
                      PlaneGCSSolver_Update* theUpdater);

protected:
  /// \brief Converts SketchPlugin constraint to a list of solver constraints
  virtual void process();

private:
  EntityWrapperPtr myPoints[4];  ///< extremities on collinear lines
  bool myIsConstraintApplied[4]; ///< set \c true if point on opposite line
  bool myInSolver; ///< the constraint is added to the solver
};

#endif
